//------------------------------------------------------------------------------
/// Copyright (c) 2014-2020 WAGO Kontakttechnik GmbH & Co. KG
///
/// PROPRIETARY RIGHTS of WAGO Kontakttechnik GmbH & Co. KG are involved in
/// the subject matter of this material. All manufacturing, reproduction,
/// use, and sales rights pertaining to this subject matter are governed
/// by the license agreement. The recipient of this software implicitly
/// accepts the terms of the license.
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
///
///  \brief    Configuration tool to configure DNS and DHCP service.
///
///  \author   HJH, WAGO Kontakttechnik GmbH & Co. KG.
///  \author   MSc, WAGO Kontakttechnik GmbH & Co. KG.
///  \author   MOe, WAGO Kontakttechnik GmbH & Co. KG.
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Include files
//------------------------------------------------------------------------------

#include "config_dnsmasq.hpp"

#include <algorithm>
#include <cstdio>
#include <cassert>
#include <cstring>
#include <getopt.h>
#include <cstdlib>
#include <unistd.h>
#include <cctype>              // for isdigit()
#include <cerrno>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/file.h>           // for flock()
#include <fcntl.h>
#include <net/if.h>
#include <ctime>
#include <libgen.h>             // for basename()
#include <ct_xml.h>
#include <ct_libnet.h>
#include <boost/algorithm/string.hpp>
#include <boost/format.hpp>
#include <sstream>

#include "error_handling.hpp"
#include "utilities.hpp"
#include "defines.hpp"
#include "DhcpRange.hpp"
#include "network_config.hpp"
#include "FileEditor.hpp"

#define NETWORK_SERVICES_XML    "/etc/specific/network-services.xml"
#define ETC_HOSTS               "/etc/hosts"
#define DNSMASQ_CONF            "/etc/dnsmasq.conf"
#define DNSMASQ_CONF_TMP        "/etc/dnsmasq.conf.tmp"
#define DNSMASQ_RESTART_CMD     "/etc/init.d/dnsmasq restart"
#define DNSMASQ_LEASE_FILE      "/var/lib/misc/dnsmasq.leases"
#define DNS_DEFAULT_CACHE       500
#define MAX_STATIC_HOSTS           15


namespace {

  class DnsmasqParameter {
   public:
    std::string name_;
    std::string value_;

    DnsmasqParameter(std::string name, std::string value)
        :
        name_ { name },
        value_ { value } {
    }
  };

  //------------------------------------------------------------------------------
  // SECTION File handling routines for plain files.
  //------------------------------------------------------------------------------

  // Plain file handling for limited file size (< INT_MAX).
  // Sizes are held in integers
  // fh_open() delivers structure with file size, modification time, file descriptor.

  #define FH_MAX_FILESIZE         (1024 * 1024 * 1024)  // 1GByte maximum file size.

  typedef struct plainfile {
    int           fd;
    int           filesize;
    struct stat   stat;
    ::std::string filepath;

    plainfile() :
      fd{-1}, filesize{-1}, filepath{""} {

    }

  } plainfile_t;

  static ::std::string error_message;
  static int error_type;
  static int error_number;

  bool isAnyFileOpenError(int e) {
    return e == EACCES || e == ENOENT || e == EISDIR || e == ENODEV || e == ELOOP;
  }

  void set_error(const int type, const int number, const std::string &message) {
    error_type = type;
    error_number = number;
    error_message = message + ": " + strerror(number);
  }

  void set_error(const int number, const std::string &message) {
    auto type = isAnyFileOpenError(number) ? FILE_OPEN_ERROR : SYSTEM_CALL_ERROR;
    set_error(type, number, message);
  }

  enum FileAccessMode {
    READ = 'r',
    WRITE = 'w'
  };

  /**
   * Opens a file for reading or writing.
   * Mode must be a string with one character. The meaning of the character is as follows:
   * r = Open file for reading.
   * w = Open file for writing, create a new file if file does not exist, truncate an existing file to length 0.
   * Error handling: Returns NULL on errors. Errorcode in fh_errorcode, error message in fh_errortxt and
   * system errno in fh_errno.
   *
   * @param path
   * @param mode
   * @return
   */
  plainfile_t fh_open(const ::std::string &path, const FileAccessMode &mode) {
    plainfile_t file;
    int oflags = (mode == WRITE) ? (O_CREAT | O_WRONLY | O_TRUNC) : O_RDONLY;

    file.filepath = path;
    error_type = 0;
    file.fd = open(path.c_str(), oflags, 0644);
    if (file.fd < 0) {
      set_error(errno, "Error on open " + path);
    }

    if (file.fd >= 0 && oflags == O_RDONLY) {
      int rc = fstat(file.fd, &file.stat);
      if (rc == 0) {
        if (FH_MAX_FILESIZE >= file.stat.st_size) {
          file.filesize = (int)file.stat.st_size;
        } else {
          set_error(SYSTEM_CALL_ERROR, 0, "Maximum file size exceeded on %s" + path);
          close(file.fd);
        }
      } else {
        set_error(SYSTEM_CALL_ERROR, errno, "Error on fstat of " + path);
        close(file.fd);
      }
    }
    return file;
  }

  int fh_read(plainfile_t &pfp, ::std::string &data) {
    ssize_t rc;
    char* buffer = (char*) g_malloc0(pfp.filesize + 1);

    rc = read(pfp.fd, buffer, pfp.filesize + 1);
    if (rc < 0) {
      set_error(FILE_READ_ERROR, errno, "Error on read " + pfp.filepath);
      data = "";
    }
    data = buffer;
    g_free(buffer);
    return rc;
  }

  int fh_write(plainfile_t &pfp, const void *data, size_t size) {
    if (size <= 0) {
      return 0;
    }

    ssize_t sz = write(pfp.fd, data, size);
    if (sz != (ssize_t)size) {
      if (errno == ENOSPC) {
        error_type = NOT_ENOUGH_SPACE_ERROR;
      } else if (errno == EIO) {
        error_type = FILE_WRITE_ERROR;
      }
      error_number = errno;
      error_message = "Error on write to " + pfp.filepath + ": " + strerror(errno);
      sz = -1;
    }

    return sz;
  }

  void fh_sync(plainfile_t &pfp) {
    int status = fdatasync(pfp.fd);
    erh_assert(0 == status, SYSTEM_CALL_ERROR, boost::format("Sync failed with error %s") % strerror(errno), false);
  }

  // operation is LOCK_SH or LOCK_EX or LOCK_UN
  int fh_lock(plainfile_t &pfp, int operation) {
    int rc = flock(pfp.fd, operation);
    if (rc) {
      set_error(errno, "Failed to aquire/free lock for " + pfp.filepath);
    }
    return rc;
  }

  void fh_close(plainfile_t &pfp) {
    if (pfp.fd >= 0) {
      close(pfp.fd);
      pfp.fd = -1;
    }
  }

  //------------------------------------------------------------------------------
  // SECTION File handling of whole files.
  //------------------------------------------------------------------------------

  // Read file, consider raise condition where file size changed.
  eStatusCode cfh_readfile(::std::string &file_content, const ::std::string &path) {
    eStatusCode status = SUCCESS;
    int readsize = 0;
    int retries = 3;
    plainfile_t pfh;

    do {
      if(pfh.fd >= 0){
        fh_close(pfh);
      }
      pfh = fh_open(path, READ);
      if (pfh.fd < 0) {
        readsize = 0;
        status = FILE_OPEN_ERROR;
        break;
      }
      readsize = fh_read(pfh, file_content);
      if (readsize != pfh.filesize) {
        // File size changed. Read again.
        fh_close(pfh);
        retries--;
        if (retries <= 0) {
          readsize = 0;
          status = FILE_READ_ERROR;
          break;
        }
      }
      file_content[readsize] = '\0';
    } while (readsize != pfh.filesize);
    fh_close(pfh);

    return status;
  }

  //------------------------------------------------------------------------------
  // SECTION Check routines for XML file related strings.
  //------------------------------------------------------------------------------

  // Structure of an entry in the parameter name list.
typedef struct xml_param {
  ::std::string pname_;    // Parameter name.
  ::std::string pjname_;   // Parameter name for JSON output.
  service_t pservice_;     // Kind of service.
  bool pext_;              // Name visible on command line.
  bool pmulti_;            // Multiple values possible.
  ::std::string pxmlname_;  // Name in XML file.
  void (*pcheckfct_)(const ::std::string &v);  // Function to check a config parameter.

  xml_param()
      :
      pname_ { "" },
      pjname_ { "" },
      pservice_ { UNDEF },
      pext_ { false },
      pmulti_ { false },
      pxmlname_ { "" } {
  }

  xml_param(::std::string pname, ::std::string pjname, service_t pservice, bool pext, bool pmulti,
            ::std::string pxmlname, void (*pcheckfct)(const ::std::string &v))
      :
      pname_ { pname },
      pjname_ { pjname },
      pservice_ { pservice },
      pext_ { pext },
      pmulti_ { pmulti },
      pxmlname_ { pxmlname },
      pcheckfct_ { pcheckfct } {
  }
} xml_param_t;

