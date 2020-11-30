// SPDX-License-Identifier: GPL-2.0-or-later

#include "BridgeManager.hpp"

#include "CommandExecutor.hpp"
#include "Logger.hpp"

#include <boost/filesystem.hpp>
#include <boost/format.hpp>

#include "EthernetInterface.hpp"

namespace netconf {

using namespace std::literals;

BridgeManager::BridgeManager(IBridgeController &bridge_controller, IDeviceProperties &properies_provider,
                             INetDevManager &netdev_manager)
    : bridge_controller_ { bridge_controller },
      properies_provider_(properies_provider),
      netdev_manager_ { netdev_manager },
      interface_validator_(bridge_controller_),
      bridge_configurator_(bridge_controller_),
      bridge_config_transformator_(properies_provider_),
      switch_config_ { "/etc/switch_settings.conf" },
      switch_settings_file_monitor_ { "/etc/switch_settings.conf", [this](FileMonitor&) {
        this->UpdateAgetime();
      } } {
  SetDefaultInterfaceUp();
  UpdateAgetime();
}

Error BridgeManager::SetDefaultInterfaceUp() const {
  // CAUTION: eth0 has to be up, before ethX? devices can be set up.

  Error system_error = bridge_controller_.SetInterfaceUp("eth0");
  if(system_error.IsNotOk()){
    Error error { ErrorCode::SET_INTERFACE_STATE, "eth0" };
    LogError(error.ToString() + " " + system_error.ToString());
  }
  return system_error;

}

Error BridgeManager::ApplyBridgeConfig(BridgeConfig const &os_config) const {

  Error error = SetDefaultInterfaceUp();

  if (error.IsOk()) {
    error = bridge_configurator_.Configure(os_config);
  }

  return error;
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

Error BridgeManager::IsValid(BridgeConfig const &product_config) const {

  Error error;

  if (product_config.empty()) {
    error.Set(ErrorCode::JSON_INCOMPLETE);
  }

  BridgeConfig os_config;
  if (error.IsOk()) {
    error = interface_validator_.Validate(product_config);
  }

  return error;

}

Error BridgeManager::ApplyBridgeConfiguration(BridgeConfig &product_config) const {

  BridgeConfig os_config = bridge_config_transformator_.ConvertProductToOS(product_config);

  Error error = IsValid(os_config);
  if (error.IsNotOk()) {
    return error;
  }

  if (error.IsOk()) {
    error = ApplyBridgeConfig(os_config);
  }

  if (error.IsOk()) {
    netdev_manager_.ConfigureBridges(product_config);
  }

  if (error.IsOk()) {
    UpdateAgetime();
  }

  return error;
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
