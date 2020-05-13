// SPDX-License-Identifier: GPL-2.0-or-later

#include "NetworkConfigBrain.hpp"

#include "Logger.hpp"
#include "Helper.hpp"
#include "TypesHelper.hpp"
#include "IpAddressManipulator.hpp"
#include "IInterfaceMonitor.hpp"
#include "EthernetInterface.hpp"
#include "IInterfaceInformation.hpp"
#include "InterfaceConfigManager.hpp"
#include "NetDevManager.hpp"
#include "IpConfigHelper.hpp"
#include <boost/format.hpp>
#include <string>

namespace netconfd {

// NOLINTNEXTLINE(cert-err58-cpp) This won't fail
const IPConfigs FIX_IP_CONFIG = IPConfigs( { { "br0", IPSource::TEMPORARY, "192.168.1.17", "255.255.255.0", ZeroIP } });

using namespace ::std::literals;

NetworkConfigBrain::NetworkConfigBrain(IBridgeManager &interface_manager, IInterfaceInformation &itf_info,
                                       IIPManager &ip_manager, IEventManager &event_manager,
                                       IDeviceProperties &device_properties_provider,
                                       IJsonConfigConverter &json_config_converter,
                                       IPersistenceProvider &persistence_provider, IDipSwitch &ip_dip_switch,
                                       InterfaceConfigManager &interface_config_manager, INetDevManager &netdev_manager)