  // Forward declarations of check function names, used in xml_param_name_list.
  static void xml_check_dnsstate(const ::std::string &value);
  static void xml_check_dnsmode(const ::std::string &value);
  static void xml_check_dnshost(const ::std::string &value);
  static void xml_check_dhcpdstate(const ::std::string &value);
  static void xml_check_dhcpdrange(const ::std::string &value);
  static void xml_check_dhcpdlease(const ::std::string &value);
  static void xml_check_dhcpdhost(const ::std::string &value);
  static void xml_check_nop(const ::std::string &value);

  // This table defines all dnsmasq configuration parameter names as they can be get/set by xml_*().
  // Forth column true means that name is used on the command line.
  // Each parameter name is associated with name in the XML config file.
  static ::std::list<xml_param_t> xml_param_name_list = {
      { "dns-state",        "dnsState",       DNS,   true,  false, "state",        xml_check_dnsstate },
      { "dns-mode",         "dnsMode",        DNS,   true,  false, "mode",         xml_check_dnsmode },
      { "dns-fix-host",     "dnsFixHost",     DNS,   true,  true,  "fix_hostname", xml_check_dnshost },
      { "dhcpd-state",      "dhcpdState",     DHCPD, true,  false, "state",        xml_check_dhcpdstate },
      { "dhcpd-range",      "dhcpdRange",     DHCPD, true,  false, "range",        xml_check_dhcpdrange },
      { "dhcpd-lease-time", "dhcpdLeaseTime", DHCPD, true,  false, "lease_time",   xml_check_dhcpdlease },
      { "dhcpd-fix-host",   "dhcpdFixHost",   DHCPD, true,  true,  "fix_host",     xml_check_dhcpdhost },
      { "ip_addr",          "",               DHCPD, false, false, "ip_addr",      xml_check_nop },
      { "netmask",          "",               DHCPD, false, false, "netmask",      xml_check_nop },
      { "port-name",        "",               DHCPD, false, false, "port_name",    xml_check_nop },
  };

//  static xml_param_t *xml_param_name_list_end =
//                     &xml_param_name_list[sizeof(xml_param_name_list) / sizeof(xml_param_name_list[0])];

  bool xml_check_condition(bool cond, bool status) {
    return cond && status;
  }

  void xml_check_dnsstate(const ::std::string &value) {
    bool result = (value == "enabled" || value == "disabled");
    erh_assert(result, INVALID_PARAMETER, "Invalid DNS state.");
  }

  void xml_check_dnsmode(const ::std::string &value) {
    bool result = (value == "relay" || value == "proxy");
    erh_assert(result, INVALID_PARAMETER, "Invalid DNS mode.");
  }

  // value example: 192.168.0.2:hostname,192.168.0.4:hostname2_hostname3
  void xml_check_dnshost(const ::std::string &value) {

    if (value.empty()) {
      return;
    }

    bool status = true;

    ::std::vector < ::std::string > entries;
    ::boost::split(entries, value, boost::is_any_of(","));

    if (entries.size() > MAX_STATIC_HOSTS) {
      erh_assert(entries.size() <= MAX_STATIC_HOSTS, INVALID_PARAMETER, "Max number of dns static hosts exceeded.");
    }
    for (guint i = 0; i < entries.size(); i++) {
      // Check for double entries.
      for (guint k = 0; k < i; k++) {
        erh_assert(entries[k] != entries[i], INVALID_PARAMETER,
                   boost::format("Double DNS static host value %s") % entries[k].c_str());
      }

      ::std::vector < ::std::string > ip_hosts;
      ::boost::split(ip_hosts, entries[i], boost::is_any_of(":"));
      erh_assert(ip_hosts.size() == 2, INVALID_PARAMETER, "Illegal dns-fix-host format");

      ::std::vector < ::std::string > hostlist;
      ::boost::split(hostlist, ip_hosts[1], boost::is_any_of("_"));

      status = xml_check_condition(!hostlist.empty(), status);
      if (!status) {
        erh_assert(status, INVALID_PARAMETER, "Missing hostname value.");
      }

      status = xml_check_condition(utilities::is_valid_ip_string(ip_hosts[0]), status);
      if (!status) {
        erh_assert(status, INVALID_PARAMETER, "Invalid IP address value.");
      }

      if (status) {
        for (auto &name : hostlist) {
          status = xml_check_condition(utilities::conv_is_valid_domain_name(name), status);
          if (!status) {
            erh_assert(status, INVALID_PARAMETER, boost::format("Invalid hostname value: %s.") % name.c_str());
          }
        }
      }
      erh_assert(status, INVALID_PARAMETER, "Invalid DNS fix host value.");
    }
  }

  void xml_check_dhcpdstate(const ::std::string &value) {
    bool result;
    result = (value == "enabled" || value == "disabled");
    erh_assert(result, INVALID_PARAMETER, "Invalid DHCPD state.");
  }

  // Check the format of an ip address range.
  // Example value: 192.168.3.50_192.168.3.150
  void xml_check_dhcpdrange(const ::std::string& value) {
    bool result = true;
    ::std::vector<::std::string> ip_addrs;
    ::boost::split(ip_addrs, value, boost::is_any_of("_"));

    result = xml_check_condition(ip_addrs.size() == 2, result);
    result = xml_check_condition(utilities::is_valid_ip_string(ip_addrs[0]), result);
    result = xml_check_condition(utilities::is_valid_ip_string(ip_addrs[1]), result);

    erh_assert(result, INVALID_PARAMETER, "Invalid DHCP range.");
  }

  // Check DHCPD lease time value.
  // Example value: 150
  void xml_check_dhcpdlease(const ::std::string &value) {

    bool result = !value.empty() && std::find_if(value.begin(),
              value.end(), [](unsigned char c) { return !std::isdigit(c); }) == value.end();

    erh_assert(result, INVALID_PARAMETER, "Invalid DHCPD lease time value.");
  }
  
  // Example value: 01:02:03:04:05:06_192.168.0.44,hostname_192.168.0.44.
  // Value may be the empty string.
  void xml_check_dhcpdhost(const ::std::string &value) {
    if (value.empty()) {
      return;
    }

    ::std::vector<::std::string> entries;
    ::boost::split(entries, value, boost::is_any_of(","));

    erh_assert(entries.size() <= MAX_STATIC_HOSTS, INVALID_PARAMETER, "Max number of dhcp static hosts exceeded.");
    for(auto& entry : entries) {
      bool result = true;
      ::std::vector<::std::string> mac_ip;
      ::boost::split(mac_ip, entry, ::boost::is_any_of("_"));
      result = xml_check_condition(mac_ip.size() == 2, result);

      if (result && !utilities::conv_is_valid_mac(mac_ip[0])) {
        result = xml_check_condition(utilities::conv_is_valid_domain_name(mac_ip[0]), result);
      }
      if (result) {
        result = xml_check_condition(0 != utilities::is_valid_ip_string(mac_ip[1]), result);
      }
      erh_assert(result, INVALID_PARAMETER, "Invalid DHCP fix host config parameter.");
    }
  }

  void xml_check_nop(const ::std::string &value) {
    (void) value;
  }

  // Find parameter structure for name parname, service serv and external mode ext.
  // Failed if pp->name == NULL.
  xml_param_t xml_find_param(const ::std::string &parname, service_t serv, bool extonly) {
    ::std::list<xml_param_t>::iterator it = ::std::find_if(xml_param_name_list.begin(), xml_param_name_list.end(),
      [&](xml_param_t &p) {
        return parname == p.pname_ && serv == p.pservice_ && (extonly ? p.pext_ : true);
    });

    return it != xml_param_name_list.end() ? *it : xml_param_t{};
  }

  // Check if config parameter name is valid, given a service type and the external mode.
  void xml_check_name(const ::std::string &parname, service_t serv, bool ext) {
    auto pp = xml_find_param(parname, serv, ext);
    erh_assert(!pp.pname_.empty(), INVALID_PARAMETER, boost::format("Unknown config parameter name '%s'") % parname.c_str());
  }

