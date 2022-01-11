// SPDX-License-Identifier: GPL-2.0-or-later

#include "BridgeManager.hpp"

#include "CommandExecutor.hpp"
#include "Logger.hpp"

#include <boost/filesystem.hpp>
#include <boost/format.hpp>

#include "EthernetInterface.hpp"
#include "BridgeConfigValidator.hpp"

namespace netconf {

using namespace std::literals;

BridgeManager::BridgeManager(IBridgeController &bridge_controller, IDeviceProperties &properies_provider,
                             INetDevManager &netdev_manager)
    : bridge_controller_ { bridge_controller },
      properies_provider_(properies_provider),
      netdev_manager_ { netdev_manager },
      bridge_configurator_(bridge_controller_),
      bridge_config_transformator_(properies_provider_),
      switch_config_ { "/etc/switch_settings.conf" },
      switch_settings_file_monitor_ { "/etc/switch_settings.conf", [this](FileMonitor&) {
        this->UpdateAgetime();
      } } {
  SetDefaultInterfaceUp();
  UpdateAgetime();
}

Status BridgeManager::SetDefaultInterfaceUp() const {
  // CAUTION: eth0 has to be up, before ethX? devices can be set up.

  Status system_status = bridge_controller_.SetInterfaceUp("eth0");
  if(system_status.IsNotOk()){
    Status status { StatusCode::SET_INTERFACE_STATE, "eth0" };
    LogError(status.ToString() + " " + system_status.ToString());
  }
  return system_status;

}

Status BridgeManager::ApplyBridgeConfig(BridgeConfig const &os_config) const {

  Status status = SetDefaultInterfaceUp();

  if (status.IsOk()) {
    status = bridge_configurator_.Configure(os_config);
  }

  return status;
}

Bridges BridgeManager::GetBridges() const {

  BridgeConfig bridge_config = bridge_configurator_.GetConfiguration();

  Bridges bridges;
  for (auto &bridge_pair : bridge_config) {
    bridges.push_back(bridge_pair.first);
  }

  return bridges;
}

BridgeConfig BridgeManager::GetBridgeConfig() const {

  BridgeConfig config_os = bridge_configurator_.GetConfiguration();
  BridgeConfig config_product = bridge_config_transformator_.ConvertOSToProduct(config_os);
  return config_product;

}

Status BridgeManager::IsValid(BridgeConfig const &product_config) const {
  return BridgeConfigValidator::Validate(product_config, bridge_controller_.GetInterfaces());
}

Status BridgeManager::ApplyBridgeConfiguration(BridgeConfig &product_config) const {

  BridgeConfig os_config = bridge_config_transformator_.ConvertProductToOS(product_config);

  Status status = IsValid(os_config);
  if (status.IsNotOk()) {
    return status;
  }

  if (status.IsOk()) {
    status = ApplyBridgeConfig(os_config);
  }

  if (status.IsOk()) {
    netdev_manager_.ConfigureBridges(product_config);
  }

  if (status.IsOk()) {
    UpdateAgetime();
  }

  return status;
}

Interfaces BridgeManager::GetBridgeAssignedInterfaces() const {
  return bridge_configurator_.GetBridgeAssignedInterfaces();
}

void BridgeManager::UpdateAgetime() const {
  auto bridges = bridge_controller_.GetBridges();
  auto age_time = switch_config_.GetFastAgeing() ? 0 : 300;
  LogInfo("UpdateAgetime");
  ::std::for_each(bridges.begin(), bridges.end(), [age_time, this](Bridge &b) {
    this->bridge_controller_.SetAgetime(b, age_time);
  });
}

} /* namespace netconf */
