// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include "MacAddressAssignment.hpp"
#include "IMacDistributor.hpp"
#include "IDeviceProperties.hpp"
#include "IMacController.hpp"
#include "Types.hpp"
#include "NetDev.hpp"

namespace netconf {

class MacDistributor : public IMacDistributor{
 public:
  MacDistributor(MacAddress mac_address, uint32_t mac_inc, IMacController &mac_controller);
  virtual ~MacDistributor() = default;

  MacDistributor(const MacDistributor&) = delete;
  MacDistributor& operator=(const MacDistributor&) = delete;
  MacDistributor(const MacDistributor&&) = delete;
  MacDistributor& operator=(const MacDistributor&&) = delete;

  void AssignMacs(NetDevs &net_devs) override;

 private:

  MacAddress base_mac_address_;
  uint32_t mac_inc_;

  IMacController& mac_controller_;

  uint32_t mac_counter_;


  void AssignFullMacSupport(NetDevs& net_devs);
  void AssignSingleMacSupport(NetDevs &net_devs);
  void AssignMultipleMacSupport(NetDevs &net_devs);

  void AssignMac(NetDevPtr net_dev, MacAddress mac);
  void AssignMac(NetDevPtr net_dev);
  void AssignMacAndIncrement(NetDevPtr net_dev);

  bool IsMacAddressAssignmentMultiple(uint32_t mac_count, uint32_t port_count) const;
  bool IsMacAddressAssignmentFull(uint32_t mac_count, uint32_t port_count) const;
};

}