  // Check config parameter values.
  // Calls one of the check functions xml_check_dnsmode(), xml_check_dnshost(), xml_check_dhcpdstate(),
  // xml_check_dhcpdrange(), xml_check_dhcpdlease(), xml_check_dhcpdhost(),  xml_check_nop() via function pointer pcheckfct().
  void xml_check_values(const ::std::string& parname, const ::std::string& value, const service_t service) {
    xml_param_t pp = xml_find_param(parname, service, true);
    assert(pp.pname_ != "");
    pp.pcheckfct_(value);
  }

  //------------------------------------------------------------------------------
  // SECTION XML file and XML memory data handling.
  //------------------------------------------------------------------------------

  // XML file session handle.
  typedef struct {
      xmlSession_t *xml_session;
      bool modified;                      // XML data modified, need to write back.
  } dnsmasq_handle_t;

  // Initialize xml file handling. This function and the following ones accept configuration parameter names
  // as defined in the table xml_param_name_list.
  dnsmasq_handle_t ct_dnsmasq_start_session(::std::string &xml_file) {
    int status;

    dnsmasq_handle_t handle;
    handle.xml_session = NULL;
    handle.modified = false;
    if (SUCCESS != (status = ct_xml_start_session(xml_file.c_str(), &(handle.xml_session)))) {
      erh_set_error((eStatusCode)status, "XML config file missing or wrong format", "");
    }
    return handle;
  }

  void ct_dnsmasq_finish_session(dnsmasq_handle_t &handle) {
    if(nullptr != handle.xml_session) {
      (void)ct_xml_finish_session(handle.xml_session);
      ct_xml_permanent_close();
    }
    handle.xml_session = nullptr;
  }

  // Save changes to disk. ct_xml_commit_xml_changes first writes to a temporary file which
  // is renamed to the original file afterwards.
  void ct_dnsmasq_save_xml_file(dnsmasq_handle_t &handle) {
    if (handle.modified) {
      int status = ct_xml_commit_xml_changes(handle.xml_session);
      erh_assert(SUCCESS == status, (eStatusCode)status, "Error on writing XML file to disk.");
    }
    handle.modified = false;
  }

  // Builds an xpath of the config parameter.
  ::std::string xml_make_xpath(xml_param_t &pp, service_t service, const ::std::string &port) {
    if (service == DNS) {
      return ::std::string {"//dns/" + pp.pxmlname_};
    }
    return ::std::string {"//dhcpd[port_name = '" + port + "']/" + pp.pxmlname_};
  }

  int get_value_from_xml(std::string &retval, dnsmasq_handle_t &handle, service_t service, const ::std::string &port,
           const ::std::string &parname) {
    int status;
    auto pp = xml_find_param(parname, service, false);
    auto xpath = xml_make_xpath(pp, service, port);

    erh_assert(!pp.pname_.empty(), INVALID_PARAMETER, boost::format("Unknown config parameter name %s.") % parname.c_str());
    if (pp.pmulti_) {
      GString *parlist = g_string_new("");
      status = ct_xml_get_multiple_values(xpath.c_str(), parlist, ",", handle.xml_session);
      retval = ::std::string { parlist->str };
      g_string_free(parlist, true);
    } else {
      char *ret = nullptr;
      status = ct_xml_dup_value(xpath.c_str(), &ret, handle.xml_session);
      retval = ::std::string { (ret != nullptr) ? ret : "" };
      g_free(ret);
    }

    return status;
  }

  // Get a value from the XML config file. If defaultval is not NULL, return that value in case
  // the parameter is not available. Otherwise terminate with an error message.
  ::std::string ct_dnsmasq_get_value(
      dnsmasq_handle_t &handle, service_t service, const ::std::string &port,
      const ::std::string &parname, const ::std::string &defaultval) {
    ::std::string retval;
    int status = get_value_from_xml(retval, handle, service, port, parname);

    return (status == SUCCESS) ? retval : defaultval;
  }


  ::std::string ct_dnsmasq_get_value(
      dnsmasq_handle_t &handle, service_t service, const ::std::string &port, const ::std::string &parname) {
    ::std::string retval;
    int status = get_value_from_xml(retval, handle, service, port, parname);

    if (SUCCESS != status) {
      erh_set_error((eStatusCode)status, "XML config file error. Missing config parameter '%s'", parname.c_str());
    }

    return retval;
  }

  int ct_dnsmasq_set_multiple_values(const std::string &xpath, const std::string &values, dnsmasq_handle_t &handle) {
    int status = ct_xml_del_multiple_values(xpath.c_str(), handle.xml_session);

    if (status != SUCCESS || values.empty()) {
      return status;
    }

    std::vector<std::string> parts;
    boost::split(parts, values, boost::is_any_of(","));

    for (auto part : parts) {
      status = ct_xml_add_value(xpath.c_str(), part.c_str(), handle.xml_session);
      if (status != SUCCESS) {
        break;
      }
    }

    return status;
  }

  // Set a configuration parameter (or a list of parameters in case of fix-host lists).
  void ct_dnsmasq_set_value(dnsmasq_handle_t &handle, service_t service, const ::std::string &port,
                            const std::string &key, const std::string &value) {
    // Check if parameter is changed and set flag in handle.
    auto current_value = ct_dnsmasq_get_value(handle, service, port, key, "");
    if (current_value == value) {
      return;
    }

    handle.modified = true;
    int status = SUCCESS;
    auto xml = xml_find_param(key, service, false);
    auto xpath = xml_make_xpath(xml, service, port);

    if (xml.pmulti_) {
      status = ct_dnsmasq_set_multiple_values(xpath, value, handle);
    } else {
      status = ct_xml_set_value(xpath.c_str(), value.c_str(), handle.xml_session);
    }
    erh_assert(SUCCESS == status, (eStatusCode) status, "Cannot write to XML data.");
  }

  //------------------------------------------------------------------------------
  // SECTION Plausibility checks of DHCP / DNS configuration parameter against network interface data.
  //------------------------------------------------------------------------------

  // Adjust netmask part of all ip addresses of one port.
  // Only if DHCPD is enabled on the specified port.
  void ct_dnsmasq_adjust_net(dnsmasq_handle_t &xmlhandle, PortData &pdt) {
    ::std::string port = pdt.port_name_;
    auto state = ct_dnsmasq_get_value(xmlhandle, DHCPD, port, "dhcpd-state");

    if (state != "enabled") {
      return;
    }

    uint32_t newnetmask = 0;
    uint32_t oldnetmask = 0;
    auto status = utilities::conv_ip_ascii2bin(newnetmask, pdt.netmask_);
    erh_assert(SUCCESS == status, INVALID_PARAMETER, "Invalid netmask from network interface configuration.");
    auto netmask_str = ct_dnsmasq_get_value(xmlhandle, DHCPD, port, "netmask");
    status = utilities::conv_ip_ascii2bin(oldnetmask, netmask_str);
    erh_assert(SUCCESS == status, INVALID_PARAMETER, "Invalid netmask from DHCPD configuration.");

    if (newnetmask > oldnetmask) {
      return;
    }

    // New netmask does not shrink host range.
    // Adjust net part of dhcp range and fix ip addresses for port, if netmask or ip address changed.
    uint32_t interface_net = 0;
    status = utilities::conv_ip_ascii2bin(interface_net, pdt.ip_addr_);
    erh_assert(SUCCESS == status, INVALID_PARAMETER, "Invalid ip address from network interface configuration.");
    interface_net &= newnetmask;

    auto range_str = ct_dnsmasq_get_value(xmlhandle, DHCPD, port, "dhcpd-range");
    auto range = DhcpRange::DhcpRange::fromRangeString(range_str, oldnetmask);
    erh_assert(range.isValid(), INVALID_PARAMETER, "Invalid dhcpd range config parameter.");

    if (interface_net != range.getLowerNetwork()) {
      // Adjust DHCP range.
      range.resetNetwork(interface_net);

      auto value = range.toString();
      erh_assert(!value.empty(), INVALID_PARAMETER, "Invalid dhcpd range config parameter.");
      ct_dnsmasq_set_value(xmlhandle, DHCPD, port, "dhcpd-range", value);

      // Adjust net part of fix ip addresses.
      ::std::string hosts_str = ct_dnsmasq_get_value(xmlhandle, DHCPD, port, "dhcpd-fix-host", "");
      if(!hosts_str.empty() ) {
        ::std::vector<utilities::dhcpd_host_t> ethers;
        status = utilities::conv_hosts_ascii2bin(hosts_str, oldnetmask, ethers);
        erh_assert(SUCCESS == status, INVALID_PARAMETER, "Invalid dhcpd fix host config parameter.");
        for(auto& ether: ethers){
          ether.ip_net_part = interface_net;
        }

        value = utilities::conv_hosts_bin2ascii(ethers);
        erh_assert(!value.empty(), INVALID_PARAMETER, "Invalid dhcpd fix host config parameter.");
        ct_dnsmasq_set_value(xmlhandle, DHCPD, port, "dhcpd-fix-host", value);
      }
    }
  }

