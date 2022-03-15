// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include <map>
#include <string>

#include "IBridgeInformation.hpp"
#include "IEthernetInterfaceFactory.hpp"
#include "IEthernetInterface.hpp"
#include "IInterfaceInformation.hpp"
#include "IPersistence.hpp"
#include "INetDevManager.hpp"
#include "InterfaceInformation.hpp"

namespace netconf {

class InterfaceConfigManager : public IInterfaceInformation{
 public:

  InterfaceConfigManager(INetDevManager& netdev_manager,
      IPersistence<InterfaceConfigs>& persistence_provider,
      IEthernetInterfaceFactory& eth_factory);
  virtual ~InterfaceConfigManager() = default;

  void InitializePorts(InterfaceState initalPortState = InterfaceState::UNKNOWN);
  Status Configure(InterfaceConfigs& port_configs);
  InterfaceConfigs const& GetPortConfigs() override;
  InterfaceStatuses GetCurrentPortStatuses() override;
  InterfaceInformations GetInterfaceInformations() override;

 private:
  InterfaceInformation GetInterfaceInformation(const NetDev& netdev) const;
  Status ApplyPortConfig(InterfaceConfig const& cfg);
  Status ApplyPortConfigs(InterfaceConfigs& port_configs);
  void InitializeEthernetInterfaceMap(const NetDevs& netdevs);
  void InitializeCurrentConfigs(const NetDevs& netdevs, const InterfaceConfigs& persistet_configs);
  void UpdateCurrentInterfaceConfigs(const InterfaceConfigs &port_configs);

  INetDevManager& netdev_manager_;
  IPersistence<InterfaceConfigs>& persistence_provider_;
  IEthernetInterfaceFactory& ethernet_interface_factory_;
  ::std::map<::std::string, ::std::unique_ptr<IEthernetInterface>> ethernet_interfaces_;
  InterfaceConfigs current_config_;
};

} /* namespace netconf */
