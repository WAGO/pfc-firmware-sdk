// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * TypesHelper.cpp
 *
 *  Created on: 29.10.2019
 *      Author: u014487
 */

#include "TypesTestHelper.hpp"
#include "Status.hpp"
#include <tuple>

namespace netconf {

std::ostream& operator<<(std::ostream& os, const InterfaceConfig& pc) {
  os << "{Device: " << pc.device_name_ << ", ";
  os << "State: " << static_cast<int>(pc.state_) << ", ";
  os << "Autoneg: " << static_cast<int>(pc.autoneg_) << ", ";
  os << "Speed: " << pc.speed_ << ", ";
  os << "Duplex: " << static_cast<int>(pc.duplex_) << "}";
  return os;
}

static std::string IpSourceToString(::netconf::IPSource s){
  switch (s) {
    case ::netconf::IPSource::BOOTP:
      return "BOOTP";
    case ::netconf::IPSource::DHCP:
      return "DHCP";
    case ::netconf::IPSource::STATIC:
      return "STATIC";
    case ::netconf::IPSource::TEMPORARY:
      return "TEMPORARY";
    case ::netconf::IPSource::NONE:
      return "NONE";
    case ::netconf::IPSource::UNKNOWN:
      return "UNKNOWN";
    default:
      return "???";
  }
}

std::ostream& operator<<(std::ostream& os, const IPConfig& pc){
  os << "{Source: " << IpSourceToString(pc.source_) << ", ";
  os << "IP: " << pc.address_ << ", ";
  os << "Mask: " << pc.netmask_ << ", ";
  return os;
}

std::ostream& operator<<(std::ostream& os, const Status& e){
  os << e.ToString();
  return os;
}

std::ostream& operator<<(std::ostream &os, const StatusCode &e) {
  auto print = [&](auto s) {
    os << s;
  };
  switch (e) {

    case ::netconf::StatusCode::OK:
      print("OK");
      break;
    case ::netconf::StatusCode::GENERIC_ERROR:
      print("GENERIC_ERROR");
      break;
    case ::netconf::StatusCode::SYSTEM_CALL:
      print("SYSTEM_CALL");
      break;
    case ::netconf::StatusCode::FILE_READ:
      print("FILE_READ");
      break;
    case ::netconf::StatusCode::FILE_WRITE:
      print("FILE_WRITE");
      break;
    case ::netconf::StatusCode::JSON_KEY_MISSING:
      print("JSON_KEY_MISSING");
      break;
    case ::netconf::StatusCode::JSON_CONVERT:
      print("JSON_CONVERT");
      break;
    case ::netconf::StatusCode::JSON_INCOMPLETE:
      print("JSON_INCOMPLETE");
      break;
    case ::netconf::StatusCode::IPV4_FORMAT:
      print("IPV4_FORMAT");
      break;
    case ::netconf::StatusCode::IP_CONFIG_READONLY:
      print("IP_CONFIG_READONLY");
      break;
    case ::netconf::StatusCode::IP_CONFIG_SOURCE:
      print("IP_CONFIG_SOURCE");
      break;
    case ::netconf::StatusCode::IP_INVALID:
      print("IP_INVALID");
      break;
    case ::netconf::StatusCode::IP_DISTRIBUTED_MULTIPLE_TIMES:
      print("IP_DISTRIBUTED_MULTIPLE_TIMES");
      break;
    case ::netconf::StatusCode::NETMASK_INVALID:
      print("NETMASK_INVALID");
      break;
    case ::netconf::StatusCode::NETWORK_CONFLICT:
      print("NETWORK_CONFLICT");
      break;
    case ::netconf::StatusCode::SYSTEM_EXECUTE:
      print("SYSTEM_EXECUTE");
      break;
    case ::netconf::StatusCode::ENTRY_DUPLICATE:
      print("ENTRY_DUPLICATE");
      break;
    case ::netconf::StatusCode::NAME_EMPTY:
      print("NAME_EMPTY");
      break;
    case ::netconf::StatusCode::INTERFACE_NOT_EXISTING:
      print("INTERFACE_NOT_EXISTING");
      break;
    case ::netconf::StatusCode::BRIDGE_NAME_INVALID:
      print("BRIDGE_NAME_INVALID");
      break;
    case ::netconf::StatusCode::DHCP_CLIENT_START:
      print("DHCP_CLIENT_START");
      break;
    case ::netconf::StatusCode::BOOTP_CLIENT_START:
      print("BOOTP_CLIENT_START");
      break;
    case ::netconf::StatusCode::PERSISTENCE_WRITE:
      print("PERSISTENCE_WRITE");
      break;
    case ::netconf::StatusCode::PERSISTENCE_READ:
      print("PERSISTENCE_READ");
      break;
    case ::netconf::StatusCode::RESTORE_CONTENT:
      print("RESTORE_CONTENT");
      break;
    case ::netconf::StatusCode::BACKUP_CONTENT_MISSING:
      print("BACKUP_CONTENT_MISSING");
      break;
    case ::netconf::StatusCode::BACKUP_CONTENT_INVALID:
      print("BACKUP_CONTENT_INVALID");
      break;
    case ::netconf::StatusCode::BACKUP_VERSION:
      print("BACKUP_VERSION");
      break;
    case ::netconf::StatusCode::DIP_NOT_AVAILABLE:
      print("DIP_NOT_AVAILABLE");
      break;
    case ::netconf::StatusCode::SET_IP:
      print("SET_IP");
      break;
    case ::netconf::StatusCode::SET_INTERFACE:
      print("SET_INTERFACE");
      break;
    case ::netconf::StatusCode::SET_INTERFACE_STATE:
      print("SET_INTERFACE_STATE");
      break;
    default:
      print("???");
  }
  return os;
}

}  // namespace netconf