  // Check if DSA mode or netmask part of ip address changed.
  // If required, disables ports and adapt ip addresses.
  void ct_dnsmasq_check_ip_dependencies(
      IpConfiguration &data, dnsmasq_handle_t &xmlhandle, std::vector<std::string>legal_ports) {

    for (auto& port : legal_ports) {
      auto it = ::std::find(data.port_name_list.begin(), data.port_name_list.end(), port);
      if (it == data.port_name_list.end()) {
        ct_dnsmasq_set_value(xmlhandle, DHCPD, port, "dhcpd-state", "disabled");
      } else {
        auto pdt = data.getPortData(port);
        if (pdt.state_ == "disabled" || pdt.type_ != "static") {
          ct_dnsmasq_set_value(xmlhandle, DHCPD, port, "dhcpd-state", "disabled");
        } else {
          ct_dnsmasq_adjust_net(xmlhandle, pdt);
        }
        ct_dnsmasq_set_value(xmlhandle, DHCPD, port, "ip_addr", pdt.ip_addr_);
        ct_dnsmasq_set_value(xmlhandle, DHCPD, port, "netmask", pdt.netmask_);
      }
    }
  }

  /**
   * Checks that the given range is in the local subnet
   *
   * @param rangestr Range string in the syntax "<lower_ip>_<upper_ip>"
   * @param ipcfg Static ip configuration against which the range is to be checked
   * @param port The port to apply the range to
   */
  void ct_dnsmasq_check_dhcp_range(const std::string &rangestr, IpConfiguration &ip_config, const std::string &port) {
    if (!ip_config.portIsAvailable(port)) {
      return;
    }
    auto port_data = ip_config.getPortData(port);
    auto netmask = port_data.netmask_ == "0.0.0.0" ? "255.255.255.0" : port_data.netmask_;
    auto range = DhcpRange::DhcpRange::fromRangeString(rangestr, netmask);
    erh_assert(range.isValid(), INVALID_PARAMETER,
               boost::format("The dhcp range %s is invalid with netmask %s") % range.toString() % netmask);
  }

  /**
   * Checks that the specified MAC addresses are all unique
   *
   * @param hoststr String of hosts in the syntax "<mac>-<ip>,<mac>-<ip>,..."
   */
  void ct_dnsmasq_check_dhcp_hosts(const std::string &hosts_string) {
    if (hosts_string.empty()) {
      return;
    }

    uint32_t dummy_netmask = 0xFFFFFFFF;
    ::std::vector<utilities::dhcpd_host_t> hosts;
    auto status = utilities::conv_hosts_ascii2bin(hosts_string, dummy_netmask, hosts);
    erh_assert(SUCCESS == status, INVALID_PARAMETER, "Invalid DHCPD fix host parameter");

    std::sort(hosts.begin(), hosts.end(), [](auto a, auto b) { return a.mac > b.mac; });

    for (size_t i = 0; i < hosts.size() - 1; i++) {
      erh_assert(!boost::iequals(hosts.at(i).mac, hosts.at(i+1).mac), INVALID_PARAMETER, "Duplicate MAC addresses");
    }
  }

  void ct_dnsmasq_check_dhcp_parameter(
      const std::string &parameter, const std::string &value, const std::string &port, IpConfiguration &ip_config) {
    auto cannot_enable_dhcp_server = ip_config.getPortData(port).type_ != "static";
    if (cannot_enable_dhcp_server) {
      erh_assert(!(parameter == "dhcpd-state" && value == "enabled"), INVALID_PARAMETER,
                 boost::format("dhcp server cannot be enabled for port %s because its ip source is not set to static") % port);
    }

    if (parameter == "dhcpd-range") {
      ct_dnsmasq_check_dhcp_range(value, ip_config, port);
    } else if (parameter == "dhcpd-fix-host") {
      ct_dnsmasq_check_dhcp_hosts(value);
    } else if (parameter == "dhcpd-lease-time") {
      erh_assert(std::stol(value) >= 2, INVALID_PARAMETER, "The DHCP lease time has to be greater than 2");
    }
  }

  // Check one name/value pair of a config parameter name and a config parameter value.
  // For example "dhcpd-range=192.168.3.100-192.168.3.200"
  DnsmasqParameter ct_dnsmasq_parse_option(
      const std::string& option, service_t service, const std::string &port, IpConfiguration &ip_config) {
    std::vector<std::string> elements;
    boost::split(elements, option, boost::is_any_of("="));
    erh_assert(elements.size() == 2, INVALID_PARAMETER,
               boost::format("The option value %s does not have the format <name>=<value>") % option);
    auto parameter = elements[0];
    auto value = elements[1];

    xml_check_name(parameter, service, true);
    xml_check_values(parameter, value, service);

    if (service == DHCPD) {
      erh_assert(!port.empty(), MISSING_PARAMETER,
                 boost::format("To configure the DHCP parameter %s, the port option -p must be set") % parameter);
      ct_dnsmasq_check_dhcp_parameter(parameter, value, port, ip_config);
    }

    return DnsmasqParameter(parameter, value);
  }

  //------------------------------------------------------------------------------
  // SECTION Config file generation and editing and dnsmasq restarts.
  //------------------------------------------------------------------------------

  static const ::std::string etc_host_marker = "# CONFIG_TOOL_SPECIFIC\n";
  static const ::std::string etc_host_marker_description =
      "# This part of /etc/hosts starting and ending with the content of the line above\n"
      "# is written by WAGO config tools for dnsmasq DNS and DHCP service. Do not edit\n"
      "# this part. Especially do not remove the markers.\n"
      "# Add your entries above or below the markers.\n"
      "#\n"
      "";

  static const size_t etc_host_marker_size = 2*etc_host_marker.size() + etc_host_marker_description.size();

  plainfile_t lock_file(const ::std::string &path) {
    auto lockfh = fh_open(path, READ);
    erh_assert(lockfh.fd >= 0, FILE_OPEN_ERROR, error_message);

    auto rc = fh_lock(lockfh, LOCK_EX);
    erh_assert(0 == rc, SYSTEM_CALL_ERROR, error_message);

    return lockfh;
  }

  void unlock_file(plainfile_t &lock) {
    auto rc = fh_lock(lock, LOCK_UN);
    erh_assert(0 == rc, SYSTEM_CALL_ERROR, error_message);
  }

  std::string read_file(const ::std::string &path) {
    ::std::string content;
    auto status = cfh_readfile(content, path);
    erh_assert(status == SUCCESS, status, "Failed to read " + path + ".");
    return content;
  }

  void append_dns_configuration_by_ip_config(std::stringstream &stream, IpConfiguration &ipConfig) {
    for (auto port : ipConfig.port_data_list) {
      if (port.state_ == "enabled" && port.type_ == "static") {
        stream << boost::format("%-16s %s\n") % port.ip_addr_ % ipConfig.host_name;
      }
    }
  }

  void append_dns_configuration_by_ip_persistence(std::stringstream &stream, dnsmasq_handle_t &handle) {

    ::std::string hosts_str = ct_dnsmasq_get_value(handle, DNS, "", "dns-fix-host", "");

    if(!hosts_str.empty()) {
      ::std::vector<::std::string> hosts;
      boost::split(hosts, hosts_str, ::boost::is_any_of(","));

      for (auto& host : hosts) {
        ::std::vector<::std::string> ip_names;
        ::boost::split(ip_names, host, ::boost::is_any_of(":"));

        ::std::vector<::std::string> names;
        ::boost::split(names, ip_names[1], ::boost::is_any_of("_"));

        stream << boost::format("%-16s") % ip_names[0];

        for (auto& name : names) {
          stream << " " << name;
        }
        stream << "\n";
      }
    }
  }

  std::string create_dns_configuration(IpConfiguration &ipConfig, dnsmasq_handle_t &serviceFileHandle) {
    std::stringstream stream;

    stream << etc_host_marker;
    stream << etc_host_marker_description;
    append_dns_configuration_by_ip_config(stream, ipConfig);
    append_dns_configuration_by_ip_persistence(stream, serviceFileHandle);
    stream << etc_host_marker;

    auto dns_section = stream.str();
    return (dns_section.size() > etc_host_marker_size) ? dns_section : "";
  }

