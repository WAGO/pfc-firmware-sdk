/*
 * TypesHelper.cpp
 *
 *  Created on: 29.10.2019
 *      Author: u014487
 */

#include "TypesTestHelper.hpp"
#include <tuple>

namespace netconfd {

using ::std::tie;

bool operator==(const netconfd::InterfaceConfig& a, const netconfd::InterfaceConfig& b){
  auto a_tuple = std::tie(a.autoneg_, a.device_name_, a.duplex_, a.speed_, a.state_);
  auto b_tuple = std::tie(b.autoneg_, b.device_name_, b.duplex_, b.speed_, b.state_);
  return a_tuple == b_tuple;
}


std::ostream& operator<<(std::ostream& os, const InterfaceConfig& pc) {
  os << "{Device: " << pc.device_name_ << ", ";
  os << "State: " << static_cast<int>(pc.state_) << ", ";
  os << "Autoneg: " << static_cast<int>(pc.autoneg_) << ", ";
  os << "Speed: " << pc.speed_ << ", ";
  os << "Duplex: " << static_cast<int>(pc.duplex_) << "}";
  return os;
}

static std::string IpSourceToString(IPSource s){
  switch (s) {
    case IPSource::BOOTP:
      return "BOOTP";
    case IPSource::DHCP:
      return "DHCP";
    case IPSource::STATIC:
      return "STATIC";
    case IPSource::TEMPORARY:
      return "TEMPORARY";
    case IPSource::NONE:
      return "NONE";
    case IPSource::UNKNOWN:
      return "UNKNOWN";
    default:
      return "???";
  }
}

std::ostream& operator<<(std::ostream& os, const IPConfig& pc){
  os << "{Source: " << IpSourceToString(pc.source_) << ", ";
  os << "IP: " << pc.address_ << ", ";
  os << "Mask: " << pc.netmask_ << ", ";
  os << "Mask: " << pc.netmask_ << ", ";
  os << "Bcast: " << pc.broadcast_ << "}";
  return os;
}

}  // namespace netconfd
