// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include <map>
#include <string>

#include "IInterfaceInformation.hpp"
#include "IJsonConfigConverter.hpp"
#include "IEthernetInterfaceFactory.hpp"
#include "IEthernetInterface.hpp"
#include "IPersistence.hpp"
#include "INetDevManager.hpp"

namespace netconfd {

class InterfaceConfigManager {
 public:
  InterfaceConfigManager(INetDevManager& netdev_manager,
      IPersistence<InterfaceConfigs>& persistence_provider,
      IJsonConfigConverter& json_config_converter,
      IEthernetInterfaceFactory& eth_factory);
  virtual ~InterfaceConfigManager() = default;

  virtual Status Configure(InterfaceConfigs& port_configs);
  virtual InterfaceConfigs const& GetPortConfigs();

 private:
  Status IsPortConfigValid(const InterfaceConfigs& port_configs);
  Status ApplyPortConfig(InterfaceConfig const& cfg);
  Status ApplyAllConfigs(InterfaceConfigs& port_configs);
  void InitializeEthernetInterfaceMap(const NetDevs& netdevs);
  void InitializePortConfigs(const NetDevs& netdevs, const InterfaceConfigs& persistet_configs);
  void UpdateCurrentConfig(const InterfaceConfigs& port_configs);

  IPersistence<InterfaceConfigs>& persistence_provider_;
  IJsonConfigConverter& json_config_converter_;
  IEthernetInterfaceFactory& ethernet_interface_factory_;
  ::std::map<::std::string, ::std::unique_ptr<IEthernetInterface>> ethernet_interfaces_;
  InterfaceConfigs current_config_;
};

} /* namespace netconfd */