  void get_header_and_trailer(::std::string &header, ::std::string &trailer, const ::std::string &content) {
    header = "";
    trailer = "";

    if (content.empty()) {
      return;
    }

    size_t position_first_marker = content.find(etc_host_marker);
    size_t position_second_marker = std::string::npos;

    if (position_first_marker != std::string::npos) {
      position_second_marker = content.find(etc_host_marker, position_first_marker + etc_host_marker.size());
      erh_assert(position_second_marker != std::string::npos, CONFIG_FILE_INCONSISTENT,
                 "Inconsistent in /etc/hosts: found only one insertion marker but two are required.");

      auto header_end = position_first_marker;
      auto trailer_start = position_second_marker + etc_host_marker.size();
      header = content.substr(0, header_end);
      trailer = content.substr(trailer_start);
    }
    else {
      header = content;
    }
  }

  void insert_dns_configuration(const ::std::string &path, const ::std::string &dns_config_section) {
    auto file_lock = lock_file(path);
    ::std::string content = read_file(path);

    ::std::string header;
    ::std::string trailer;

    get_header_and_trailer(header, trailer, content);


    // Write header, config tool part, trailer to temporary file.
    int status;
    plainfile_t pfh = fh_open(path, WRITE);
    erh_assert(pfh.fd >= 0, FILE_OPEN_ERROR, error_message);

    status = fh_write(pfh, header.c_str(), header.size());
    erh_assert(status >= 0, FILE_WRITE_ERROR, error_message);

    if (!dns_config_section.empty()) {
      status = fh_write(pfh, dns_config_section.c_str(), dns_config_section.size());
      erh_assert(status >= 0, FILE_WRITE_ERROR, error_message);
    }

    status = fh_write(pfh, trailer.c_str(), trailer.size());
    erh_assert(status >= 0, FILE_WRITE_ERROR, error_message);

    fh_sync(pfh);
    fh_close(pfh);

    unlock_file(file_lock);
  }

  // Edit /etc/hosts
  // Remember /etc/hosts content up to marker ETC_HOST_MARKER and content from second marker to end.
  // Finally write first part, generated part with markers, and second part to a temporary file.
  void ct_dnsmasq_edit_etc_hosts(dnsmasq_handle_t &xmlhandle, IpConfiguration &data, const ::std::string &etchosts) {
    auto dns_config_section = create_dns_configuration(data, xmlhandle);
    insert_dns_configuration(etchosts, dns_config_section);
  }

  #define DNSMASQ_DONT_CHANGE_MARKER  "# KEEP\n"

  static char const *cfg_dnsmasq_start =
      "# This is a generated file by WAGO configuration tools for DNS and DHCP service.\n"
      "# To prevent from overwriting this file, replace the first line with # KEEP.\n"
      "# Read /etc/dnsmasq.d/dnsmasq_default.conf for meaning of configuration parameters.\n"
      "\n"
      "# DNS part\n"
      "\n"
      "";

  static char const *cfg_dnsmasq_end =
      "\n"
      "# Include default config file\n"
      "conf-file=/etc/dnsmasq.d/dnsmasq_default.conf\n"
      "conf-file=/var/run/dnsmasq/dnsmasq_gadget.conf\n"
      "";

  // Check if DNSMASQ_MARKER line is present in dnsmasq.conf.
  // Accept missing dnsmasq.conf for healing purposes.
  void ct_dnsmasq_assert_conf_not_compromised(const ::std::string &dnsmasq_conf) {
    // Read dnsmasq.conf and look for valid start marker in dnsmasq.conf.
    ::std::string dnsmasq_conf_content = "";
    cfh_readfile(dnsmasq_conf_content, dnsmasq_conf);

//    erh_assert(status == SUCCESS, status, "Failed to read file %s.", dnsmasq_conf);

    // Look for valid start marker in dnsmasq.conf. Otherwise don't touch the file.
    if (dnsmasq_conf_content.empty()) {
      return;
    }

    auto it = dnsmasq_conf_content.find(DNSMASQ_DONT_CHANGE_MARKER);

    if (it != ::std::string::npos) {
      erh_set_error(CONFIG_FILE_INCONSISTENT, "Detected manually edited dnsmasq.conf.", "");
    }
  }


  bool port_is_up(const ::std::string &port,IpConfiguration &ip_config_data) {
    for(auto& entry : ip_config_data.port_data_list) {
      if(entry.port_name_ == port) {
        return entry.state_ == "enabled";
      }
    }
    return false;
  }

  /**
   * Add DHCPD config values of one port to the GString for dnsmasq.conf.
   *
   * @param xmlhandle
   * @param port
   * @param config
   * @param ip_config_data
   */
  void append_dhcpd_port_config(std::stringstream &stream,
      dnsmasq_handle_t &handle, const std::string &port, IpConfiguration &ip_config) {

    auto dhcpd_state = ct_dnsmasq_get_value(handle, DHCPD, port, "dhcpd-state");

    if (dhcpd_state == "enabled" && port_is_up(port, ip_config)) {
      stream << "\n# Interface port " << port << "\n";
      auto dhcpd_range_str = ct_dnsmasq_get_value(handle, DHCPD, port, "dhcpd-range");

      ::std::vector<::std::string> range;
      ::boost::split(range, dhcpd_range_str, ::boost::is_any_of("_"));

      auto lease = ct_dnsmasq_get_value(handle, DHCPD, port, "dhcpd-lease-time");
      stream << "dhcp-range=" << range[0] << "," << range[1] << "," << lease << "m\n";
      stream << "interface=" << port << "\n";

      auto hosts_str = ct_dnsmasq_get_value(handle, DHCPD, port, "dhcpd-fix-host", "");
      if(!hosts_str.empty()) {
        std::vector<std::string> hosts;
        boost::split(hosts, hosts_str, boost::is_any_of(","));

        for (auto &host : hosts) {
          std::vector<std::string> mac_ip;
          boost::split(mac_ip, host, boost::is_any_of("_"));
          stream << "dhcp-host=" << mac_ip[0] << "," << mac_ip[1] << "\n";
        }
      }
    } else {
      stream << "no-dhcp-interface=" << port << "\n";
    }
  }

  /**
   * Write dnsmasq.conf
   *
   * @param xmlhandle
   * @param dnsmasq_conf_tmp
   * @param legal_ports
   * @param ip_config
   */
  void ct_dnsmasq_generate_conf(
      dnsmasq_handle_t &handle, const std::string dnsmasq_conf_tmp,
      const std::vector<std::string> legal_ports, IpConfiguration &ip_config) {

    auto pfp = fh_open(dnsmasq_conf_tmp, WRITE);
    erh_assert(pfp.fd >= 0, FILE_OPEN_ERROR, error_message);

    std::stringstream stream;
    stream << cfg_dnsmasq_start;

    auto dnsstate = ct_dnsmasq_get_value(handle, DNS, "", "dns-state");
    if (dnsstate == "enabled") {
      for (auto &port : ip_config.port_data_list) {
        if (port.state_ == "enabled") {
            stream << "interface=" << port.port_name_ << "\n";
        }
      }
    } else {
      stream << "port=0\n";
    }

    auto dnsmode = ct_dnsmasq_get_value(handle, DNS, "", "dns-mode");
    auto dnscache = (dnsmode == "relay") ? 0 : DNS_DEFAULT_CACHE;
    stream << "cache-size=" << dnscache << "\n";

    stream << "\n# DHCP part\n";
    for (auto &port : legal_ports) {
      append_dhcpd_port_config(stream, handle, port, ip_config);
    }

    stream << cfg_dnsmasq_end;
    auto rc = fh_write(pfp, stream.str().c_str(), stream.str().size());
    erh_assert(rc >= 0, FILE_WRITE_ERROR, error_message);

    fh_sync(pfp);
    fh_close(pfp);
  }

  // Call /etc/init.d/dnsmasq restart
  void restart_dnsmasqd(const ::std::string &restart_cmd) {
    int rc = system(restart_cmd.c_str());
    int ecode = WEXITSTATUS(rc);
    erh_assert(0 == ecode, SYSTEM_CALL_ERROR, boost::format("Restarting dnsmasq failed with exit code %d.") % ecode);
  }

  // Rename temporary file to original file name. Rename minimizes the time the original file is
  // not available.
  void rename_file(const ::std::string &filepath, const ::std::string &tmpfilepath) {
    int rc = rename(tmpfilepath.c_str(), filepath.c_str());
    erh_assert(0 == rc, SYSTEM_CALL_ERROR, boost::format("Rename %s failed.") % tmpfilepath);
  }

  //------------------------------------------------------------------------------
  // SECTION Main function, command line option handling.
  //------------------------------------------------------------------------------

