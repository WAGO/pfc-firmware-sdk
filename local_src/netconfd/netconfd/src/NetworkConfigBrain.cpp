// SPDX-License-Identifier: GPL-2.0-or-later

#include "NetworkConfigBrain.hpp"

#include <string>
#include <boost/algorithm/string.hpp>
#include <boost/format.hpp>


#include "Logger.hpp"
#include "Helper.hpp"
#include "IpAddressManipulator.hpp"
#include "IInterfaceMonitor.hpp"
#include "EthernetInterface.hpp"
#include "InterfaceConfigManager.hpp"
#include "NetDevManager.hpp"
#include "IpConfigHelper.hpp"
#include "IBridgeInformation.hpp"
#include "TypesHelper.hpp"
#include "JsonConverter.hpp"
#include "LinkMode.hpp"

namespace netconf {

using namespace ::std::literals;

NetworkConfigBrain::NetworkConfigBrain(IBridgeManager &interface_manager, IBridgeInformation &itf_info,
                                       IIPManager &ip_manager, IEventManager &event_manager,
                                       IDeviceProperties &device_properties_provider,
                                       IPersistenceProvider &persistence_provider, IDipSwitch &ip_dip_switch,
                                       InterfaceConfigManager &interface_config_manager, INetDevManager &netdev_manager)

