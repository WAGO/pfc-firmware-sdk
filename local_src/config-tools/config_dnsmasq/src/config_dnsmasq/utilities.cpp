//------------------------------------------------------------------------------
/// Copyright (c) 2020-2022 WAGO GmbH & Co. KG
///
/// PROPRIETARY RIGHTS of WAGO GmbH & Co. KG are involved in
/// the subject matter of this material. All manufacturing, reproduction,
/// use, and sales rights pertaining to this subject matter are governed
/// by the license agreement. The recipient of this software implicitly
/// accepts the terms of the license.
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
///
///  \brief    Collection of utility functions.
///
///  \author   MSc : WAGO GmbH & Co. KG
///  \author   MOe : WAGO GmbH & Co. KG
//------------------------------------------------------------------------------

#include <boost/algorithm/string.hpp>
#include <boost/asio.hpp>

#include "defines.hpp"
#include "utilities.hpp"
#include "error_handling.hpp"

using ::boost::asio::ip::make_address_v4;
using ::boost::system::error_code;

namespace configdnsmasq::utilities {

namespace {

// Character code to character class. Available classes:
//   0 = Illegal characters
//   1 = digits
//   2 = alphabetic
//   3 = Hyphen
//   4 = Label terminating '.'
//   5 = String terminating '\0'
static char conv_dnchar[] = {
    5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 4, 0,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0,
    0, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
    2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 0, 0, 0, 0, 0,
    0, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
    2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

// State transition table.
static conv_state_t conv_transitions[] = {
    /*             ill. digit    letter   -       .      \0 */
    /* STOP */     ERR, ERR,     ERR,     ERR,    ERR,   ERR,
    /* START */    ERR, ERR,     INLABEL, ERR,    ERR,   ERR,
    /* INLABEL */  ERR, INLABEL, INLABEL, HYPHEN, START, STOP,
    /* HYPHEN */   ERR, INLABEL, INLABEL, HYPHEN, ERR,   ERR,
};

// Convert one ascii character '0' to '9' and 'a' - 'f' to decimal 0 - 15.
// Upper case letters work as well.
int hexval(int c) {
  static int8_t ktab[] = {
      0,  1,  2,  3,  4,  5,  6,  7,  8,  9, -1, -1, -1, -1, -1, -1,
     -1, 10, 11, 12, 13, 14, 15, -1, -1, -1, -1, -1, -1, -1, -1, -1,
     -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
     -1, 10, 11, 12, 13, 14, 15, -1, -1, -1, -1, -1, -1, -1, -1, -1,
 };
  int rv = -1;

  c -= 0x30;
  if (c >= 0 && c < 0x40) {
    rv = ktab[c];
  }
  return rv;
}

}  // Anonymous namespace

// Convert IP-Address from dotted ASCII string to uint32_t.
eStatusCode conv_ip_ascii2bin(uint32_t &bin_address, const ::std::string &address) {
  error_code error;
  bin_address = make_address_v4(address, error).to_uint();
  if (error) {
    bin_address = 0;
    return INVALID_PARAMETER;
  }
  return SUCCESS;
}

bool is_valid_ip_string(const ::std::string &address) {
  uint32_t bin_address;
  auto status = conv_ip_ascii2bin(bin_address, address);

  return SUCCESS == status;
}

::std::string conv_ip_bin2ascii(const uint32_t address) {
  return make_address_v4(address).to_string();
}

eStatusCode conv_hosts_ascii2bin(const std::string &hostsstr, const uint32_t netmask,
                                 std::vector<dhcpd_host_t> &hosts) {
  eStatusCode status = SUCCESS;

  if(hostsstr.empty()) {
    return status;
  }

  std::vector < std::string > hostsString;
  ::boost::split(hostsString, hostsstr, ::boost::is_any_of(","));

  for (auto &host : hostsString) {
    dhcpd_host_t hp;

    ::std::vector < ::std::string > parts;
    ::boost::split(parts, host, ::boost::is_any_of("_"));

    if (parts.size() != 2) {
      hosts.clear();
      status = eStatusCode::INVALID_PARAMETER;
      break;
    }
    if (!conv_is_valid_mac(parts[0]) && !conv_is_valid_domain_name(parts[0])) {
      hosts.clear();
      status = eStatusCode::INVALID_PARAMETER;
      break;
    }
    hp.mac = parts[0];
    uint32_t ip;
    status = conv_ip_ascii2bin(ip, parts[1]);
    if (SUCCESS != status) {
      break;
    }
    hp.ip_net_part = ip & netmask;
    hp.ip_host_part = ip & ~netmask;

    hosts.push_back(hp);
  }

  return status;
}

std::string conv_hosts_bin2ascii(const std::vector<dhcpd_host_t> &hosts) {
  std::list < std::string > strings;
  for (auto host : hosts) {
    auto ip = conv_ip_bin2ascii(host.ip_net_part | host.ip_host_part);
    strings.push_back(host.mac + "_" + ip);
  }
  return boost::algorithm::join(strings, ",");
}

// Statemachine to check domain/host name according to RFC 1034.
bool conv_is_valid_domain_name(const ::std::string &name) {
  //return std::regex_match(name, std::regex("^(?![0-9]+)(?!-)[a-zA-Z0-9-]{0,63}(?<!-)$"));
  bool isname = true;
  int labelidx = 0;
  int charidx = 0;
  size_t i = 0;
  conv_state_t state = START;

  while (state > STOP && isname && charidx <= 255) {
    unsigned int c = (i >= name.size()) ? 0 : name.at(i) & 255;
    int ccl = conv_dnchar[c];
    conv_state_t nstate = conv_transitions[state * 6 + ccl];
    switch (state) {
      case START:
        isname = (ccl == 2);
        labelidx = 0;
        break;
      case INLABEL:
        isname = (ccl > 0);
        break;
      case HYPHEN:
        isname = (ccl == 1 || ccl == 2 || ccl == 3);
        break;
      default:
        break;
    }
    state = nstate;
    i++;
    labelidx++;
    charidx++;
  }
  return isname && state == STOP && labelidx <= 64;
}

// Returns true if mac address is valid. Checks also for broadcast and multicast.
bool conv_is_valid_mac(const ::std::string &mac_in) {
  static char states[] = { 1, 1, 2, 1, 1, 2, 1, 1, 2, 1, 1, 2, 1, 1, 2, 1, 1, 0 };
  bool result = false;

  if(mac_in.size() != 17) {
    return false;
  }

  for (size_t i = 0; i <= mac_in.size(); i++) {
    if (states[i] == 0) {
      if (mac_in.size() == i) {
        result = true;
      }
      break;
    } else if (states[i] == 1) {
      if (hexval(mac_in.at(i)) < 0) {
        break;
      }
    } else if (states[i] == 2) {
      if (mac_in.at(i) != ':') {
        break;
      }
    }
  }

  // Check for broadcast or multicast MAC address.
  if (hexval(mac_in[1]) & 1) {
    result = false;
  }
  return result;
}
}  //namespace configdnsmasq::utilities