  static char const *usage_text =
      "* Get or change the configuration of DHCP or DNS server *\n"
      "\n"
      "Usage:\n"
      "  config_dnsmasq -h                      # Print this usage message.\n"
      "  config_dnsmasq [options] -c            # Generate config files.\n"
      "  config_dnsmasq [options] -g [params]   # Get one or more or all configuration\n"
      "                                         #  parameters.\n"
      "  config_dnsmasq [options] -s param ...  # Set one or more config parameters.\n"
      "\n"
      "Functional command line switches:\n"
      "  -h/--help                  Print this usage message.\n"
      "  -c/--gen-config            Generate dnsmasq config file and /etc/hosts part.\n"
      "  -g/--get-config [param]    Get named configuration parameter.\n"
      "  -s/--set-config param ...  Set one or more of the following config parameters.\n"
      "\n"
      "These are the configuration parameter names and possible values.\n"
      "For the DHCP server\n"
      "  dhcpd-state=<state>        state = disabled | enabled.\n"
      "  dhcpd-range=<range>        range = ip-addr_ip-addr.\n"
      "  dhcpd-lease-time=<minutes> Minimal 2 minutes, default is 120 minutes.\n"
      "  dhcpd-fix-host=<hosts>     hosts = comma sep. list of mac-addr_ip-addr or\n"
      "                             hostname_ip-addr.\n"
      "\n"
      "For the DNS server\n"
      "  dns-state=<state>          state for DNS = disabled | enabled. Default is disabled.\n"
      "  dns-mode=<mode>            mode = relay | proxy. Default is proxy.\n"
      "  dns-fix-host=<dns-hosts>   dns-hosts = comma sep. list ip-addr:hostname[_hostname]\n"
      "\n"
      "Options:\n"
      "  -d/--dhcp                  Configure DHCP server.\n"
      "  -n/--dns                   Configure name service (DNS)\n"
      "  -p/--port=<port-value>     Port name for DHCP server configuration, e.g. br0, br1.\n"
      "  -j/--json                  For get-config: put all output into one JSON format string.\n"
      "                             Parameter names are converted to camel case representation.\n"
      "                             Additionally prints current leases.\n"
      "  -x/--no-dnsmasq-restart    After setting a value do not run /etc/init.d/dnsmasq restart.\n"
      "  -r/dbg-root <path>         Prepend path to config file paths for debugging.\n"
      "  -C/--services-config <file>Alternative network-services.xml configuration file.\n"
      "                             Default xml network configuration file is\n"
      "                             " NETWORK_SERVICES_XML "\n"
      "  -o/--dnsmasq-config <file> Alternative dnsmasq configuration file.\n"
      "                             Default dnsmasq configuration file is\n"
      "                             " DNSMASQ_CONF "\n"
      "  -B/--bridge-config <json>    Bridge configuration as json string\n"
      "  -I/--ip-config <json>        IP configuration as json string\n"
      "  -P/--interface-config <json> Interface configuration as json string\n"
      "  -S/--store-config <path>   Store configuration to file path\n"
      "  -R/--restore-config <path> Restore configuration from file path\n"
      "\n"
      "Examples\n"
      "  config_dnsmasq -d -p br1 -s dhcpd-state=enabled dhcpd-range=192.168.2.100_192.168.2.200\n"
      "    Enables DHCP server on port br1 and sets an address range.\n"
      "\n"
      "  config_dnsmasq --dns --get-config\n"
      "    Prints all DNS server configuration parameters as name=value pairs, one per line.\n"
      "    Same parameter names as required for -s.\n"
      "\n"
      "";

  static struct option long_options[] =
  {
      { "services-config", required_argument, NULL, 'C' },
      { "interfaces-config", required_argument, NULL, 'i' },
      { "dnsmasq-config", required_argument, NULL, 'o' },
      { "help", no_argument, NULL, 'h' },
      { "port", required_argument, NULL, 'p' },
      { "dns", no_argument, NULL, 'n' },
      { "dhcp", no_argument, NULL, 'd' },
      { "get-config", no_argument, NULL, 'g' },
      { "set-config", no_argument, NULL, 's' },
      { "gen-config", no_argument, NULL, 'c' },
      { "dbg-root", required_argument, NULL, 'r' },
      { "json", no_argument, NULL, 'j' },
      { "no-dnsmasq-restart", no_argument, NULL, 'x' },
      { "bridge-config", required_argument, NULL, 'B' },
      { "ip-config", required_argument, NULL, 'I' },
      { "interface-config", required_argument, NULL, 'P' },
      { "restore-config", required_argument, NULL, 'R'},
      { "store-config", required_argument, NULL, 'S'},
      { 0, 0, 0, 0 }
  };

  bool is_legal_port_name(const std::string &name, const std::vector<std::string> &ports) {
    return std::find(ports.begin(), ports.end(), name) != ports.end();
  }

  // Return JSON string with list of DHCPD lease times.
  // Format of one line of the leases file:
  //   1397224102 00:30:de:40:0d:b6 192.168.2.199 PFC200-400DB6 01:00:30:de:40:0d:b6
  ::std::string json_get_lease_file(uint32_t network, uint32_t netmask, const std::string lease_file) {
    ::std::string file_content;
    auto status = cfh_readfile(file_content, lease_file);

    if (status != SUCCESS || file_content.empty()) {
      return "";
    }

    std::stringstream result;
    result << "\"dhcpdLeases\": [";

    std::vector<std::string> lines;
    boost::split(lines, file_content, boost::is_any_of("\n"));

    auto delim = "";
    for (auto &line : lines) {
      if (line.empty()) {
        break;
      }

      std::vector<std::string> fields;
      boost::split(fields, line, boost::is_any_of(" "));
      erh_assert(fields.size() > 4, SYSTEM_CALL_ERROR, "Invalid dhcpd lease file.");

      uint32_t lease_net = 0;
      auto status_ip_conv = utilities::conv_ip_ascii2bin(lease_net, fields[2]);
      erh_assert(SUCCESS == status_ip_conv, SYSTEM_CALL_ERROR, "Invalid ip address in dhcpd lease file.");

      if ((lease_net & netmask) != network) {
        continue;
      }

      char timestr[20];
      auto lt = (time_t) strtoul(fields[0].c_str(), NULL, 10);
      if (lt >= time(NULL)) {
        timestr[0] = '\0';
        auto timestruct = localtime(&lt);
        strftime(timestr, sizeof(timestr), "%F %T", timestruct);
        result << delim << "{\"ipAddr\": \"" << fields[2] << "\", \"macAddr\": \"" << fields[1] << "\", ";
        result << "\"hostName\": \"" << fields[3] << "\", \"endOfLease\": \"" << timestr << "\"}";
        delim = ", ";
      }
    }
    result << "]";

    return result.str();
  }

  void json_print_dhcpd_conf(dnsmasq_handle_t &handle, const PortData &pd, const std::string &lease_file) {

    ::std::string delim = "";
    printf("\"%s\": {", pd.port_name_.c_str());

    for (auto &parameter : xml_param_name_list) {
      if (parameter.pservice_ == DHCPD && parameter.pext_ == true) {

        auto value = ct_dnsmasq_get_value(handle, DHCPD, pd.port_name_, parameter.pname_, "");
        if (parameter.pname_ == "dhcpd-fix-host") {
          // List of static DHCP MAC or host name to ip address conversions.
          // Example: 11:22:33:44:55:66_192.168.2.22,hostname_192.168.2.35 leads to
          //          "dhcpdFixHost": [{"hostMac": "11:22:33:44:55:66", "ipAddr": "192.168.2.22"},
          //                           {"hostMac": "hostname", "ipAddr": "192.168.2.35"}]

          printf("%s\"%s\": [", delim.c_str(), parameter.pjname_.c_str());
          if(!value.empty()) {
            ::std::vector < ::std::string > result;
            ::boost::split(result, value, ::boost::is_any_of(","));

            std::string subdelim = "";
            for (auto &r : result) {
              ::std::vector<::std::string> mac_ip_group;
              ::boost::split(mac_ip_group, r, ::boost::is_any_of("_"));
              printf("%s{\"hostMac\": \"%s\", \"ipAddr\": \"%s\"}", subdelim.c_str(), mac_ip_group[0].c_str(),
                     mac_ip_group[1].c_str());
              subdelim = ", ";
            }
          }
          printf("]");
        } else {
          printf("%s\"%s\": \"%s\"", delim.c_str(), parameter.pjname_.c_str(), value.c_str());
        }
        delim = ", ";
      }
    }

    ::std::string lease = json_get_lease_file(pd.network_bin_, pd.netmask_bin_, lease_file);
    if (!lease.empty()) {
      printf(", %s", lease.c_str());
    }
    printf("}");
  }