    : bridge_manager_ { interface_manager },
      itf_info_ { itf_info },
      interface_config_manager_ { interface_config_manager },
      ip_manager_ { ip_manager },
      event_manager_ { event_manager },
      device_properties_provider_ { device_properties_provider },
      json_config_converter_ { json_config_converter },
      persistence_provider_ { persistence_provider },
      netdev_manager_ { netdev_manager },
      ip_dip_switch_ { ip_dip_switch } {
}

Status NetworkConfigBrain::Start() {
  Status statusBridgeConfig;
  Status statusIPConfig;
  Status statusDIPSwitchIPConfig;
  Status status;

  BridgeConfig bridge_config;
  IPConfigs ip_configs;
  DipSwitchIpConfig dip_switch_ip_config;

  Interfaces interfaces = device_properties_provider_.GetProductInterfaces();
  if (interfaces.empty()) {
    status.Prepend(StatusCode::ERROR, "Failed to determine available product interfaces.");
    return status;
  }

  statusBridgeConfig = persistence_provider_.Read(bridge_config);
  statusIPConfig = persistence_provider_.Read(ip_configs);
  statusDIPSwitchIPConfig = persistence_provider_.Read(dip_switch_ip_config);

  if (statusBridgeConfig.Ok()) {
    FilterEmptyBridges(bridge_config);
    statusBridgeConfig = FilterRequiredBridgeConfigs(bridge_config);
  }

  if (statusBridgeConfig.Ok()) {
    if (statusIPConfig.NotOk()) {
      LogDebug("Reset IP configuration.");
      ResetIpConfigsToDefault(ip_configs);
    }
  } else {
    ResetBridgeConfigToDefault(interfaces, bridge_config);
    LogDebug(status.GetMessage());
    ResetIpConfigsToDefault(ip_configs);
  }

  FilterRequiredIpConfigs(ip_configs, bridge_config);
  RemoveUnnecessaryIPParameter(ip_configs);

  status = ip_manager_.ValidateIPConfigs(ip_configs, true);
  if (status.NotOk()) {
    LogWarning("IP validation failed. Reset IP configuration.");
    ResetIpConfigsToDefault(ip_configs);
    status = Status { };
  }

  // At this point bridge and IP configs should be valid.

  if (statusDIPSwitchIPConfig.NotOk()) {
    LogDebug("Reset DIP switch IP configuration.");
    ResetDIPSwitchIPConfigToDefault(dip_switch_ip_config);
  }

  persistence_provider_.Write(bridge_config);
  persistence_provider_.Write(ip_configs);
  persistence_provider_.Write(dip_switch_ip_config);

  if (status.Ok()) {
    status = ApplyConfig(bridge_config, ip_configs);
    if (status.NotOk()) {
      status.Prepend("Failed to apply configuration.");
    }
  }

  if (status.Ok()) {
    event_manager_.ProcessEvents();
  }

  return status;
}

void NetworkConfigBrain::GetValidIpConfigsSubset(const IPConfigs &configs, IPConfigs &subset) {
  subset.clear();
  for (const auto &config : configs) {
    subset.push_back(config);
    Status status = ip_manager_.ValidateIPConfigs(subset, true);
    if (status.NotOk()) {
      // Remove element that leads to an invalid configuration.
      subset.pop_back();
    }
  }
}

void NetworkConfigBrain::ResetBridgeConfigToDefault(Interfaces product_interfaces, BridgeConfig &config) {
  config = { { "br0", product_interfaces } };
}

void NetworkConfigBrain::ResetIpConfigsToDefault(IPConfigs &configs) {
  configs = { { "br0", IPSource::DHCP, ZeroIP, ZeroIP, ZeroIP } };
}

void NetworkConfigBrain::ResetDIPSwitchIPConfigToDefault(DipSwitchIpConfig &config) {
  config = { "192.168.1.0", "255.255.255.0" };
}

void NetworkConfigBrain::FilterEmptyBridges(BridgeConfig &product_config) const {

  auto it = product_config.cbegin();
  while (it != product_config.cend()) {
    if (it->second.empty()) {
      it = product_config.erase(it);
    } else {
      it++;
    }
  }
}

void NetworkConfigBrain::FilterRequiredIpConfigs(IPConfigs &ip_configs, const BridgeConfig &bridge_config) const {

  Interfaces bridges_to_remove;
  for (auto &ip_config : ip_configs) {
    if (bridge_config.count(ip_config.interface_) == 0) {
      bridges_to_remove.push_back(ip_config.interface_);
    }
  }

  for (auto bridge : bridges_to_remove) {
    ip_configs.erase(::std::remove_if(ip_configs.begin(), ip_configs.end(), [&](IPConfig &ip_config) {
      return ip_config.interface_ == bridge;
    })
                     , ip_configs.end());
  }
}

Status NetworkConfigBrain::FilterRequiredBridgeConfigs(BridgeConfig &bridge_config) const {

  Status status;

  Interfaces supported_interfaces = device_properties_provider_.GetProductInterfaces();
  if (supported_interfaces.empty()) {
    status.Prepend(StatusCode::ERROR, " No device specific interfaces found.");
  }

  for (auto &bridge : bridge_config) {

    Interfaces interfaces_to_remove;

    for (auto &bridge_interface : bridge.second) {
      if (IsNotIncluded(bridge_interface, supported_interfaces)) {
        interfaces_to_remove.push_back(bridge_interface);
      }
    }

    for (auto &interface : interfaces_to_remove) {
      RemoveByValue(interface, bridge.second);
    }
  }

  return status;
}

IPConfigs NetworkConfigBrain::FilterRequiredIPConfigs(const IPConfigs &ip_configs,
                                                      const BridgeConfig &product_config) const {

  IPConfigs required_ip_configs;
  for (auto &ip_config : ip_configs) {

    auto it = product_config.find(ip_config.interface_);
    if (it != product_config.end()) {
      required_ip_configs.push_back(ip_config);
    }

  }
  return required_ip_configs;

}

Status NetworkConfigBrain::ApplyConfig(BridgeConfig &product_config, const IPConfigs &ip_configs) const {

  Status status = bridge_manager_.ApplyBridgeConfiguration(product_config);

  if (status.Ok()) {
    status = ip_manager_.ApplyIpConfiguration(ip_configs);
  }

  return status;
}

Status NetworkConfigBrain::ApplyConfig(BridgeConfig &product_config, const IPConfigs &ip_configs,
                                       const DipSwitchIpConfig &dip_switch_ip_config) const {

  Status status = bridge_manager_.ApplyBridgeConfiguration(product_config);

  if (status.Ok()) {
    status = ip_manager_.ApplyIpConfiguration(ip_configs, dip_switch_ip_config);
  }

  return status;
}

void NetworkConfigBrain::ReplaceSystemToLabelInterfaces(BridgeConfig& config) const{

  auto convert_to_product_interfaces = [&](auto& bridge){
    Interfaces itfs = bridge.second;
    device_properties_provider_.ConvertOSToProductInterfaces(itfs);
    bridge.second = itfs;
  };

  ::std::for_each(config.begin(), config.end(), convert_to_product_interfaces);

}

Status NetworkConfigBrain::SetBridgeConfig(::std::string const &product_config_json) {

  BridgeConfig product_config;
  Status status = json_config_converter_.JsonToBridgeConfig(product_config_json, product_config);

  ReplaceSystemToLabelInterfaces(product_config);
  FilterEmptyBridges(product_config);

  if (status.Ok()) {
    BridgeConfig current_config;
    persistence_provider_.Read(current_config);

    if (IsEqual(product_config, current_config)) {
      LogDebug("New bridge configuration is equal to current. Do nothing.");
      return Status { StatusCode::OK };
    }
  }

  if (status.Ok()) {
    status = bridge_manager_.ApplyBridgeConfiguration(product_config);
  }

  Status persistence_status;
  if (status.Ok()) {
    persistence_status = persistence_provider_.Write(product_config);
  }

  if (status.Ok()) {
    event_manager_.NotifyNetworkChanges(EventType::SYSTEM, EventLayer::EVENT_FOLDER);
  }

  if (persistence_status.NotOk()) {
    status.Append(StatusCode::ERROR, "Failed to persist bridge configuration.");
  }

  if (status.NotOk()) {
    status.Prepend("Failed to set bridge config. ");
    LogError(status.GetMessage());
  }

  return status;
}

::std::string NetworkConfigBrain::GetBridgeConfig() const {

  BridgeConfig product_config = bridge_manager_.GetBridgeConfig();

  ::std::string json;
  Status status = json_config_converter_.BridgeConfigToJson(product_config, json);

  if (status.NotOk()) {
    status.Prepend("Failed to get bridge config. ");
    LogError(status.GetMessage());
  }

  return json;
}

::std::string NetworkConfigBrain::GetDeviceInterfaces() const {

  Interfaces interfaces = device_properties_provider_.GetProductInterfaces();

  return json_config_converter_.InterfacesToJson(interfaces);
}

Status NetworkConfigBrain::SetInterfaceConfig(const ::std::string &config) {
  InterfaceConfigs port_configs;
  auto status = json_config_converter_.InterfaceConfigFromJson(config, port_configs);
  if (status.Ok()) {
    status = interface_config_manager_.Configure(port_configs);
  }

  if (status.Ok()) {
    event_manager_.ProcessEvents();
  }

  if (status.NotOk()) {
    status.Prepend("Failed to configure interface(s). ");
    LogError(status.GetMessage());
  }

  return status;
}

::std::string NetworkConfigBrain::GetInterfaceConfig() const {
  auto port_configs = interface_config_manager_.GetPortConfigs();
  return json_config_converter_.InterfaceConfigToJson(port_configs);
}

::std::string NetworkConfigBrain::GetAllIPConfigs() const {
  Bridges bridges = bridge_manager_.GetBridges();
  IPConfigs configs = ip_manager_.GetIPConfigs(bridges);

  return json_config_converter_.IPConfigsToJson(configs);
}

::std::string NetworkConfigBrain::GetAllCurrentIPConfigs() const {
  Bridges bridges = bridge_manager_.GetBridges();
  IPConfigs configs = ip_manager_.GetCurrentIPConfigs(bridges);

  return json_config_converter_.IPConfigsToJson(configs);
}

::std::string NetworkConfigBrain::GetIPConfig(const ::std::string &config) const {

  IPConfigs ip_configs;
  Status status = json_config_converter_.JsonToIPConfigs(config, ip_configs);
  ::std::string json;

  if (status.Ok()) {
    Bridges bridges;
    IPConfigs configs;

    if (not ip_configs.empty()) {
      ::std::string interface = ip_configs.at(0).interface_;
      bridges.push_back(interface);
    }

    configs = ip_manager_.GetIPConfigs(bridges);
    json = json_config_converter_.IPConfigsToJson(configs);
  }

  if (status.NotOk()) {
    status.Prepend("Failed to get ip config.");
    LogError(status.GetMessage());
  }

  return json;
}

Status NetworkConfigBrain::SetAllIPConfigs(::std::string const &json_config) {
  IPConfigs ip_configs;
  Status status = json_config_converter_.JsonToIPConfigs(json_config, ip_configs);

  RemoveUnnecessaryIPParameter(ip_configs);

  if (status.Ok()) {
    status = ip_manager_.ApplyIpConfiguration(ip_configs);
  }

  if (status.Ok()) {
    event_manager_.ProcessEvents();
  }

  if(status.NotOk()){
    LogError(status.GetMessage());
  }

  return status;
}

Status NetworkConfigBrain::ApplyIpConfiguration(const IPConfigs &config) {
  DipSwitchIpConfig dip_switch_config;
  persistence_provider_.Read(dip_switch_config);

  return ip_manager_.ApplyIpConfiguration(config);
}

Status NetworkConfigBrain::SetIPConfig(::std::string const &config) {
  return SetAllIPConfigs(config);
}

uint32_t NetworkConfigBrain::GetBackupParamterCount() const {
  return persistence_provider_.GetBackupParameterCount();
}

Status NetworkConfigBrain::Backup(const ::std::string &file_path, const ::std::string &targetversion) const {
  Status status = persistence_provider_.Backup(file_path, targetversion);

  if (status.NotOk()) {
    LogError(status.GetMessage());
  }

  return status;
}

Status NetworkConfigBrain::Restore(const std::string &file_path) {

  BridgeConfig persisted_bridge_config;
  IPConfigs persisted_ip_configs;
  Status status = persistence_provider_.Read(persisted_bridge_config, persisted_ip_configs);

  BridgeConfig product_config;
  IPConfigs ip_configs;
  InterfaceConfigs interface_configs;
  DipSwitchIpConfig dip_switch_ip_config;
  if (status.Ok()) {
    status = persistence_provider_.Restore(file_path, product_config, ip_configs, interface_configs,
                                           dip_switch_ip_config);
  }

  if (status.Ok()) {
    interface_config_manager_.Configure(interface_configs);

    FilterEmptyBridges(product_config);
    RemoveUnnecessaryIPParameter(ip_configs);
    /* Clean out ip configs that are not available,
     * the restore data might contain unnecessary ip data from former netconfd releases. */
    CleanWithRespectToSystem(ip_configs, netdev_manager_.GetNetDevs(), itf_info_.GetBridgeAssignedInterfaces());
    status = ApplyConfig(product_config, ip_configs, dip_switch_ip_config);

    // Try rollback if status is not ok
    if (status.NotOk()) {
      status = ApplyConfig(persisted_bridge_config, persisted_ip_configs);
      if (status.NotOk()) {
        status.Prepend("Failed to rollback after restore error");
      }
    }
  }

  if (status.Ok()) {
    event_manager_.ProcessEvents();
  }

  if (status.NotOk()) {
    status.Prepend("Restore configurations. ");
    LogError(status.GetMessage());
  }

  return status;
}

void NetworkConfigBrain::TempFixIp() {
  ip_manager_.ApplyTempFixIpConfiguration(FIX_IP_CONFIG);
  event_manager_.ProcessEvents();
}

::std::string NetworkConfigBrain::GetDipSwitchConfig() const {
  DipSwitchIpConfig dip_switch_ip_config;
  Status status = persistence_provider_.Read(dip_switch_ip_config);

  auto dip_switch_config = DipSwitchConfig(dip_switch_ip_config, ip_dip_switch_.GetMode(), ip_dip_switch_.GetValue());

  return json_config_converter_.DipSwitchConfigToJson(dip_switch_config);
}

Status NetworkConfigBrain::SetDipSwitchConfig(const ::std::string &config) {
  if (ip_dip_switch_.GetMode() == DipSwitchMode::HW_NOT_AVAILABLE) {
    return Status { StatusCode::ERROR, "Device has no DIP switch for IP configuration." };
  }

  DipSwitchIpConfig dip_switch_ip_config;
  Status status = json_config_converter_.JsonToDipSwitchIPConfig(config, dip_switch_ip_config);

  if (status.Ok()) {
    status = ip_manager_.ApplyIpConfiguration(dip_switch_ip_config);
  }

  if (status.Ok()) {
    event_manager_.ProcessEvents();
  }

  return status;
}

} /* namespace netconfd */
