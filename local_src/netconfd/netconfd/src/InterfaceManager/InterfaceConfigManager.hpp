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

namespace netconf {

class InterfaceConfigManager : public IInterfaceInformation{
 public:
  InterfaceConfigManager(INetDevManager& netdev_manager,
      IPersistence<InterfaceConfigs>& persistence_provider,
      IEthernetInterfaceFactory& eth_factory);
  virtual ~InterfaceConfigManager() = default;

  virtual void InitializePorts();
  virtual Error Configure(InterfaceConfigs& port_configs);
  virtual InterfaceConfigs const& GetPortConfigs() override;

 private:
  Error IsPortConfigValid(const InterfaceConfigs& port_configs);
  Error ApplyPortConfig(InterfaceConfig const& cfg);
  Error ApplyPortConfigs(InterfaceConfigs& port_configs);
  void InitializeEthernetInterfaceMap(const NetDevs& netdevs);
  void InitializeCurrentConfigs(const NetDevs& netdevs, const InterfaceConfigs& persistet_configs);
  void UpdateCurrentInterfaceConfigs(const InterfaceConfigs &port_configs);

  IPersistence<InterfaceConfigs>& persistence_provider_;
  IEthernetInterfaceFactory& ethernet_interface_factory_;
  ::std::map<::std::string, ::std::unique_ptr<IEthernetInterface>> ethernet_interfaces_;
  InterfaceConfigs current_config_;
};

} /* namespace netconf */