  void json_print_dns_conf(dnsmasq_handle_t &handle) {
    ::std::string delim = "";
    printf("{");
    std::vector<std::string> json_entries;
    std::vector<xml_param_t> dns_parameters;
    std::copy_if(xml_param_name_list.begin(), xml_param_name_list.end(),
                 std::back_inserter(dns_parameters),
                 [](auto &p) { return p.pservice_ == DNS && p.pext_ == true; });

    for (auto& parameter : dns_parameters) {
      auto val = ct_dnsmasq_get_value(handle, DNS, "", parameter.pname_, "");
      if (parameter.pname_ == "dns-fix-host") {
        // List of static DNS ip address / hostnames
        // Example: 192.168.1.22:host1_host2_host3,192.168.2.44:hostname leads to
        //          "dnsFixHost": [{"ipAddr": "192.168.1.22", "names": ["host1", "host2", "host3"]},
        //           {"ipAddr": "192.168.2.44", "names": ["hostname"]}]
        printf("%s\"%s\": [", delim.c_str(), parameter.pjname_.c_str());

        if (!val.empty()) {
          std::string subdelim = "";
          ::std::vector<::std::string> result;
          ::boost::split(result, val, ::boost::is_any_of(","), boost::token_compress_on);

          for (auto &r : result) {
            std::string subsubdelim = "";

            ::std::vector<::std::string> ip_name_group;
            ::boost::split(ip_name_group, r, ::boost::is_any_of(":"));
            printf("%s{\"ipAddr\": \"%s\", \"names\": [", subdelim.c_str(), ip_name_group[0].c_str());

            ::std::vector<::std::string> name_list;
            ::boost::split(name_list, ip_name_group[1], ::boost::is_any_of("_"));

            for (auto &name : name_list) {
              printf("%s\"%s\"", subsubdelim.c_str(), name.c_str());
              subsubdelim = ", ";
            }
            printf("]}");
            subdelim = ", ";
          }
        }
        printf("]");
      } else {
        printf("%s\"%s\": \"%s\"", delim.c_str(), parameter.pjname_.c_str(), val.c_str());
      }
      delim = ", ";
    }
    printf("}\n");
  }

  void cmd_assert(bool cond, const std::string &emsg) {
      erh_assert(cond, INVALID_PARAMETER, emsg);
  }

  ::std::map<::std::string,::std::string> port_mapping = {{"br0","X1"},{"br1","X2"},{"br2","X11"},{"br3","X12"}};

  std::string bridge_to_port(const std::string &bridge)
  {
    auto it = port_mapping.find(bridge);
    if (it != port_mapping.end())
    {
      return port_mapping[bridge];
    }
    else
    {
      return bridge;
    }
  }

  std::string port_to_bridge(const std::string &port) {
    ::std::string bridge = port;

    if (port == "X1")
      bridge = "br0";
    else if (port == "X2")
      bridge = "br1";

    return bridge;
  }
  
