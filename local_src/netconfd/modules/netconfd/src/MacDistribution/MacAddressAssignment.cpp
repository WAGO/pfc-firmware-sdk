// SPDX-License-Identifier: GPL-2.0-or-later

#include "MacAddressAssignment.hpp"
#include "Types.hpp"
#include "Helper.hpp"

namespace netconf {

uint32_t MacAddressAssignment::OtherMacs(const Interfaces &interfaces) const {
  if (IsIncluded("usb0", interfaces)) {
    return 1;
  }
  return 0;
}

bool MacAddressAssignment::IsMacAddressAssignmentMultiple(uint32_t mac_count, uint32_t port_count) const {
  if (not IsMacAddressAssignmentFull(mac_count, port_count) && not IsMacAddressAssignmentSingle(mac_count)) {
    if (mac_count >= port_count) {
      return true;
    }
  }
  return false;
}

bool MacAddressAssignment::IsMacAddressAssignmentFull(uint32_t mac_count, uint32_t port_count) const {
  auto max_bridges_with_exclusiv_mac = port_count / 2;
  auto required_macs = port_count + max_bridges_with_exclusiv_mac;

  if (mac_count >= required_macs) {
    return true;
  }
  return false;
}

MacAddressAssignment::MacAddressAssignment(IDeviceProperties &properties_provider)
    : properties_provider_ { properties_provider } {

  Interfaces interfaces = properties_provider_.GetOSInterfaceNames();
  auto other_macs_ = OtherMacs(interfaces);

  auto mac_count = properties_provider_.GetMacCount();

  auto eth_macs = mac_count - other_macs_;
  auto ports = properties_provider_.GetOSPortNames().size();

  if (IsMacAddressAssignmentFull(eth_macs, ports)) {
    mode_ = Mode::Full;
  } else if (IsMacAddressAssignmentMultiple(eth_macs, ports)) {
    mode_ = Mode::Multiple;
  } else {
    mode_ = Mode::Single;
  }
}

MacAddressAssignment::Mode MacAddressAssignment::GetMode() {
  return mode_;
}

::std::string MacAddressAssignment::GetBaseMac() const {
  return properties_provider_.GetIncrementedMac(0);
}

}
