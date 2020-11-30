// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * TypesHelper.cpp
 *
 *  Created on: 29.10.2019
 *      Author: u014487
 */

#include "TypesTestHelper.hpp"
#include "Error.hpp"
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

std::ostream& operator<<(std::ostream& os, const Error& e){
  os << e.ToString();
  return os;
}

std::ostream& operator<<(std::ostream &os, const ErrorCode &e) {
  auto print = [&](auto s) {
    os << s;
  };
  switch (e) {

    case ::netconf::ErrorCode::OK:
      print("OK");
      break;
    case ::netconf::ErrorCode::GENERIC_ERROR:
      print("GENERIC_ERROR");
      break;
    case ::netconf::ErrorCode::SYSTEM_CALL:
      print("SYSTEM_CALL");
      break;
    case ::netconf::ErrorCode::FILE_READ:
      print("FILE_READ");
      break;
    case ::netconf::ErrorCode::FILE_WRITE:
      print("FILE_WRITE");
      break;
    case ::netconf::ErrorCode::JSON_KEY_MISSING:
      print("JSON_KEY_MISSING");
      break;
    case ::netconf::ErrorCode::JSON_CONVERT:
      print("JSON_CONVERT");
      break;
    case ::netconf::ErrorCode::JSON_INCOMPLETE:
      print("JSON_INCOMPLETE");
      break;
    case ::netconf::ErrorCode::IPV4_FORMAT:
      print("IPV4_FORMAT");
      break;
    case ::netconf::ErrorCode::IP_CONFIG_READONLY:
      print("IP_CONFIG_READONLY");
      break;
    case ::netconf::ErrorCode::IP_CONFIG_SOURCE:
      print("IP_CONFIG_SOURCE");
      break;
    case ::netconf::ErrorCode::IP_INVALID:
      print("IP_INVALID");
      break;
    case ::netconf::ErrorCode::IP_DISTRIBUTED_MULTIPLE_TIMES:
      print("IP_DISTRIBUTED_MULTIPLE_TIMES");
      break;
    case ::netconf::ErrorCode::NETMASK_INVALID:
      print("NETMASK_INVALID");
      break;
    case ::netconf::ErrorCode::NETWORK_CONFLICT:
      print("NETWORK_CONFLICT");
      break;
    case ::netconf::ErrorCode::SYSTEM_EXECUTE:
      print("SYSTEM_EXECUTE");
      break;
    case ::netconf::ErrorCode::ENTRY_DUPLICATE:
      print("ENTRY_DUPLICATE");
      break;
    case ::netconf::ErrorCode::NAME_EMPTY:
      print("NAME_EMPTY");
      break;
    case ::netconf::ErrorCode::INTERFACE_NOT_EXISTING:
      print("INTERFACE_NOT_EXISTING");
      break;
    case ::netconf::ErrorCode::BRIDGE_NAME_INVALID:
      print("BRIDGE_NAME_INVALID");
      break;
    case ::netconf::ErrorCode::DHCP_CLIENT_START:
      print("DHCP_CLIENT_START");
      break;
    case ::netconf::ErrorCode::BOOTP_CLIENT_START:
      print("BOOTP_CLIENT_START");
      break;
    case ::netconf::ErrorCode::PERSISTENCE_WRITE:
      print("PERSISTENCE_WRITE");
      break;
    case ::netconf::ErrorCode::PERSISTENCE_READ:
      print("PERSISTENCE_READ");
      break;
    case ::netconf::ErrorCode::RESTORE_CONTENT:
      print("RESTORE_CONTENT");
      break;
    case ::netconf::ErrorCode::BACKUP_CONTENT_MISSING:
      print("BACKUP_CONTENT_MISSING");
      break;
    case ::netconf::ErrorCode::BACKUP_CONTENT_INVALID:
      print("BACKUP_CONTENT_INVALID");
      break;
    case ::netconf::ErrorCode::BACKUP_VERSION:
      print("BACKUP_VERSION");
      break;
    case ::netconf::ErrorCode::DIP_NOT_AVAILABLE:
      print("DIP_NOT_AVAILABLE");
      break;
    case ::netconf::ErrorCode::SET_IP:
      print("SET_IP");
      break;
    case ::netconf::ErrorCode::SET_INTERFACE:
      print("SET_INTERFACE");
      break;
    case ::netconf::ErrorCode::SET_INTERFACE_STATE:
      print("SET_INTERFACE_STATE");
      break;
    default:
      print("???");
  }
  return os;
}

}  // namespace netconf