  // Evaluate command line options. Get and set (-g, -s) do not require option arguments because
  // these functions will interpret non option arguments as list of settings or list of
  // config parameter names to print.
  // Returns optind.
  int eval_options(int argc, char **argv, prgconf_t &prgconf)
  {
      int c;
      std::string services_xml = NETWORK_SERVICES_XML;
      std::string interfaces_xml = NETWORK_INTERFACES_XML;
      std::string etchosts = ETC_HOSTS;
      std::string dnsmasq_conf = DNSMASQ_CONF;
      std::string dnsmasq_conf_tmp = DNSMASQ_CONF_TMP;
      std::string restart_command = DNSMASQ_RESTART_CMD;
      std::string lease_file = DNSMASQ_LEASE_FILE;
      int option_index = 0;

      while(-1 != (c = getopt_long(argc, argv, "C:i:o:p:hndgsr:cjxB:I:P:S:R:", long_options, &option_index)))
      {
          const std::string err_servtype = "Double service type command line option.";
          const std::string err_mport = "Command line: multiple port specification.";
          const std::string err_moption = "Command line: multiple options detected.";
          switch(c) {
          case 'r':
              cmd_assert(!prgconf.dbg_root.empty(), err_moption);
              prgconf.dbg_root = optarg;
              break;
          case 'C':
              cmd_assert(services_xml == NETWORK_SERVICES_XML, err_moption);
              services_xml = optarg;
              break;
          case 'o':
              cmd_assert(dnsmasq_conf == DNSMASQ_CONF, err_moption);
              dnsmasq_conf = optarg;
              cmd_assert(dnsmasq_conf_tmp == DNSMASQ_CONF_TMP, err_moption);
              dnsmasq_conf_tmp = ::std::string(optarg) + ".tmp";
              break;
          case 'i':
              cmd_assert(interfaces_xml == NETWORK_INTERFACES_XML, err_moption);
              interfaces_xml = optarg;
              break;
          case 'x':
              prgconf.dnsmasq_restart = false;
              break;
          case 'n':
              cmd_assert(prgconf.service_type == UNDEF, err_servtype);
              prgconf.service_type = DNS;
              break;
          case 'd':
              cmd_assert(prgconf.service_type == UNDEF, err_servtype);
              prgconf.service_type = DHCPD;
              break;
          case 'p':
              cmd_assert(prgconf.selected_port.empty(), err_mport);
              prgconf.selected_port = port_to_bridge(optarg);
              break;
          case 'j':
              prgconf.command = GET_JSON_CONFIG;
              break;
          case 'g':
              prgconf.command = GET_CONFIG;
              break;
          case 's':
              prgconf.command = SET_CONFIG;
              break;
          case 'c':
              prgconf.command = GENERATE;
              break;
          case 'B':
              prgconf.bridge_config = optarg;
              break;
          case 'I':
              prgconf.ip_config = optarg;
              break;
          case 'P':
              prgconf.interface_config = optarg;
              break;
          case 'h':
              prgconf.command = HELP;
              break;
          case 'R':
              prgconf.command = RESTORE_CONFIG;
              prgconf.backup_file_path = optarg;
              break;
          case 'S':
              prgconf.command = STORE_CONFIG;
              prgconf.backup_file_path = optarg;
              break;
              /*NOTREACHED*/
          default:
              erh_set_error(INVALID_PARAMETER, "Illegal command line option", "");
              break;
          }
      }

      // Check for valid function command code.
      cmd_assert(prgconf.command != NONE, "Missing one of command line options -c, -g, -s, -j, -h");

      // Prepend config file paths with debug root path. Default debug root path is empty.
      // Copy file paths to program config structure.
      prgconf.services_xml_path = prgconf.dbg_root + services_xml;
      prgconf.interfaces_xml_path = prgconf.dbg_root + interfaces_xml;
      prgconf.etchosts = prgconf.dbg_root + etchosts;
      prgconf.dnsmasq_conf = prgconf.dbg_root + dnsmasq_conf;
      prgconf.dnsmasq_conf_tmp = prgconf.dbg_root + dnsmasq_conf_tmp;
      prgconf.restart_command = prgconf.dbg_root + restart_command;
      prgconf.lease_file = prgconf.dbg_root + lease_file;

      return optind;
  }


void print_config(dnsmasq_handle_t &session, const service_t service, const std::string &port,
                  int argc, char **argv, int loptind) {
  erh_assert(service != UNDEF, MISSING_PARAMETER, "Missing command line option -d or -n");
  if (DHCPD == service) {
    erh_assert(!port.empty(), INVALID_PARAMETER, "Missing port option -p");
  }

  if (argc > loptind) {
    // Further command line arguments available. Get configuration parameters.
    int idx;
    for (idx = loptind; idx < argc; idx++) {
      std::string key = std::string { argv[idx] };
      std::string value;

      if (key == "dhcpd-fix-host" || key == "dns-fix-host") {
        value = ct_dnsmasq_get_value(session, service, port, argv[idx], "");
      } else {
        value = ct_dnsmasq_get_value(session, service, port, argv[idx]);
      }

      printf("%s\n", value.c_str());
    }
  } else {
    // No parameter selection present. Print all config parameters.
    for (auto &elem : xml_param_name_list) {
      if (elem.pservice_ == service && elem.pext_ == true) {
        std::string value_str = ct_dnsmasq_get_value(session, service, port, elem.pname_, "");
        printf("%s=%s\n", elem.pname_.c_str(), value_str.c_str());
      }
    }
  }
}

/**
 * Print configuration in JSON format for WBM.
 *
 * @param session
 * @param service
 * @param ip_config
 * @param lease_file
 */
void print_json_config(
    dnsmasq_handle_t &session, const service_t service,
    IpConfiguration &ip_config, const std::string &lease_file) {

  erh_assert(service != UNDEF, MISSING_PARAMETER, "Missing command line option -d or -n");

  if (DHCPD == service) {
    char const *delim = "";

    printf("{");
    for (const auto &port : ip_config.port_name_list) {
      printf(delim);
      auto pd = ip_config.getPortData(port);
      json_print_dhcpd_conf(session, pd, lease_file);
      delim = ", ";
    }
    printf("}\n");
  } else {
    json_print_dns_conf(session);
  }
}

void set_config(const service_t &service, const std::string &port, int argc, char **argv, int loptind,
                const prgconf_t &prgconf, dnsmasq_handle_t &session, IpConfiguration &ip_config,
                const std::vector<::std::string> &ports) {
  // First check and adapt configuration parameters to possibly changed network interface settings.
  erh_assert(service != UNDEF, MISSING_PARAMETER,
             "The service to configure is unknown, set option -d (dhcp) or -n (dns)");
  ct_dnsmasq_check_ip_dependencies(ip_config, session, ports);

  for (int idx = loptind; idx < argc; idx++) {
    auto param = ct_dnsmasq_parse_option(argv[idx], service, port, ip_config);
    ct_dnsmasq_set_value(session, service, port, param.name_, param.value_);
  }
  ct_dnsmasq_save_xml_file(session);

  if (prgconf.dnsmasq_restart) {
    // The config_dnsmasq_c tool might be called during restart again.
    // Therefore, we have to close the session. Otherwise the restart may result in a deadlock.
    ct_dnsmasq_finish_session(session);
    restart_dnsmasqd(prgconf.restart_command);
  }
}

/**
 * Create the dnsmasq configuration file and edit /etc/hosts.
 * First save xml file. Could be changed due to changes of network interfaces.
 * This function is typically called from the dnsmasq init script on dnsmasq startup.
 */
void generate_config_file(const prgconf_t &prgconf, dnsmasq_handle_t &session, IpConfiguration &ip_config,
                          const std::vector<::std::string> &ports) {
  ct_dnsmasq_check_ip_dependencies(ip_config, session, ports);
  ct_dnsmasq_save_xml_file (session);
  ct_dnsmasq_edit_etc_hosts(session, ip_config, prgconf.etchosts);
  ct_dnsmasq_generate_conf(session, prgconf.dnsmasq_conf_tmp, ports, ip_config);
  rename_file(prgconf.dnsmasq_conf, prgconf.dnsmasq_conf_tmp);
}


void store_config(const prgconf_t &prgconf, dnsmasq_handle_t &session, const ::std::vector<::std::string> &bridges ){

  ::std::stringstream backup;
  ::std::stringstream legacy_backup;

  for (auto bridge : bridges) {
    for (auto &parameter : xml_param_name_list) {
      if (parameter.pservice_ == DHCPD && parameter.pext_ == true) {
        auto value = ct_dnsmasq_get_value(session, DHCPD, bridge, parameter.pname_, "");

        backup << bridge << "-" << parameter.pname_ << "=" << value << ::std::endl;

        if(bridge == "br0" || bridge == "br1"){
          legacy_backup << bridge_to_port(bridge) << "-" << parameter.pname_ << "=" << value << ::std::endl;
        }
      }
    }
  }

  FileEditor editor;
  eStatusCode status = editor.Append(prgconf.backup_file_path, legacy_backup.str());
  erh_assert(SUCCESS == status, status, "Failed to write legacy backup to file: " + prgconf.backup_file_path, false);
  status = editor.Append(prgconf.backup_file_path, backup.str());
  erh_assert(SUCCESS == status, status, "Failed to write backup to file: " + prgconf.backup_file_path, false);
}

eStatusCode get_key_value(const ::std::string &backup_content,
                          const ::std::string &key,
                          ::std::string &value)
{
  bool found_key = false;

  ::std::stringstream data_stream = ::std::stringstream(backup_content);
  ::std::string subdata;
  while(::std::getline(data_stream, subdata, '\n') && !found_key)
  {
    if(boost::starts_with(subdata, key))
    {
      subdata.erase(0, key.length());
      value += subdata;
      found_key = true;
    }
  }

  return (found_key) ? SUCCESS : MISSING_PARAMETER;
}

void restore_config(const prgconf_t &prgconf, dnsmasq_handle_t &session, IpConfiguration &ip_config,
                    const ::std::vector<::std::string> &bridges){

  FileEditor editor;
  ::std::string backup_file_content;
  eStatusCode status = SUCCESS;

  status = editor.Read(prgconf.backup_file_path, backup_file_content);
  erh_assert(SUCCESS == status, status, "Failed to read backup from file: " + prgconf.backup_file_path);

  bool read_backup_parameters_successfully = true;
  for (auto bridge : bridges) {
    for (auto &parameter : xml_param_name_list) {
      if (parameter.pservice_ == DHCPD && parameter.pext_ == true) {
        ::std::string key = bridge + "-" + parameter.pname_ + "=";
        ::std::string value = "";
        status = get_key_value(backup_file_content, key, value);

        if(SUCCESS != status) {
          read_backup_parameters_successfully = false;
          erh_assert(SUCCESS == status, status, "Missing parameter in backup file: " + key, false);
          break;
        }

        ct_dnsmasq_set_value(session, DHCPD, bridge, parameter.pname_, value);
      }
    }
  }

  // Try to do legacy backup if previous backup attempt failed.
  if(!read_backup_parameters_successfully) {
    erh_assert(false, status, "Try to do legacy backup.", false);
    ::std::vector<::std::string> legacy_ports = {"X1", "X2"};
    for (auto port : legacy_ports) {
      for (auto &parameter : xml_param_name_list) {
        if (parameter.pservice_ == DHCPD && parameter.pext_ == true) {
          ::std::string key = port + "-" + parameter.pname_ + "=";
          ::std::string value = "";
          status = get_key_value(backup_file_content, key, value);
          erh_assert(SUCCESS == status, status, "Missing legacy parameter in backup file: " + key);
          ct_dnsmasq_set_value(session, DHCPD, port_to_bridge(port), parameter.pname_, value);
        }
      }
    }
  }

  generate_config_file(prgconf, session, ip_config, bridges);
}

}  // Anonymous namespace

// main: call option evaluation and execute selected function.
int execute(int argc, char **argv) {
  IpConfiguration ip_config_data;
  int debugmode = false;
  prgconf_t prgconf;
  ::std::vector<::std::string> legal_bridge_names;

  // Check for running as root and evaluate command line.
  erh_init(basename(argv[0]));
  int loptind = eval_options(argc, argv, prgconf);

  if (HELP == prgconf.command) {
    printf(usage_text);
    return 0;
  }

  ::std::string port = prgconf.selected_port;
  service_t serv = prgconf.service_type;
  if (prgconf.dbg_root.empty()) {
    erh_assert(0 == getuid(), SYSTEM_CALL_ERROR, "Not running as root. Abort.");
  } else {
    debugmode = true;
  }
    
  // Assert unmodified config file and valid port name, if given.
  // Open xml file session to network-services.xml, read all required configuration data.
  ct_dnsmasq_assert_conf_not_compromised(prgconf.dnsmasq_conf);
  auto dnsmasq_session = ct_dnsmasq_start_session(prgconf.services_xml_path);

  netcfg_read_settings(ip_config_data, legal_bridge_names, prgconf, debugmode);

  if (!port.empty()) {
    erh_assert(is_legal_port_name(port, legal_bridge_names), INVALID_PARAMETER,
               boost::format("Option -p, invalid port name '%s'") % port);
  }

  // Start selected program function.
  if (GET_CONFIG == prgconf.command) {
    print_config(dnsmasq_session, serv, port, argc, argv, loptind);
  } else if (GET_JSON_CONFIG == prgconf.command) {
    print_json_config(dnsmasq_session, serv, ip_config_data, prgconf.lease_file);
  } else if (SET_CONFIG == prgconf.command) {
    set_config(serv, port, argc, argv, loptind, prgconf, dnsmasq_session, ip_config_data, legal_bridge_names);
  } else if (GENERATE == prgconf.command) {
    generate_config_file(prgconf, dnsmasq_session, ip_config_data, legal_bridge_names);
  } else if (STORE_CONFIG == prgconf.command) {
    store_config(prgconf, dnsmasq_session, legal_bridge_names);
  } else if (RESTORE_CONFIG == prgconf.command) {
    restore_config(prgconf, dnsmasq_session, ip_config_data, legal_bridge_names);
  }

  ct_dnsmasq_finish_session(dnsmasq_session);

  return 0;
}
