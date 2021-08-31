// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// Copyright (c) 2020 WAGO Kontakttechnik GmbH & Co. KG

#include "ct_libnet.h"

#include <arpa/inet.h>
#include <assert.h>
#include <ct_error_handling.h>
#include <ctype.h>
#include <net/if.h>
#include <netinet/in.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include <BaseTypes.hpp>
#include <BridgeConfig.hpp>
#include <IPConfig.hpp>
#include <InterfaceConfig.hpp>
#include <InterfaceInformationApi.hpp>
#include <boost/algorithm/string/join.hpp>

#include "ct_ethtool.h"
#include "ct_netlink.h"
#include "ct_xml.h"
#include "ct_xml_xpath.h"
#include "ct_xslt.h"
#ifdef __ENABLE_DSA
#include "ct_swconfig.h"
#endif

using namespace netconf;
namespace napi = netconf::api;

extern "C" {

#define INET_ADDR_WITH_CIDRSTRLEN (INET_ADDRSTRLEN + sizeof("/32"))
#define ETH_ADDRSTRLEN (sizeof("11:22:33:44:55:66"))

#define NET_IFACES_TMP "/tmp/net-ifaces"

#include <unistd.h>
#include <sys/stat.h>

#include <typelabel_API.h>

struct libnetSession {
    xmlSession_t *xml;
};

enum ethtoolParamType
{
    ETHTOOL_PARAM_AUTONEG,
    ETHTOOL_PARAM_SPEED,
    ETHTOOL_PARAM_DUPLEX,
};

// The mapping port -> net interface depends from context:
// When using virtual interfaces, ip settings may be configured for example on virtual interface br0
// whereas ethernet settings like port speed or duplex are configured on physical interface ethX1
// That's why we need 2 functions to resolve the mapping

namespace {

constexpr size_t netmask_count = 33;

const char *netmasks[netmask_count] = {
    "0.0.0.0",          // 0

    "128.0.0.0",        // 1
    "192.0.0.0",        // 2
    "224.0.0.0",        // 3
    "240.0.0.0",        // 4
    "248.0.0.0",        // 5
    "252.0.0.0",        // 6
    "254.0.0.0",        // 7
    "255.0.0.0",        // 8

    "255.128.0.0",      // 9
    "255.192.0.0",      // 10
    "255.224.0.0",      // 11
    "255.240.0.0",      // 12
    "255.248.0.0",      // 13
    "255.252.0.0",      // 14
    "255.254.0.0",      // 15
    "255.255.0.0",      // 16

    "255.255.128.0",      // 17
    "255.255.192.0",      // 18
    "255.255.224.0",      // 19
    "255.255.240.0",      // 20
    "255.255.248.0",      // 21
    "255.255.252.0",      // 22
    "255.255.254.0",      // 23
    "255.255.255.0",      // 24


    "255.255.255.128",      // 25
    "255.255.255.192",      // 26
    "255.255.255.224",      // 27
    "255.255.255.240",      // 28
    "255.255.255.248",      // 29
    "255.255.255.252",      // 30
    "255.255.255.254",      // 31
    "255.255.255.255",      // 32
};

const char* g_lastError;

int port2dev_ethernet(const char *port, char *dev, size_t devLen);
int port2dev_ip(const char *port, char *dev, size_t devLen);

void set_wbm_error_state(int *status, int error_code, const char *errmsg);

bool is_valid_ip(const char *ip);
bool is_valid_mac(const char *ip);

bool is_valid_ip(const char *ip)
{
    struct sockaddr_in sa;
    int result = inet_pton(AF_INET, ip, &(sa.sin_addr));
    return (result != 0);
}

bool is_valid_mac(const char *mac)
{
    bool result = true;

    int cur = 0;
    while(*mac)
    {
        if((cur + 1) % 3 == 0)
        {
            if(*mac != ':')
            {
                result = false;
                break;
            }
        }
        else
        {
            if(!isxdigit(*mac))
            {
                result = false;
                break;
            }
        }

        ++mac;
        ++cur;
    }

    return (result && cur == (ETH_ADDRSTRLEN -1));
}

// NOTE: only interesting for setter functions.
// Error message is shown in WBM/CBM -> error in a getter results in an empty field.
// Error messages should have an according granularity (end user relevant)!
void set_wbm_error_state(int *status, int error_code, const char *errmsg)
{
    if(NULL != status)
    {
        *status = error_code;
    }

    g_lastError = errmsg;
}

int port2dev_ethernet(const char *port,
                        char *dev,
                        size_t devLen)
{
  try{
    auto is_label_equal_to_port = [&](const auto& interface_information) {
      return interface_information.GetInterfaceLabel() == port;
    };

    auto vector_of_interface_information
        = napi::GetInterfaceInformation(netconf::DeviceType::Port);
    auto it = ::std::find_if(
        vector_of_interface_information.begin(),
        vector_of_interface_information.end(),
        is_label_equal_to_port);
    if(it == vector_of_interface_information.end()){
      return INVALID_PARAMETER;
    }

    auto port_device = it->GetInterfaceName();

    if (devLen <= port_device.size())
        return INVALID_PARAMETER;

    strcpy(dev, port_device.c_str());
    return SUCCESS;
  }
  catch(...)
  {
    return ERROR;
  }
}

int port2dev_ip(const char *port,
                char *dev,
                size_t devLen)
{
  ::std::string bridge{};
  try{
    napi::BridgeConfig bridge_config;
    auto error = ::netconf::api::GetBridgeConfig(bridge_config);
    if (!error) {
        return static_cast<int>( error.GetStatusCode());
    }
    bridge = bridge_config.GetBridgeOfInterface(port);
  }
  catch (...){
    /* nothing to do -> bridge.empty()-handling follows */
  }

  if (bridge.empty())
  {
    /* No bridge for this interface found, take port as is */
    strncpy(dev, port, devLen);
    return devLen > strlen(port) ? SUCCESS : INVALID_PARAMETER;
  }

  if (bridge.size() >= devLen)
  {
      return INVALID_PARAMETER;
  }

  strcpy(dev, bridge.c_str());
  return SUCCESS;
}

int exec_post_netconfig_scripts(const char *dev)
{
    int status = SUCCESS;
    int rc;
    GString *cmd = g_string_new("/bin/run-parts");

    assert(NULL != dev);
    if('\0' != *dev)
    {
        g_string_append_printf(cmd, " -a %s", dev);
    }
    g_string_append(cmd, " /etc/config-tools/post_netconfig.d");
    rc = system((const char *)cmd->str);
    status = WEXITSTATUS(rc);
    g_string_free(cmd, TRUE);
    return status;
}

// Assumption: string is already stripped
bool is_classic_netmask(const char *netmask)
{
    bool res = false;

    int i;
    for(i = 0; (i<netmask_count) && (false == res); ++i)
    {
        if(0 == strcmp(netmasks[i], netmask))
        {
            res = true;
        }
    }

    return res;
}

// Assumption: string is already stripped
bool is_cidr_netmask(const char *netmask)
{
    bool res = false;

    if(0 == strcmp("0", netmask)) // strtol returns 0 in error case
    {
        res = true;
    }
    else
    {
        int nm_as_int = strtol(netmask, NULL, 10);

        if( (nm_as_int > 0) && (nm_as_int < netmask_count) )
        {
            res = true;
        }
    }

    return res;
}

::boost::optional<InterfaceConfig> get_interface_config(const ::std::string& interface_name)
{
  try{
    napi::InterfaceConfigs interface_configs;
    auto error = napi::GetInterfaceConfigs(interface_configs);
    if (!error) {
      return ::boost::optional<InterfaceConfig>{};
    }
    return interface_configs.GetInterfaceConfig(interface_name);
  }
  catch(...)
  {
    return ::boost::optional<InterfaceConfig>{};
  }
}

::std::string get_bridge_name(const ::std::string interface_name){
  try{
    napi::BridgeConfig bridge_config;
    napi::GetBridgeConfig(bridge_config);
    return bridge_config.GetBridgeOfInterface(interface_name);
  }
  catch (...) {
    return ::std::string{};
  }
}

::boost::optional<IPConfig> get_ip_config(const ::std::string& interface_name)
{
  auto bridge_name = get_bridge_name(interface_name);
  napi::IPConfigs ip_configs;
  auto error = napi::GetIPConfigs(ip_configs);
  if(not bridge_name.empty() && error.IsOk()){
    return ip_configs.GetIPConfig(bridge_name);
  }
  return boost::optional<IPConfig>();
}
} // namspace -> static linkage

const char * ct_libnet_get_last_error(void)
{
    return g_lastError;
}

int ct_libnet_netmask_to_cidr(const char *from, char *res, size_t resLen)
{
    assert(NULL != from);
    assert(NULL != res);
// allow overlapping from and to (as in netmask_to_cidr(netmask,netmask...)
//    assert(0 == strlen(res));

    assert(resLen >= sizeof("32"));

    int status = INVALID_PARAMETER;

    if(is_cidr_netmask(from))
    {
        (void) g_strlcpy(res, from, resLen);
        status = SUCCESS;
    }
    else
    {
        int i;
        for(i = 0; (i<netmask_count) && (SUCCESS != status); ++i)
        {
            if(0 == strcmp(netmasks[i], from))
            {
                (void) snprintf(res, resLen, "%d", i);
                status = SUCCESS;
            }
        }
    }

    return status;
}

int ct_libnet_start_session(const char *xmlConfigFile, libnetSession_t **sessionHandle)
{
    int status = SUCCESS;

    assert(NULL != sessionHandle);

    *sessionHandle = reinterpret_cast<libnetSession_t*>( g_malloc(sizeof(libnetSession_t)));

    libnetSession_t *ressources = *sessionHandle;

    if(NULL == ressources)
    {
        status = SYSTEM_CALL_ERROR;
    }
    else
    {
        (*sessionHandle)->xml = NULL;
        status = ct_xml_start_session(xmlConfigFile, &((*sessionHandle)->xml));
    }

    return status;
}

int ct_libnet_finish_session(libnetSession_t *sessionHandle)
{
    int status = SUCCESS;

    if(NULL != sessionHandle)
    {
        if(NULL != sessionHandle->xml)
        {
            status = ct_xml_finish_session(sessionHandle->xml);
        }
        g_free(sessionHandle);
    }

    return status;
}

void ct_libnet_permanent_close(void)
{
    ct_xml_permanent_close();
}

int ct_libnet_netmask_to_classic(const char *from, char *res, size_t resLen)
{
    assert(NULL != from);
    assert(NULL != res);

// allow overlapping from and to (as in netmask_to_classic(netmask,netmask...)
//    assert(0 == strlen(res));

    assert(resLen >= INET_ADDRSTRLEN);

    int status = INVALID_PARAMETER;

    if(is_classic_netmask(from))
    {
        (void) g_strlcpy(res, from, resLen);
        status = SUCCESS;
    }
    else
    {
        // strtol returns 0 in error case
        if(0 == strcmp("0", from))
        {
            (void) snprintf(res, resLen, "0.0.0.0");
            status = SUCCESS;
        }
        else
        {
            int nm_as_int = strtol(from, NULL, 10);

            if( (nm_as_int > 0) && (nm_as_int < netmask_count) )
            {
                (void) snprintf(res, resLen, "%s", netmasks[nm_as_int]);
                status = SUCCESS;
            }
        }
    }

    return status;
}

int ct_libnet_get_eth_port_state_from_hw(const char *port,
                                         char *strState,
                                         size_t strStateLen,
                                         libnetSession_t *sessionHandle)
{
    assert(NULL != port);
    assert(NULL != strState);
    assert(strStateLen >= sizeof("disabled"));
    assert(NULL != sessionHandle);

    int status = SUCCESS;
    unsigned int flags = 0;

    char dev[IFNAMSIZ + 1];
    dev[0] = '\0';

    status = port2dev_ethernet(port, dev, sizeof(dev));

    // FIXME: find a better place for init/cleanup!
    // User should be able to chain several ct_libnet_ functions without
    // reinitializing the underlying stuff.

	netlinkSession_t *nlSessionHandle = NULL;

    if(SUCCESS == status)
    {
        status = ct_netlink_init(&nlSessionHandle);
    }

    if(SUCCESS == status)
    {
        status = ct_netlink_get_link_flags(dev, &flags, nlSessionHandle);
    }

    if(SUCCESS == status)
    {
        if(flags & IFF_UP)
        {
            snprintf(strState, strStateLen, "enabled");
        }
        else
        {
            snprintf(strState, strStateLen, "disabled");
        }
    }

    ct_netlink_cleanup(nlSessionHandle);

    return status;
}

int ct_libnet_get_cable_state(const char *port,
                              enum cableState_t *state,
                              libnetSession_t *sessionHandle)
{
    assert(NULL != port);
    assert(NULL != state);
    assert(NULL != sessionHandle);

    int status = SUCCESS;
    unsigned int flags = 0;

    char dev[IFNAMSIZ + 1];
    dev[0] = '\0';

    status = port2dev_ethernet(port, dev, sizeof(dev));

    // FIXME: find a better place for init/cleanup!
    // User should be able to chain several ct_libnet_ functions without
    // reinitializing the underlying stuff.

	netlinkSession_t *nlSessionHandle = NULL;

    if(SUCCESS == status)
    {
        status = ct_netlink_init(&nlSessionHandle);
    }

    if(SUCCESS == status)
    {
        status = ct_netlink_get_link_flags(dev, &flags, nlSessionHandle);
    }

    if(SUCCESS == status)
    {

        if(flags & IFF_RUNNING)
        {
            *state = CT_CABLE_STATE_CONNECTED;
        }
        else
        {
            if(flags & IFF_UP)
            {
                *state = CT_CABLE_STATE_DISCONNECTED;
            }
            else
            {
                *state = CT_CABLE_STATE_UNKNOWN;
            }
        }
    }

    ct_netlink_cleanup(nlSessionHandle);

    return status;
}

/**
 * @brief Returns the IPv4 address currently configured for the given device
 *
 * @param dev   Device name (i.e. eth0)
 * @param ip    Pointer to char array where the ip address will be stored. Should be at least 20 bytes long
 *             to store string like "255.255.255.255/32"
 * @param ipLen Length of the ip char array
 * @param mode  IP_FORMAT_MODE_NO_NETMASK/IP_FORMAT_MODE_NETMASK expected. The former modifier may be used
 *             to cut the CIDR netmask part (i.e. /24) from the IP address.
 *
 * @return SUCCESS if no errors happened, ct_netlink_get_ipaddr's errors
 */
int ct_libnet_get_ip_addr_from_hw(const char *port,
                                  char *ip,
                                  size_t ipLen,
                                  enum ipAddrFormatModes_t mode,
                                  libnetSession_t *sessionHandle)
{
    assert(NULL != port);
    assert(NULL != ip);

    assert(ipLen >= sizeof("255.255.255.255/32"));
    assert(NULL != sessionHandle);


    int status = SUCCESS;


    char dev[IFNAMSIZ + 1];
    dev[0] = '\0';

    status = port2dev_ip(port, dev, sizeof(dev));

	netlinkSession_t *nlSessionHandle = NULL;

    if(SUCCESS == status)
    {
        status = ct_netlink_init(&nlSessionHandle);
    }

    if(SUCCESS == status)
    {
        status = ct_netlink_get_ipaddr(dev, ip, ipLen, nlSessionHandle);
    }

    if((SUCCESS == status) && (IP_FORMAT_MODE_NO_NETMASK == mode))
    {
        char *delim = strchr(ip, '/');
        if(NULL != delim)
        {
            *delim = '\0';
        }
    }

    ct_netlink_cleanup(nlSessionHandle);

    return status;
}

int ct_libnet_get_netmask_from_hw(const char *port,
                                  char *netmask,
                                  size_t netmaskLen,
                                  enum netmaskFormatModes_t mode,
                                  libnetSession_t *sessionHandle)
{

    assert(NULL != port);
    assert(NULL != netmask);
    assert(NULL != sessionHandle);

    switch(mode)
    {
        case NETMASK_FORMAT_MODE_CIDR:
            assert(netmaskLen > 2);
            break;
        case NETMASK_FORMAT_MODE_CLASSIC:
            assert(netmaskLen >= INET_ADDRSTRLEN);
            break;
        default:
            break;
    }

    int status = SUCCESS;

    char ip[INET_ADDR_WITH_CIDRSTRLEN];
    ip[0] = '\0';

    char dev[IFNAMSIZ + 1];
    dev[0] = '\0';

    status = port2dev_ip(port, dev, sizeof(dev));

    netlinkSession_t *nlSessionHandle = NULL;

    if(SUCCESS == status)
    {
        status = ct_netlink_init(&nlSessionHandle);
    }

    if(SUCCESS == status)
    {
        status = ct_netlink_get_ipaddr(dev, ip, sizeof(ip), nlSessionHandle);
    }

    if(SUCCESS == status)
    {
        char *delim = strchr(ip, '/');
        if(NULL != delim)
        {
            strncpy(netmask, (delim + 1), netmaskLen - 1);
            netmask[netmaskLen - 1] = '\0';

            if(NETMASK_FORMAT_MODE_CLASSIC == mode)
            {
                status = ct_libnet_netmask_to_classic(delim + 1, netmask, netmaskLen);
            }
            else // CIDR mode is the netlink layer default
            {
                g_strlcpy(netmask, delim + 1, netmaskLen);
            }
        }
    }

    ct_netlink_cleanup(nlSessionHandle);

    return status;
}

int ct_libnet_get_mac_addr_from_hw(const char *port,
                                   char *mac,
                                   size_t macLen,
                                   libnetSession_t *sessionHandle)
{
    assert(NULL != port);
    assert(NULL != mac);
    assert(NULL != sessionHandle);

    int status = SUCCESS;

    char dev[IFNAMSIZ + 1];
    dev[0] = '\0';

    status = port2dev_ip(port, dev, sizeof(dev));

    netlinkSession_t *nlSessionHandle = NULL;

    if(SUCCESS == status)
    {
        status = ct_netlink_init(&nlSessionHandle);
    }

    if(SUCCESS == status)
    {
        status = ct_netlink_get_macaddr(dev, mac, macLen, nlSessionHandle);
    }

    ct_netlink_cleanup(nlSessionHandle);

    return status;
}

int ct_libnet_get_broadcast_from_hw(const char *port,
                                    char *broadcast,
                                    size_t broadcastLen,
                                    libnetSession_t *sessionHandle)
{

    assert(NULL != port);
    assert(NULL != broadcast);
    assert(NULL != sessionHandle);

    int status = SUCCESS;

    char dev[IFNAMSIZ + 1];
    dev[0] = '\0';

    status = port2dev_ip(port, dev, sizeof(dev));

    netlinkSession_t *nlSessionHandle = NULL;

	  status = ct_netlink_init(&nlSessionHandle);

    if(SUCCESS == status)
    {
        status = ct_netlink_get_broadcast(dev, broadcast, broadcastLen, nlSessionHandle);
    }

    ct_netlink_cleanup(nlSessionHandle);

	return status;
}

int ct_libnet_get_config_type(const char *port,
                              char *configType,
                              size_t configTypeLen,
                              libnetSession_t *)
{
    assert(NULL != port);
    assert(NULL != configType);

    auto ip_cfg = get_ip_config(port);

    if(! ip_cfg)
    {
        return INVALID_PARAMETER;
    }

    int lengthNeeded = 0;
    switch(ip_cfg->source_){
        case IPSource::DHCP:
            lengthNeeded = snprintf(configType, configTypeLen, "dhcp");
        break;
        case IPSource::BOOTP:
            lengthNeeded = snprintf(configType, configTypeLen, "bootp");
        break;
        default:
            /*Old API doenst support other types, so map everything to static */
            lengthNeeded = snprintf(configType, configTypeLen, "static");
        break;
    }

    return (lengthNeeded <= configTypeLen)? SUCCESS : NOT_ENOUGH_SPACE_ERROR;
}

static int ct_libnet_get_ethtool_param(enum ethtoolParamType param,
                                       const char *port,
                                       char *result,
                                       size_t resultLen,
                                       libnetSession_t *)
{
    assert(NULL != port);
    assert(NULL != result);

    int status = SUCCESS;

    char dev[IFNAMSIZ + 1];
    dev[0] = '\0';

    status = port2dev_ethernet(port, dev, sizeof(dev));

    if(SUCCESS == status)
    {
        switch(param)
        {
        case ETHTOOL_PARAM_SPEED:
            status = ct_ethtool_get_speed(dev, result, resultLen);
            break;
        case ETHTOOL_PARAM_DUPLEX:
            status = ct_ethtool_get_duplex(dev, result, resultLen);
            break;
        case ETHTOOL_PARAM_AUTONEG:
            status = ct_ethtool_get_autoneg(dev, result, resultLen);
            break;
        default:
            status = INVALID_PARAMETER;
        }
    }

    return status;
}

int ct_libnet_get_speed_from_hw(const char *port,
                                char *speed,
                                size_t speedLen,
                                libnetSession_t *sessionHandle)
{
    return ct_libnet_get_ethtool_param(ETHTOOL_PARAM_SPEED,
                                       port,
                                       speed,
                                       speedLen,
                                       sessionHandle);
}

int ct_libnet_get_duplex_from_hw(const char *port,
                                 char *duplex,
                                 size_t duplexLen,
                                 libnetSession_t *sessionHandle)
{
    return ct_libnet_get_ethtool_param(ETHTOOL_PARAM_DUPLEX,
                                       port,
                                       duplex,
                                       duplexLen,
                                       sessionHandle);
}

int ct_libnet_get_autoneg_from_hw(const char *port,
                                  char *autoneg,
                                  size_t autonegLen,
                                  libnetSession_t *sessionHandle)
{
    return ct_libnet_get_ethtool_param(ETHTOOL_PARAM_AUTONEG,
                                       port,
                                       autoneg,
                                       autonegLen,
                                       sessionHandle);
}

int ct_libnet_get_autoneg_from_config(const char *port,
                                      char *autoneg,
                                      size_t autonegLen,
                                      libnetSession_t *)
{
    assert(NULL != port);
    assert(NULL != autoneg);
    assert(autonegLen >= sizeof("disabled"));

    auto config = get_interface_config(port);

    if (config)
    {
        switch (config->autoneg_)
        {
        case netconf::Autonegotiation::ON:
            strcpy(autoneg, "enabled");
            return SUCCESS;
        case netconf::Autonegotiation::OFF:
            strcpy(autoneg, "disabled");
            return SUCCESS;
        default:
            break;
        }
    }
    return INVALID_PARAMETER;
}

int ct_libnet_get_port_state_from_config(const char *port,
                                         char *state,
                                         size_t stateLen,
                                         libnetSession_t *)
{
    assert(NULL != port);
    assert(NULL != state);
    assert(stateLen >= sizeof("disabled"));

    auto config = get_interface_config(port);

    if (config)
    {
        switch (config->state_)
        {
        case InterfaceState::UP:
            strcpy(state, "enabled");
            return SUCCESS;
        case InterfaceState::DOWN:
            strcpy(state, "disabled");
            return SUCCESS;
        default:
            break;
        }
    }

    return INVALID_PARAMETER;
}

int ct_libnet_get_speed_from_config(const char *port,
                                    char *speed,
                                    size_t speedLen,
                                    libnetSession_t *)
{
    assert(NULL != port);
    assert(NULL != speed);
    assert(speedLen >= sizeof("1000M"));

    auto config = get_interface_config(port);

    if (config)
    {
        snprintf(speed, speedLen, "%dM", config->speed_);
        return SUCCESS;
    }

    return INVALID_PARAMETER;
}

int ct_libnet_get_duplex_from_config(const char *port,
                                     char *duplex,
                                     size_t duplexLen,
                                     libnetSession_t *)
{
    assert(NULL != port);
    assert(NULL != duplex);
    assert(duplexLen >= sizeof("half"));

    auto config = get_interface_config(port);

    if (config)
    {
        switch (config->duplex_)
        {
        case Duplex::FULL:
            strcpy(duplex, "full");
            return SUCCESS;
        case Duplex::HALF:
            strcpy(duplex, "half");
            return SUCCESS;
        default:
            break;
        }
    }

    return INVALID_PARAMETER;
}

int ct_libnet_get_default_via_from_hw(const char *port, char *strDefaultVia, size_t strDefaultViaLen)
{
    assert(NULL != port); // TODO: do we need the port at all?
    assert(NULL != strDefaultVia);
    assert(strDefaultViaLen >= INET_ADDRSTRLEN);

	netlinkSession_t *nlSessionHandle = NULL;

    int status = ct_netlink_init(&nlSessionHandle);

    if(SUCCESS == status)
    {
        status = ct_netlink_get_default_via("dontcare", strDefaultVia, strDefaultViaLen, nlSessionHandle);
    }

    ct_netlink_cleanup(nlSessionHandle);

    return status;
}

int ct_libnet_get_ip_addr_from_config(const char *port,
                                      char *ip,
                                      size_t ipLen,
                                      libnetSession_t *)
{
    assert(NULL != port);
    assert(NULL != ip);
    assert(ipLen >= INET_ADDRSTRLEN);

    auto config = get_ip_config(port);
    if (config)
    {
        strcpy(ip, config->address_.c_str());
        return SUCCESS;
    }

    return INVALID_PARAMETER;
}

int ct_libnet_get_netmask_from_config(const char *port,
                                      char *netmask,
                                      size_t netmaskLen,
                                      enum netmaskFormatModes_t mode,
                                      libnetSession_t *)
{
    assert(NULL != port);
    assert(NULL != netmask);

    int status = SUCCESS;

    if(SUCCESS == status)
    {
        auto config = get_ip_config(port);
        if (config)
        {
            switch(mode)
            {
                case NETMASK_FORMAT_MODE_CIDR:
                    assert(netmaskLen > 2);
                    status = ct_libnet_netmask_to_cidr(
                        config->netmask_.c_str(), netmask, netmaskLen);
                    break;
                case NETMASK_FORMAT_MODE_CLASSIC:
                    assert(netmaskLen >= INET_ADDRSTRLEN);
                    strncpy(netmask, config->netmask_.c_str(), netmaskLen);
                    break;
                default:
                    status = INVALID_PARAMETER;
                    break;
            }
        }
    }

    return status;
}

int ct_libnet_get_broadcast_from_config(const char *port,
                                        char *broadcast,
                                        size_t broadcastLen,
                                        libnetSession_t *)
{
    assert(NULL != port);
    assert(NULL != broadcast);
    assert(broadcastLen >= INET_ADDRSTRLEN);

    auto config = get_ip_config(port);
    if (config)
    {
        strcpy(broadcast, napi::CalculateBroadcast(*config).c_str());
        return SUCCESS;
    }

    return INVALID_PARAMETER;
}

/* Return all active ports */
int ct_libnet_get_ports_list(char *result,
                             size_t resultLen,
                             const char *delim,
                             libnetSession_t *)
{

    napi::BridgeConfig bridge_config;
    auto error = napi::GetBridgeConfig(bridge_config);
    auto ports = boost::algorithm::join(bridge_config.GetBridges(), delim);
    if (error.IsNotOk() || resultLen < ports.size())
        return INVALID_PARAMETER;

    strcpy(result, ports.c_str());
    return SUCCESS;
}

/* Return all possible ports */
int ct_libnet_get_all_ports_list(char *result,
                                 size_t resultLen,
                                 const char *delim,
                                 libnetSession_t *sessionHandle)
{
    // FIXME: use InterfaceInfo from netconf
    return ct_libnet_get_ports_list(result, resultLen, delim, sessionHandle);
}

int ct_libnet_set_ip_addr_to_config(const char *port,
                                    const char *ip,
                                    libnetSession_t *)
{
    assert(NULL != port);
    assert(NULL != ip);

    int status = ERROR;

    if(is_valid_ip(ip))
    {
        auto bridge_name = get_bridge_name(port);
        if(not bridge_name.empty()){
            napi::IPConfigs ip_configs;
            auto error = napi::GetIPConfigs(ip_configs);
            auto ip_config = ip_configs.GetIPConfig(bridge_name);
            if (error.IsOk() && ip_config && ip_config->source_ == IPSource::STATIC)
            {
                ip_config->address_ = ip;
                ip_configs.AddIPConfig(*ip_config);
                error = napi::SetIPConfigs(ip_configs);
                if (error.IsOk())
                {
                    status = SUCCESS;
                }
            }
        }
    }
    else
    {
        set_wbm_error_state(&status, INVALID_PARAMETER, "Invalid IP address.");
    }

    return status;
}

int ct_libnet_set_netmask_to_config(const char *port,
                                    const char *netmask,
                                    libnetSession_t *)
{
    assert(NULL != port);
    assert(NULL != netmask);

    int status = ERROR;

    auto bridge_name = get_bridge_name(port);
    napi::IPConfigs ip_configs;
    auto error = napi::GetIPConfigs(ip_configs);
    if(error.IsOk() && not bridge_name.empty()){
        auto ip_config = ip_configs.GetIPConfig(bridge_name);
        if (ip_config && ip_config->source_ == IPSource::STATIC)
        {
            ip_config->netmask_ = netmask;
            ip_configs.AddIPConfig(*ip_config);
            error = napi::SetIPConfigs(ip_configs);
            if (error.IsOk())
            {
                status = SUCCESS;
            }
        }
    }

    return status;
}

int ct_libnet_set_broadcast_to_config(const char *port,
                                      const char *broadcast,
                                      libnetSession_t *sessionHandle)
{
    assert(NULL != port);
    assert(NULL != broadcast);
    assert(NULL != sessionHandle);

    int status = SUCCESS;

    if(!is_valid_ip(broadcast))
    {
        set_wbm_error_state(&status, INVALID_PARAMETER, "Invalid broadcast address.");
    }

    /* PT-35 04.05.2020: Don't set the broadcast address, it gets computed by netconfd.
       Just validate the parameters and return the status to conform to the published API.
    */

    return status;
}

static inline bool __hasStaticConfig(const char *port, int *status, libnetSession_t *sessionHandle)
{
    bool result = false;

    char configType[sizeof("static")];
    configType[0] = '\0';

    *status = ct_libnet_get_config_type(port, configType, sizeof(configType), sessionHandle);

    if(SUCCESS == *status && (0 == strcmp("static", configType)) )
    {
        result = true;
    }

    return result;
}


int ct_libnet_set_mac_addr(const char *port, const char *mac, libnetSession_t *sessionHandle)
{
    assert(NULL != port);
    assert(NULL != mac);
    assert(NULL != sessionHandle);

    int status = SUCCESS;

    if(!is_valid_mac(mac))
    {
        set_wbm_error_state(&status, INVALID_PARAMETER, "Invalid MAC address.");
    }
    /* (PT-35 04.05.2020): Removed write of MAC address to XML file, it is not used anymore.
        Leaving this function though, because it is public API. */
    return status;
}


int ct_libnet_set_port_state_to_config(const char *port,
                                       const char *state,
                                       libnetSession_t *sessionHandle)
{
    assert(NULL != port);
    assert(NULL != state);
    assert(NULL != sessionHandle);

    int status = SUCCESS;

    if( 0 != strcmp(state, "enabled") &&
        0 != strcmp(state, "disabled"))
    {
        // No need to __setWBMErrorState: can only happen for console/C user
       return INVALID_PARAMETER;
    }

    napi::InterfaceConfigs interface_configs;
    auto error = napi::GetInterfaceConfigs(interface_configs);
    auto config = interface_configs.GetInterfaceConfig(port);
    if (error.IsOk() && config)
    {
        config->state_ = strcmp(state, "enabled") == 0 ? InterfaceState::UP : InterfaceState::DOWN;
        interface_configs.AddInterfaceConfig(*config);
        error = napi::SetInterfaceConfigs(interface_configs);
        if (error.IsNotOk())
            status = ERROR;
    } else
    {
        status = INVALID_PARAMETER;
    }


    return status;
}

int ct_libnet_set_config_type(const char *port,
                              const char *configType,
                              libnetSession_t *sessionHandle)
{
    assert(NULL != port);
    assert(NULL != configType);
    assert(NULL != sessionHandle);

    int status = SUCCESS;

    auto ip_source =
        strcmp(configType, "static") == 0   ? IPSource::STATIC
      : strcmp(configType, "dhcp")   == 0   ? IPSource::DHCP
      : strcmp(configType, "bootp")  == 0   ? IPSource::BOOTP
      : IPSource::UNKNOWN;

    if (ip_source == IPSource::UNKNOWN)
    {
        status = INVALID_PARAMETER;
    }


    if(SUCCESS == status)
    {
        auto bridge_name = get_bridge_name(port);
        napi::IPConfigs ip_configs;
        auto err = napi::GetIPConfigs(ip_configs);
        if(err.IsOk() && not bridge_name.empty())
        {
            auto ip_config = IPConfig{};
            ip_config.source_ = ip_source;
            ip_configs.AddIPConfig(ip_config);
            err = napi::SetIPConfigs(ip_configs);
            if (err.IsNotOk())
            {
                status = ERROR;
            }
        }
        else
        {
            status = INVALID_PARAMETER;
        }
    }

    return status;
}

int ct_libnet_set_port_state_to_hw(const char *port,
                                   const char *state,
                                   libnetSession_t *sessionHandle)
{
    assert(NULL != port);
    assert(NULL != state);
    assert(NULL != sessionHandle);

    int status = SUCCESS;

    char dev[IFNAMSIZ + 1];
    dev[0] = '\0';

	netlinkSession_t *nlSessionHandle = NULL;

    status = ct_netlink_init(&nlSessionHandle);

    if(SUCCESS == status)
    {
        status = port2dev_ip(port, dev, sizeof(dev));
    }

    if(0 == strcmp("enabled", state))
    {
        status = ct_netlink_enable_link(dev, nlSessionHandle);
    }
    else if(0 == strcmp("disabled", state))
    {
        status = ct_netlink_disable_link(dev, nlSessionHandle);
    }
    else
    {
        // No need to __setWBMErrorState: can only happen for console/C user
        status = INVALID_PARAMETER;
    }

    ct_netlink_cleanup(nlSessionHandle);

    return status;
}

// TODO: list of accepted autoneg, speed and duplex values
// TODO: currently unused: eth params are written to config and updated
// via ifdown/ifup
int ct_libnet_set_port_params_to_hw(const char *port,
                                    const char *autoneg,
                                    const char *speed,
                                    const char *duplex,
                                    libnetSession_t *sessionHandle)
{
    assert(NULL != port);
    assert(NULL != autoneg);
    assert(NULL != speed);
    assert(NULL != duplex);
    assert(NULL != sessionHandle);

    int status = SUCCESS;

    enum ct_ethtoolValues tAutoneg;
    enum ct_ethtoolValues tSpeed;
    enum ct_ethtoolValues tDuplex;

    char dev[IFNAMSIZ + 1];
    dev[0] = '\0';

    status = port2dev_ethernet(port, dev, sizeof(dev));

    if(SUCCESS == status)
    {

        if(0 == strcmp("enabled", autoneg))
        {
            tAutoneg = CT_ETHTOOL_AUTONEG_ON;
        }
        else if(0 == strcmp("disabled", autoneg))
        {
            tAutoneg = CT_ETHTOOL_AUTONEG_OFF;
        }
        else
        {
            // No need to __setWBMErrorState: can only happen for console/C user
            status = INVALID_PARAMETER;
        }
    }

    if(SUCCESS == status)
    {

        if(0 == strcmp("10M", speed))
        {
            tSpeed = CT_ETHTOOL_SPEED_10M;
        }
        else if(0 == strcmp("100M", speed))
        {
            tSpeed = CT_ETHTOOL_SPEED_100M;
        }
        else if(0 == strcmp("1000M", speed))
        {
            tSpeed = CT_ETHTOOL_SPEED_1000M;
        }
        else
        {
            // No need to __setWBMErrorState: can only happen for C user
            status = INVALID_PARAMETER;
        }
    }

    if(SUCCESS == status)
    {

        if(0 == strcmp("full", duplex))
        {
            tDuplex = CT_ETHTOOL_DUPLEX_FULL;
        }
        else if(0 == strcmp("half", duplex))
        {
            tDuplex = CT_ETHTOOL_DUPLEX_HALF;
        }
        else
        {
            // No need to __setWBMErrorState: can only happen for C user
            status = INVALID_PARAMETER;
        }
    }

    if(SUCCESS == status)
    {
        status = ct_ethtool_set_port_params(dev, tAutoneg, tSpeed, tDuplex);
    }

    return status;
}

int ct_libnet_set_speed_to_config(const char *port,
                                  const char *speed,
                                  libnetSession_t *)
{
    assert(NULL != port);
    assert(NULL != speed);

    int status = SUCCESS;
    int speed_value = 0;
    int matches = sscanf(speed, "%dM", &speed_value);

    if(matches != 1)
    {
        // No need to __setWBMErrorState: can only happen for console/C user
        status = INVALID_PARAMETER;
    }

    if(SUCCESS == status)
    {
        napi::InterfaceConfigs interface_configs;
        auto error = napi::GetInterfaceConfigs(interface_configs);
        auto config = interface_configs.GetInterfaceConfig(port);
        if (error.IsOk() && config)
        {
            config->speed_ = speed_value;
            interface_configs.AddInterfaceConfig(*config);
            error = napi::SetInterfaceConfigs(interface_configs);
            if (error.IsNotOk())
            {
                status = ERROR;
            }
        }
        else {
            status = INVALID_PARAMETER;
        }
    }

    return status;
}

int ct_libnet_set_autoneg_to_config(const char *port,
                                    const char *autoneg,
                                    libnetSession_t *)
{
    assert(NULL != port);
    assert(NULL != autoneg);

    int status = SUCCESS;
    Autonegotiation aneg;
    if(0 == strcmp("enabled", autoneg))
    {
        aneg = Autonegotiation::ON;
    }
    else if(0 == strcmp("disabled", autoneg))
    {
        aneg = Autonegotiation::OFF;
    }
    else
    {
        return  INVALID_PARAMETER;
    }

    napi::InterfaceConfigs interface_configs;
    auto err = napi::GetInterfaceConfigs(interface_configs);
    auto config = interface_configs.GetInterfaceConfig(port);
    if (err.IsOk() && config)
    {
        config->autoneg_ = aneg;
        interface_configs.AddInterfaceConfig(*config);
        err = napi::SetInterfaceConfigs(interface_configs);
        if (err.IsNotOk())
        {
            status = ERROR;
        }
    }
    else
    {
        status = INVALID_PARAMETER;
    }

    return status;
}

int ct_libnet_set_duplex_to_config(const char *port,
                                   const char *duplex,
                                   libnetSession_t *)
{
    assert(NULL != port);
    assert(NULL != duplex);

    int status = SUCCESS;
    Duplex dup;
    if(0 == strcmp("half", duplex))
    {
        dup = Duplex::HALF;
    }
    else if(0 == strcmp("full", duplex))
    {
        dup = Duplex::FULL;
    }
    else
    {
        return INVALID_PARAMETER;
    }

    napi::InterfaceConfigs interface_configs;
    auto err = napi::GetInterfaceConfigs(interface_configs);
    auto config = interface_configs.GetInterfaceConfig(port);
    if (err.IsOk() && config)
    {
        config->duplex_ = dup;
        interface_configs.AddInterfaceConfig(*config);
        err = napi::SetInterfaceConfigs(interface_configs);
        if (err.IsNotOk())
        {
            status = ERROR;
        }
    }
    else
    {
        status = INVALID_PARAMETER;
    }

    return status;
}

int ct_libnet_commit_changes_all(libnetSession_t *)
{
    return SUCCESS;
}

int ct_libnet_commit_changes_eth(const char *, libnetSession_t *)
{
    return SUCCESS;
}

int ct_libnet_commit_changes_ip(const char *, libnetSession_t *)
{
    return SUCCESS;
}

int ct_libnet_save_xml_file(libnetSession_t *)
{
    return SUCCESS;
}

int ct_generate_network_config(const char *outFilename, libnetSession_t *sessionHandle)
{
    assert(NULL != sessionHandle);

    if (strcmp(NETWORK_INTERFACES_XML, outFilename) == 0)
        return SUCCESS;

    return ct_xml_generate_network_config(outFilename, sessionHandle->xml);
}

int ct_libnet_calculate_broadcast(const char *ipAddr,
                                  const char *netmask,
                                  char *broadcast,
                                  size_t broadcastLen)
{
    int status = SUCCESS;

    assert(NULL != ipAddr);
    assert(NULL != netmask);
    assert(NULL != broadcast);
    assert(broadcastLen >= INET_ADDRSTRLEN);

    char classicNetmask[INET_ADDRSTRLEN];

    struct sockaddr_in ipRepr;
    struct sockaddr_in netmaskRepr;
    struct sockaddr_in bcastRepr;

    if(1 == inet_pton(AF_INET, ipAddr, &(ipRepr.sin_addr)))
    {
        status = SUCCESS;
    }
    else
    {
        status = INVALID_PARAMETER;
    }

    if(SUCCESS == status)
    {
        status = ct_libnet_netmask_to_classic(netmask, classicNetmask, sizeof(classicNetmask));
    }

    if(SUCCESS == status)
    {
        // netmask_to_classic already checks for validity
        (void) inet_pton(AF_INET, classicNetmask, &(netmaskRepr.sin_addr));
    }

    if(SUCCESS == status)
    {
        bcastRepr.sin_addr.s_addr = ipRepr.sin_addr.s_addr | ~(netmaskRepr.sin_addr.s_addr);

        if(NULL == inet_ntop(AF_INET, &(bcastRepr.sin_addr), broadcast, broadcastLen))
        {
            //  EAFNOSUPPORT - actually impossible (address family not supported)
            status = SYSTEM_CALL_ERROR;
        }
    }

    return status;
}

#ifdef __ENABLE_DSA

bool ct_libnet_swconfig_is(const char* alias)
{
  swconfigSession_t *sessionHandle = NULL;
  int status = ct_swconfig_start_session(SWCONFIG_SWITCH_NAME, &sessionHandle);
  const char *found_alias = "";

  if(SUCCESS == status)
  {
    found_alias = ct_swconfig_get_switch_alias(sessionHandle);
  }

  if(NULL != sessionHandle)
  {
      ct_swconfig_finish_session(sessionHandle);
  }

  return strcmp(alias, found_alias) == 0;
}

static bool ct_libnet_swconfig_is_unsupported_switch(void)
{
  return ct_libnet_swconfig_is("mv88e6321");
}

bool ct_libnet_swconfig_default_for_unsupported_switch(const char* default_value, char *value, size_t valueLen)
{
  if (ct_libnet_swconfig_is_unsupported_switch())
  {
    g_strlcpy(value, default_value, valueLen);
    return true;
  }
  return false;
}

static int get_dsa_state__(char * const value, size_t const valueLen, const napi::BridgeConfig& bridge_config)
{
    if(valueLen < 2)
    {
        return NOT_ENOUGH_SPACE_ERROR;
    }

    auto areinsamebridge = bridge_config.AreInSameBridge({"X1", "X2"});
    value[0] = areinsamebridge ? '0' : '1';
    value[1] = '\0';

    return SUCCESS;
}

// Set requested dsa state to hw switch AND configuration file.
int ct_libnet_set_dsa_state(const char *value, libnetSession_t *sessionHandle)
{
    assert(NULL != value);
    assert(NULL != sessionHandle);


    if(0 != strcmp("0", value) && 0 != strcmp("1", value))
    {
        return INVALID_PARAMETER;
    }

    napi::BridgeConfig bridge_config;
    auto error = napi::GetBridgeConfig(bridge_config);
    if (error.IsNotOk())
    {
        return INVALID_PARAMETER;
    }

    // Only change switch hw if old and new dsa state differ.
    char oldValue[2] = {'\0'};
    get_dsa_state__(oldValue, sizeof(oldValue), bridge_config);
    if(0 == strcmp(value, oldValue))
    {
        return SUCCESS;
    }

    if (strcmp("0", value) == 0) {
        bridge_config.AddBridge("br0");
        bridge_config.AssignInterfaceToBridge("X1", "br0");
        bridge_config.AssignInterfaceToBridge("X2", "br0");
        if (bridge_config.BridgeIsEmpty("br1")) {
            bridge_config.DeleteBridge("br1");
        }
    } else if (strcmp("1", value) == 0) {
        bridge_config.AddBridge("br0");
        bridge_config.AssignInterfaceToBridge("X1", "br0");
        bridge_config.AddBridge("br1");
        bridge_config.AssignInterfaceToBridge("X2", "br1");
    }

    return SUCCESS;
}

int ct_libnet_get_dsa_state(char *value, size_t valueLen, libnetSession_t *)
{
    assert(NULL != value);
    napi::BridgeConfig bridge_config;
    napi::GetBridgeConfig(bridge_config);
    return get_dsa_state__(value, valueLen, bridge_config);
}

int ct_libnet_get_actual_dsa_state(char * const value, size_t const valueLen)
{
    assert(NULL != value);
    napi::BridgeConfig bridge_config;
    napi::GetBridgeConfig(bridge_config);
    return get_dsa_state__(value, valueLen, bridge_config);
}

namespace {

int swconfig_set_attr(const char *dev, const char *attr, const char *value, const char *valid_arg_list[])
{
    assert(NULL != value);

    int status = SUCCESS;

    swconfigSession_t *sessionHandle = NULL;

    status = isValidArg(value, valid_arg_list);

    if(SUCCESS == status)
    {
        status = ct_swconfig_start_session(dev, &sessionHandle);
    }

    if(SUCCESS == status)
    {
        status = ct_swconfig_set_attr(attr, value, -1, sessionHandle);
    }

    if(NULL != sessionHandle)
    {
        ct_swconfig_finish_session(sessionHandle);
    }

    return status;
}

int swconfig_get_attr(const char *dev, const char *attr, char *value, size_t valueLen)
{
    int status = SUCCESS;

    swconfigSession_t *sessionHandle = NULL;

    status = ct_swconfig_start_session(dev, &sessionHandle);

    if(SUCCESS == status)
    {
        status = ct_swconfig_get_attr(attr, value, valueLen, sessionHandle);
    }

    if(NULL != sessionHandle)
    {
        ct_swconfig_finish_session(sessionHandle);
    }

    return status;
}

#define BCAST_DEFAULT "0"
const char *bcast_protect_arg_list[] = {"0","1","2","3","4","5",NULL};

} // namespace -> static linkage

int ct_libnet_set_bcast_protect(const char *dev, const char *value)
{
    if (ct_libnet_swconfig_is_unsupported_switch())
      return strcmp(BCAST_DEFAULT, value);
    return swconfig_set_attr(dev, "bcast_protect", value, bcast_protect_arg_list);
}

void ct_libnet_print_bcast_protect_args(void)
{
    printValidArgs(bcast_protect_arg_list);
}

int ct_libnet_get_bcast_protect(const char *dev, char *value, size_t valueLen)
{
    assert(NULL != value);
    assert(valueLen > 2);

    if (ct_libnet_swconfig_default_for_unsupported_switch(BCAST_DEFAULT, value, valueLen))
      return 0;

    return swconfig_get_attr(dev, "bcast_protect", value, valueLen);
}

namespace {
int ct_libnet_is_pfc100(void){
    int retVal = 0;
    tTypeLabel typelabel;

    typelabel = typelabel_OBJECT_New();
    if(TYPELABEL_RETURN_OK == typelabel_OBJECT_Open(typelabel))
    {
        typelabel_OBJECT_Sync(typelabel);
        char *orderNr=typelabel_OBJECT_GetValue(typelabel,"ORDER");
        if (orderNr != NULL)
        {
            //Compare only the beginning of the order-number
            //all PFC100 start with "750-81"
            if (strncmp(orderNr, "750-81xx", 6) == 0)
            {
                //printf("PFC100 Series\n");
                retVal = 1;
            }
            free(orderNr);
        }
        typelabel_OBJECT_Destroy(typelabel);
    }
    return retVal;
}

#define PORT_MIRROR_DEFAULT "0"
const char *port_mirror_arg_list[] = {"0","1","2", NULL};
} // namespace -> static linkage


int ct_libnet_set_port_mirror(const char *dev, const char *value)
{
    //A workaround is needed here because on PFC100 the ports X1 and X2
    //are crosswise connected as on PFC200
    //---BEGIN WORKAROUND---
    // If hostname starts with "PFC100" we have to swap the values 1 and 2
    int iValue = atoi(value);
    char newValue[2];
    newValue[0]='0';
    newValue[1]='\0';

    if (ct_libnet_swconfig_is_unsupported_switch())
      return strcmp(PORT_MIRROR_DEFAULT, value);

    if (ct_libnet_is_pfc100())
    {
        //printf("WORKAROUND FOR PFC100 - VALUE: %s - SETTO:", value);
        switch (iValue)
        {
         case 1:
          newValue[0]='2';
          break;
         case 2:
          newValue[0]='1';
          break;
         default:
          break;
        }
        //printf("%s\n", newValue);
        return swconfig_set_attr(dev, "port_mirror", (const char*) newValue, port_mirror_arg_list);
    }
    //---END WORKAROUND---


    return swconfig_set_attr(dev, "port_mirror", value, port_mirror_arg_list);
}

void ct_libnet_print_port_mirror_args(void)
{
    printValidArgs(port_mirror_arg_list);
}

int ct_libnet_get_port_mirror(const char *dev, char *value, size_t valueLen)
{
    assert(NULL != value);
    assert(valueLen > 1);

    if (ct_libnet_swconfig_default_for_unsupported_switch(PORT_MIRROR_DEFAULT,
                                                          value, valueLen))
      return 0;

    //Original has to be commented out!
    //return __swconfig_get_attr(dev, "port_mirror", value, valueLen);
    //---BEGIN WORKAROUND---
    int status = swconfig_get_attr(dev, "port_mirror", value, valueLen);

    if (SUCCESS == status)
    {
        if (ct_libnet_is_pfc100())
        {
            //If value is "0" we have to do nothing
            if (strncmp(value, "0",1) != 0)
            {
                if (strncmp(value, "1", 1) == 0)
                {
                    g_strlcpy(value, "2", 2);
                }
                else if (strncmp(value, "2", 1) == 0)
                {
                    g_strlcpy(value, "1", 2);
                }
                else
                {
                    status = ERROR;
                }
            }
        }
    }

    return status;
    //---END WORKAROUND---

}

#define RATE_LIMIT_DEFAULT_GET "disabled"
#define RATE_LIMIT_DEFAULT_SET "off"
static const char *rate_limit_arg_list[] = {    "off",
                                                "64.kbps", "128.kbps", "192.kbps", "256.kbps", "320.kbps", "384.kbps", "448.kbps", "512.kbps",
                                                "576.kbps", "640.kbps", "704.kbps", "768.kbps", "832.kbps", "896.kbps", "960.kbps",
                                                "1.mbps", "2.mbps", "3.mbps", "4.mbps", "5.mbps", "6.mbps","7.mbps", "8.mbps", "9.mbps", "10.mbps",
                                                "11.mbps", "12.mbps", "13.mbps", "14.mbps", "15.mbps", "16.mbps", "17.mbps", "18.mbps", "19.mbps", "20.mbps",
                                                "21.mbps", "22.mbps", "23.mbps", "24.mbps", "25.mbps", "26.mbps", "27.mbps", "28.mbps", "29.mbps", "30.mbps",
                                                "31.mbps", "32.mbps", "33.mbps", "34.mbps", "35.mbps", "36.mbps", "37.mbps", "38.mbps", "39.mbps", "40.mbps",
                                                "41.mbps", "42.mbps", "43.mbps", "44.mbps", "45.mbps", "46.mbps", "47.mbps", "48.mbps", "49.mbps", "50.mbps",
                                                "51.mbps", "52.mbps", "53.mbps", "54.mbps", "55.mbps", "56.mbps", "57.mbps", "58.mbps", "59.mbps", "60.mbps",
                                                "61.mbps", "62.mbps", "63.mbps", "64.mbps", "65.mbps", "66.mbps", "67.mbps", "68.mbps", "69.mbps", "70.mbps",
                                                "71.mbps", "72.mbps", "73.mbps", "74.mbps", "75.mbps", "76.mbps", "77.mbps", "78.mbps", "79.mbps", "80.mbps",
                                                "81.mbps", "82.mbps", "83.mbps", "84.mbps", "85.mbps", "86.mbps", "87.mbps", "88.mbps", "89.mbps", "90.mbps",
                                                "91.mbps", "92.mbps", "93.mbps", "94.mbps", "95.mbps", "96.mbps", "97.mbps", "98.mbps", "99.mbps",

                                                NULL};

const char *
ct_libnet_get_rate_limit_by_index (size_t value)
{
  const char *rl;

  if (value >= (sizeof(rate_limit_arg_list)/sizeof(rate_limit_arg_list[0])))
  {
    rl = NULL;
  }
  else
  {
    rl = rate_limit_arg_list[value];
  }

  return rl;
}

int ct_libnet_set_rate_limit(const char *dev, const char *value)
{
    if (ct_libnet_swconfig_is_unsupported_switch())
      return strcmp(RATE_LIMIT_DEFAULT_SET, value);

    return swconfig_set_attr(dev, "rate_limit", value, rate_limit_arg_list);
}

void ct_libnet_print_rate_limit_args(void)
{
    printValidArgs(rate_limit_arg_list);
}

int ct_libnet_get_rate_limit(const char *dev, char *value, size_t valueLen)
{
    assert(NULL != value);
    assert(valueLen > 2);

    if (ct_libnet_swconfig_default_for_unsupported_switch(RATE_LIMIT_DEFAULT_GET, value, valueLen))
      return 0;

    return swconfig_get_attr(dev, "rate_limit", value, valueLen);
}

#define FAST_AGING_DEFAULT "0"
static const char *fast_aging_arg_list[] = {"0","1", NULL};

void ct_libnet_print_fast_aging_args(void)
{
   printValidArgs(fast_aging_arg_list);
}

int ct_libnet_set_fast_aging(const char *dev, const char *value)
{
    if (ct_libnet_swconfig_is_unsupported_switch())
        return strcmp(FAST_AGING_DEFAULT, value);

    return swconfig_set_attr(dev, "fast_aging", value, fast_aging_arg_list);
}

int ct_libnet_get_fast_aging(const char *dev, char *value, size_t valueLen)
{
    assert(NULL != value);
    assert(valueLen > 1);

    if (ct_libnet_swconfig_default_for_unsupported_switch(FAST_AGING_DEFAULT, value, valueLen))
      return 0;

    return swconfig_get_attr(dev, "fast_aging", value, valueLen);
}

int ct_libnet_save_switch_settings_to_config(const char *configFile, ct_switch_settings_t *settings)
{
    // TODO: this function has code copied from ct_xml.c::__reliably_save_to_disk()
    // This should be refactored. AGa, 2014-11-24

    int status = SUCCESS;

    char tmpFilename[strlen(configFile) + sizeof(".XXXXXX")];
    snprintf(tmpFilename, sizeof(tmpFilename), "%s.XXXXXX", configFile);

    int tmpFd = mkstemp(tmpFilename);

    if(-1 == tmpFd)
    {
        perror("mkstemp");
        status = FILE_OPEN_ERROR;
    }

    if(SUCCESS == status)
    {
        // the file created by mkstemp has 600 permissions, change it to 644
        if(-1 == fchmod(tmpFd, S_IWUSR | S_IRUSR | S_IRGRP | S_IROTH))
        {
            perror("fchmod");
            status = FILE_OPEN_ERROR;
        }
    }

    if(SUCCESS == status)
    {
        const char *configFileContents = "#Auto-generated configuration file. Do not change!\n\nPORT_MIRROR=%s\nBCAST_PROTECT=%s\nRATE_LIMIT=%s\nFAST_AGING=%s\n";

        status = dprintf(tmpFd,
                         configFileContents,
                         settings->portMirror,
                         settings->bcastProtect,
                         settings->rateLimit,
                         settings->fastAging);

        if(-1 == status)
        {
            perror("dprintf");
            status = FILE_WRITE_ERROR;
        }
        else
        {
            // status contains number of written bytes
            status = SUCCESS;
        }
    }

    if(SUCCESS == status)
    {
        status = fsync(tmpFd);

        if(-1 == status)
        {
            // possible errnos: EIO: sync error (i.e. not enough free space)
            //                  EROFS, EINVAL: sync not supported (unlikely)
            //                  EBADF: Not a valid writable fd    (impossible, because it has already been written to above)

            perror("fsync");
            status = FILE_WRITE_ERROR;
        }
    }

    if(-1 != tmpFd)
    {
        if(-1 == close(tmpFd))
        {
        // At this point we cannot do anything, but our config might have been destroyed
        // TODO: Ultima ratio: reset to factory settings?
            perror("close");
            status = FILE_WRITE_ERROR;
        }
    }

    if(SUCCESS == status)
    {
        status = rename(tmpFilename, configFile);
        if(-1 == status)
        {
            perror("rename");
            status = FILE_WRITE_ERROR;
        }
        // rename fails: old config not touched
    }

    if(SUCCESS != status)
    {
        set_wbm_error_state(NULL, 0, "Error saving switch settings.\nChanges will not be restored after reboot.");
    }

    return status;
}


#else

// Provide value '0' if platform has no DSA feature.
int ct_libnet_get_dsa_state(char *value, size_t valueLen, libnetSession_t *sessionHandle)
{
    assert(NULL != value);
    assert(NULL != sessionHandle);
    assert(1 < valueLen);

    value[0] = '0';
    value[1] = '\0';
    return SUCCESS;
}

#endif

}