    : bridge_manager_ { interface_manager },
      bridge_information_ { itf_info },
      interface_config_manager_ { interface_config_manager },
      ip_manager_ { ip_manager },
      event_manager_ { event_manager },
      device_properties_provider_ { device_properties_provider },
      persistence_provider_ { persistence_provider },
      netdev_manager_ { netdev_manager },
      ip_dip_switch_ { ip_dip_switch } {
}

void NetworkConfigBrain::Start(StartWithPortstate startwithportstate) {
  Status statusBridgeConfig;
  Status statusIPConfig;
  Status statusDIPSwitchIPConfig;
  Status status;

  BridgeConfig bridge_config;
  IPConfigs ip_configs;
  DipSwitchIpConfig dip_switch_ip_config;

  Interfaces interfaces = device_properties_provider_.GetProductPortNames();
  if (interfaces.empty()) {
    LogError("Start: Failed to determine available product interfaces.");
    return;
  }

  statusBridgeConfig = persistence_provider_.Read(bridge_config);
  statusIPConfig = persistence_provider_.Read(ip_configs);
  statusDIPSwitchIPConfig = persistence_provider_.Read(dip_switch_ip_config);

  if (statusBridgeConfig.IsOk()) {
    RemoveUnsupportedInterfaces(bridge_config, interfaces);
    RemoveEmptyBridges(bridge_config);
  } else {
    ResetBridgeConfigToDefault(interfaces, bridge_config);
    LogWarning("Start: " + statusBridgeConfig.ToString());
  }

  if (statusIPConfig.IsNotOk()) {
    LogDebug("Start: Reset IP configuration.");
    ResetIpConfigsToDefault(ip_configs);
  }

  FilterRequiredIpConfigs(ip_configs, bridge_config);
  RemoveUnnecessaryIPParameter(ip_configs);

  if (ip_configs.empty()) {
    LogWarning("Start: No IP config left on startup!");
  }

  status = ip_manager_.ValidateIPConfigs(ip_configs);
  if (status.IsNotOk()) {
    LogWarning("Start: IP validation failed. Reset IP configuration.");
    ResetIpConfigsToDefault(ip_configs);
    status = Status { };
  }

  // At this point bridge and IP configs should be valid.

  if (statusDIPSwitchIPConfig.IsNotOk()) {
    LogDebug("Start: Reset DIP switch IP configuration.");
    ResetDIPSwitchIPConfigToDefault(dip_switch_ip_config);
  }

  persistence_provider_.Write(bridge_config);
  persistence_provider_.Write(ip_configs);
  persistence_provider_.Write(dip_switch_ip_config);

  if (status.IsOk()) {
    status = ApplyConfig(bridge_config, ip_configs);
  }

  interface_config_manager_.InitializePorts(static_cast<InterfaceState>(startwithportstate));



  if (status.IsOk()) {
    event_manager_.ProcessEvents();
  }

  if (status.IsNotOk()) {
    LogError("Start: " + status.ToString());
  }
}

void NetworkConfigBrain::GetValidIpConfigsSubset(const IPConfigs &configs, IPConfigs &subset) {
  subset.clear();
  for (const auto &config : configs) {
    subset.push_back(config);
    Status status = ip_manager_.ValidateIPConfigs(subset);
    if (status.IsNotOk()) {
      // Remove element that leads to an invalid configuration.
      subset.pop_back();
    }
  }
}

void NetworkConfigBrain::ResetBridgeConfigToDefault(Interfaces product_interfaces, BridgeConfig &config) {
  config = { { "br0", product_interfaces } };
}

void NetworkConfigBrain::ResetIpConfigsToDefault(IPConfigs &configs) {
  configs = { { "br0", IPSource::DHCP, ZeroIP, ZeroIP } };
}

void NetworkConfigBrain::ResetDIPSwitchIPConfigToDefault(DipSwitchIpConfig &config) {
  config = { "192.168.1.0", "255.255.255.0" };
}

void NetworkConfigBrain::RemoveEmptyBridges(BridgeConfig &config) const {
  auto it = config.cbegin();
  while (it != config.cend()) {
    if (it->second.empty()) {
      it = config.erase(it);
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

void NetworkConfigBrain::RemoveUnsupportedInterfaces(BridgeConfig &config, Interfaces &supported_interfaces) const {
  for (auto &bridge_entry : config) {
    Interfaces unsupported_interfaces;

    for (auto &interface : bridge_entry.second) {
      if (IsNotIncluded(interface, supported_interfaces)) {
        unsupported_interfaces.push_back(interface);
      }
    }

    for (auto &interface : unsupported_interfaces) {
      RemoveByValue(interface, bridge_entry.second);
    }
  }
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

  if (status.IsOk()) {
    status = ip_manager_.ApplyIpConfiguration(ip_configs);
  }

  return status;
}

void NetworkConfigBrain::ReplaceSystemToLabelInterfaces(BridgeConfig &config) const {

  auto convert_to_product_interfaces = [&](auto &bridge) {
    Interfaces itfs = bridge.second;
    device_properties_provider_.ConvertOSToProductInterfaces(itfs);
    bridge.second = itfs;
  };

  ::std::for_each(config.begin(), config.end(), convert_to_product_interfaces);

}

::std::string NetworkConfigBrain::SetBridgeConfig(::std::string const &product_config_json) {

  BridgeConfig product_config;

  Status status = jc.FromJsonString(product_config_json, product_config);

  ReplaceSystemToLabelInterfaces(product_config);
  RemoveEmptyBridges(product_config);

  if (status.IsOk()) {
    BridgeConfig current_config;
    persistence_provider_.Read(current_config);

    if (IsEqual(product_config, current_config)) {
      LogDebug("New bridge configuration is equal to current. Do nothing.");
      return jc.ToJsonString(Status::Ok());
    }
  }

  if (status.IsOk()) {
    status = bridge_manager_.ApplyBridgeConfiguration(product_config);
  }

  Status persistence_status;
  if (status.IsOk()) {
    persistence_status = persistence_provider_.Write(product_config);
  }

  if (status.IsOk()) {
    event_manager_.NotifyNetworkChanges(EventType::SYSTEM, EventLayer::EVENT_FOLDER);
  }

  if (persistence_status.IsNotOk()) {
    status.Set(StatusCode::PERSISTENCE_WRITE);
  }

  if (status.IsNotOk()) {
    LogError("Set Bridge Config: " + status.ToString());
  }

  return jc.ToJsonString(status);
}

::std::string NetworkConfigBrain::GetBridgeConfig(::std::string &data) const {

  BridgeConfig product_config = bridge_manager_.GetBridgeConfig();

  data = jc.ToJsonString(product_config);

  return jc.ToJsonString(Status::Ok());
}

::std::string NetworkConfigBrain::GetInterfaceInformation(::std::string &data) const {

  auto netdevs = netdev_manager_.GetNetDevs();
  InterfaceInformations iis;
  std::transform(netdevs.begin(), netdevs.end(), ::std::back_inserter(iis), [&](const auto &netdev_ptr) {
       return interface_config_manager_.GetInterfaceInformation(*netdev_ptr);
     });

  data = jc.ToJsonString(iis);

  return jc.ToJsonString(Status::Ok());

}

::std::string NetworkConfigBrain::SetInterfaceConfig(const ::std::string &config) {
  InterfaceConfigs port_configs;

  auto status = jc.FromJsonString(config, port_configs);

  if (status.IsOk()) {
    status = interface_config_manager_.Configure(port_configs);
  }

  if (status.IsOk()) {
    event_manager_.ProcessEvents();
  }

  if (status.IsNotOk()) {
    LogError("SetInterfaceConfig: " + status.ToString());
  }

  return jc.ToJsonString(status);
}

::std::string NetworkConfigBrain::GetInterfaceConfig(::std::string &config) const {
  auto port_configs = interface_config_manager_.GetPortConfigs();

  config = jc.ToJsonString(port_configs);

  return jc.ToJsonString(Status::Ok());
}

::std::string NetworkConfigBrain::GetInterfaceStatuses(::std::string &config) const {
  auto itf_statuses = interface_config_manager_.GetCurrentPortStatuses();

  config = jc.ToJsonString(itf_statuses);

  return jc.ToJsonString(Status::Ok());
}

::std::string NetworkConfigBrain::GetAllIPConfigs(::std::string &config) const {
  IPConfigs configs = ip_manager_.GetIPConfigs();

  config = jc.ToJsonString(configs);

  return jc.ToJsonString(Status::Ok());
}

::std::string NetworkConfigBrain::GetCurrentIPConfigs(::std::string &data) const {
  IPConfigs configs = ip_manager_.GetCurrentIPConfigs();

  data = jc.ToJsonString(configs);
  return jc.ToJsonString(Status::Ok());
}

::std::string NetworkConfigBrain::GetIPConfig(const ::std::string &config, ::std::string &data) const {

  IPConfigs ip_configs;

  Status status = jc.FromJsonString(config, ip_configs);

  if (status.IsOk()) {
    Bridges bridges;
    IPConfigs configs;

    if (not ip_configs.empty()) {
      ::std::string interface = ip_configs.at(0).interface_;
      bridges.push_back(interface);
    }

    configs = ip_manager_.GetIPConfigs(bridges);
    data = jc.ToJsonString(configs);
  }

  if (status.IsNotOk()) {
    LogError("GetIPConfig: " + status.ToString());
  }

  return jc.ToJsonString(status);
}

::std::string NetworkConfigBrain::SetAllIPConfigs(::std::string const &json_config) {
  IPConfigs ip_configs;

  Status status = jc.FromJsonString(json_config, ip_configs);

  RemoveUnnecessaryIPParameter(ip_configs);

  if (status.IsOk()) {
    status = ip_manager_.ApplyIpConfiguration(ip_configs);
  }

  if (status.IsOk()) {
    event_manager_.ProcessEvents();
  }

  if (status.IsNotOk()) {
    LogError("SetAllIPConfigs: " + status.ToString());
  }

  return jc.ToJsonString(status);
}

Status NetworkConfigBrain::ApplyIpConfiguration(const IPConfigs &config) {
  DipSwitchIpConfig dip_switch_config;
  persistence_provider_.Read(dip_switch_config);

  return ip_manager_.ApplyIpConfiguration(config);
}

::std::string NetworkConfigBrain::SetIPConfig(::std::string const &config) {
  return SetAllIPConfigs(config);
}

::std::string NetworkConfigBrain::GetBackupParamterCount() const {
  return ::std::to_string(persistence_provider_.GetBackupParameterCount());
}

::std::string NetworkConfigBrain::Backup(const ::std::string &file_path, const ::std::string &targetversion) const {
  Status status = persistence_provider_.Backup(file_path, targetversion);

  if (status.IsNotOk()) {
    LogError("Backup: " + status.ToString());
  }

  return jc.ToJsonString(status);
}

::std::string NetworkConfigBrain::Restore(const std::string &file_path) {

  BridgeConfig bridge_config;
  IPConfigs ip_configs;
  InterfaceConfigs interface_configs;
  DipSwitchIpConfig dip_switch_ip_config;

  BridgeConfig persisted_bridge_config;
  IPConfigs persisted_ip_configs;
  Status status = persistence_provider_.Read(persisted_bridge_config, persisted_ip_configs);
  if (status.IsOk()) {
    // TODO (PND): Cleanup the backup data before restoring it, might contain data that is not applicable!!!
    status = persistence_provider_.Restore(file_path, bridge_config, ip_configs, interface_configs,
                                          dip_switch_ip_config);
  }

  if (status.IsOk()) {
    interface_config_manager_.Configure(interface_configs);

    RemoveEmptyBridges(bridge_config);
    RemoveUnnecessaryIPParameter(ip_configs);
    status = bridge_manager_.ApplyBridgeConfiguration(bridge_config);
    if (status.IsNotOk()) {
      LogError("Failed to restore bridge configuration from file:"s + file_path);
    }

    ip_manager_.ClearIpConfiguration();
    /* Clean out ip configs that are not available,
     * the restore data might contain unnecessary ip data from former netconfd releases. */
    CleanWithRespectToSystem(ip_configs, netdev_manager_.GetNetDevs(),
                             bridge_information_.GetBridgeAssignedInterfaces());
    status = ip_manager_.ApplyIpConfiguration(ip_configs, dip_switch_ip_config);
    if (status.IsNotOk()) {
      LogError("Failed to restore ip configuration configuration from file:"s + file_path);
    }

    // Try rollback if status is not ok
    if (status.IsNotOk()) {
      status = ApplyConfig(persisted_bridge_config, persisted_ip_configs);
      if (status.IsNotOk()) {
        LogError("Failed to rollback after restore error: " + status.ToString());
      }
    }
  }

  if (status.IsOk()) {
    event_manager_.ProcessEvents();
  }

  if (status.IsNotOk()) {
    LogError("Restore configurations: " + status.ToString());
  }

  return jc.ToJsonString(status);
}

::std::string NetworkConfigBrain::TempFixIp() {
  auto status = ip_manager_.ApplyTempFixIpConfiguration();

  if (status.IsOk()) {
    event_manager_.ProcessEvents();
  } else {
    LogError("Set temp fix IP: " + status.ToString());
  }
  return jc.ToJsonString(status);
}

::std::string NetworkConfigBrain::GetDipSwitchConfig(::std::string &data) const {
  DipSwitchIpConfig dip_switch_ip_config;
  Status status = persistence_provider_.Read(dip_switch_ip_config);

  auto dip_switch_config = DipSwitchConfig(dip_switch_ip_config, ip_dip_switch_.GetMode(), ip_dip_switch_.GetValue());

  data = jc.ToJsonString(dip_switch_config);

  return jc.ToJsonString(Status::Ok());
}

::std::string NetworkConfigBrain::SetDipSwitchConfig(const ::std::string &config) {
  if (ip_dip_switch_.GetMode() == DipSwitchMode::HW_NOT_AVAILABLE) {
    return jc.ToJsonString(Status { StatusCode::DIP_NOT_AVAILABLE });
  }

  DipSwitchIpConfig dip_switch_ip_config;

  auto status = jc.FromJsonString(config, dip_switch_ip_config);

  if (status.IsOk()) {
    status = ip_manager_.ApplyIpConfiguration(dip_switch_ip_config);
  }

  if (status.IsOk()) {
    event_manager_.ProcessEvents();
  }

  if (status.IsNotOk()) {
    LogError("DIP Switch: " + status.ToString());
  }

  return jc.ToJsonString(status);
}

} /* namespace netconf */
