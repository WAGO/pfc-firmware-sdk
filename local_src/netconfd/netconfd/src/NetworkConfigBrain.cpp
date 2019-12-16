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
#include <boost/format.hpp>
#include <string>

namespace netconfd {

using namespace ::std::literals;

const auto os_interface_prefix = "ethX";

NetworkConfigBrain::NetworkConfigBrain(IBridgeManager& interface_manager, IInterfaceInformation& itf_info,
    IIPManager& ip_manager, IEventManager& event_manager,
    IDevicePropertiesProvider& device_properties_provider,
    IJsonConfigConverter& json_config_converter,
    IPersistenceProvider& persistence_provider, IDipSwitch& ip_dip_switch,
                                       IInterfaceMonitor& if_monitor, InterfaceConfigManager& interface_config_manager)

    : bridge_manager_ { interface_manager },
      itf_info_ { itf_info },
      if_monitor_ { if_monitor },
      interface_config_manager_ { interface_config_manager },
      ip_manager_ { ip_manager },
      event_manager_ { event_manager },
      device_properties_provider_ { device_properties_provider },
      json_config_converter_ { json_config_converter },
      persistence_provider_ { persistence_provider },
      ip_dip_switch_ { ip_dip_switch } {
}

Status NetworkConfigBrain::Start() {

  LogInfo("Loading inital configuration");

  BridgeConfig bridge_config;
  IPConfigs ip_configs;
  Status status = persistence_provider_.Read(bridge_config, ip_configs);

  if (status.Ok()) {
    status = FilterRequiredBridgeConfigs(bridge_config);
    if (status.NotOk()) {
      status.Prepend("Failed to adjust bridge configuration to device specific interfaces. ");
    }
  }

  ModifyIpConfigByDipSwitch(ip_configs, ip_dip_switch_);

  if (status.Ok()) {
    status = ip_manager_.ValidateIPConfigs(ip_configs, true);
    if (status.NotOk()) {
      LogWarning("Invalid IP-configuration in persistence file.");
    }
  }

  if (status.Ok()) {
    status = ApplyConfig(bridge_config, ip_configs);
    if (status.NotOk()) {
      status.Prepend("Failed to apply bridge configuration from persistence file. ");
    }
  }

  if (status.NotOk()) {
    status = ApplyFallbackConfig();
    if(status.Ok()){
      LogInfo("Apply fallback configuration: All interfaces are switched and DHCP is started.");
    }
  }

  if (status.Ok()) {
    if_monitor_.RegisterEventHandler(*this);
  }

  if (status.NotOk()) {
    LogError(status.GetMessage());
  }
  return status;
}

void NetworkConfigBrain::ModifyIpConfigByDipSwitch(IPConfigs& ip_configs, IDipSwitch& ip_dip_switch) {
  if (ip_dip_switch.GetValue() > 0) {
    LogDebug("Applying DipSwitch");
    auto it = ::std::find_if(ip_configs.begin(), ip_configs.end(),
        [](const IPConfig& cfg) {return cfg.interface_ == "br0";});
    if (it != ip_configs.end() && it->source_ != IPSource::TEMPORARY) {
      if (ip_dip_switch.HasReachedMaxValue()) {
        it->source_ = IPSource::DHCP;
      } else {
        it->source_ = IPSource::STATIC;
        it->address_ = IpAddressManipulator::ChangeHost(*it, ip_dip_switch.GetValue()).address_;
      }
    }
  }
}

Status NetworkConfigBrain::FilterRequiredBridgeConfigs(BridgeConfig& bridge_config) const {

  Status status;

  Interfaces supported_interfaces = device_properties_provider_.GetProductInterfaces();
  if (supported_interfaces.empty()) {
    status.Prepend(StatusCode::ERROR, " No device specific interfaces found.");
  }

  for (auto& bridge : bridge_config) {

    Interfaces interfaces_to_remove;

    for (auto& bridge_interface : bridge.second) {
      if (IsNotIncluded(bridge_interface, supported_interfaces)) {
        interfaces_to_remove.push_back(bridge_interface);
      }
    }

    for (auto& interface : interfaces_to_remove) {
      RemoveByValue(interface, bridge.second);
    }
  }

  return status;
}

IPConfigs NetworkConfigBrain::FilterRequiredIPConfigs(const IPConfigs& ip_configs,
                                                      const BridgeConfig& product_config) const {

  IPConfigs required_ip_configs;
  for (auto& ip_config : ip_configs) {

    auto it = product_config.find(ip_config.interface_);
    if (it != product_config.end()) {
      required_ip_configs.push_back(ip_config);
    }

  }
  return required_ip_configs;

}

Status NetworkConfigBrain::ApplyFallbackConfig() {
  Status status;

  //In case of fallback, all interfaces are assigned to br0 and DHCP is started.

  Interfaces interfaces = device_properties_provider_.GetProductInterfaces();
  if (interfaces.empty()) {
    status.Prepend(StatusCode::ERROR, "Apply fallback configuration failed. Netconfd cannot get available interfaces.");
  }

  BridgeConfig fallback_bridge_config = { { "br0", interfaces }, { "br1", { } } };
  IPConfigs fallback_ip_config = { { "br0", IPSource::DHCP, "192.168.1.17","255.255.255.0", "192.168.1.255" } , { "br1", IPSource::STATIC, "192.168.2.17","255.255.255.0", "192.168.2.255" } };

  ModifyIpConfigByDipSwitch(fallback_ip_config, ip_dip_switch_);

  Status apply_status = ApplyConfig(fallback_bridge_config, fallback_ip_config);
  if (apply_status.NotOk()) {
    status.Prepend(StatusCode::ERROR,
        "Apply fallback configuration failed. Netconfd cannot set a valid configuration.");
  }

  if (apply_status.Ok()) {
    persistence_provider_.Write(fallback_bridge_config);
    persistence_provider_.Write(fallback_ip_config);
  }
  return status;
}

Status NetworkConfigBrain::StopDynamicIPClientsOfBridgesToBeRemoved(const BridgeConfig& product_config) const {

  Bridges actual_bridges = bridge_manager_.GetBridges();

  IPConfigs ip_configs;
  for (auto& actual_bridge : actual_bridges) {
    if (KeyIsNotIncluded(actual_bridge, product_config)) {
      ip_configs.push_back(IPConfig(actual_bridge, IPSource::NONE));
    }
  }

  return ip_manager_.Configure(ip_configs);
}

Status NetworkConfigBrain::ApplyConfig(const BridgeConfig& product_config, const IPConfigs& ip_configs) const {

  Status status = bridge_manager_.IsValid(product_config);

  if (status.Ok()) {
    status = StopDynamicIPClientsOfBridgesToBeRemoved(product_config);
  }

  if (status.Ok()) {
    status = bridge_manager_.Configure(product_config);
  }

  if (status.Ok()) {
    IPConfigs required_ip_configs = FilterRequiredIPConfigs(ip_configs, product_config);

    status = ip_manager_.Configure(required_ip_configs);

    //set up only bridges to which interfaces have been assigned.
    //TODO (PT35) necessary as long as empty bridges are supported.
    for (auto& config_pair : product_config) {
      Status status_interface;
      if (config_pair.second.empty()) {
        status_interface = bridge_manager_.SetBridgeDown(config_pair.first);

        IPConfigs disable_interface_config = { { config_pair.first, IPSource::NONE } };
        ip_manager_.Configure(disable_interface_config);
      } else {
        status_interface = bridge_manager_.SetBridgeUp(config_pair.first);
      }
      if (status.Ok()) {
        status = status_interface;
      }
    }

    for (auto& ip_config : required_ip_configs) {
      event_manager_.NotifyIpChange(ip_config.interface_);
    }
  }

  return status;
}

Status NetworkConfigBrain::SetBridgeConfig(::std::string const& product_config_json) {

  BridgeConfig product_config;
  Status status = json_config_converter_.JsonToBridgeConfig(product_config_json, product_config);

  if(status.Ok())
  {
    BridgeConfig current_config;
    persistence_provider_.Read(current_config);

    if(IsEqual(product_config, current_config))
    {
      LogDebug("New bridge configuration is equal to current. Do nothing.");
      return Status { StatusCode::OK };
    }
  }

  bridge_changes_.Clear();

  IPConfigs ip_configs;
  if (status.Ok()) {
    status = persistence_provider_.Read(ip_configs);
  }

  ModifyIpConfigByDipSwitch(ip_configs, ip_dip_switch_);

  if (status.Ok()) {
    status = ApplyConfig(product_config, ip_configs);
  }

  if (status.Ok()) {
    event_manager_.NotifyNetworkChanges();
  }

  if (status.Ok()) {
    status = persistence_provider_.Write(product_config);
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

Status NetworkConfigBrain::SetInterfaceConfig(const ::std::string& config) {
  InterfaceConfigs port_configs;
  auto status = json_config_converter_.InterfaceConfigFromJson(config, port_configs);
  if (status.Ok()) {
    status = interface_config_manager_.Configure(port_configs);
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

::std::string NetworkConfigBrain::GetIPConfig(const ::std::string& config) const {

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

bool NetworkConfigBrain::HasToBePersisted(const IPConfig& ip_config) const {

  if (ip_config.interface_ == "wwan0") {
    return false;
  }

  return (ip_config.source_ != IPSource::TEMPORARY && ip_config.source_ != IPSource::UNKNOWN);
}

Status NetworkConfigBrain::SetAllIPConfigs(::std::string const& config) {

  IPConfigs ip_configs;
  Status status = json_config_converter_.JsonToIPConfigs(config, ip_configs);

  ComplementNetmasks(ip_configs);
  // Hint: If a DIP switch is present and used to set the current ip, the
  //       ip configurations for system and persistence might be different.

  auto dip_switch_modified_ip_configs = ip_configs;
  // Validate system ip configs.
  ModifyIpConfigByDipSwitch(dip_switch_modified_ip_configs, ip_dip_switch_);

  status = ip_manager_.ValidateIPConfigs(dip_switch_modified_ip_configs, true);

  bool is_applicable_to_system = false;
  if (status.Ok()) {
    is_applicable_to_system = ip_manager_.IsApplicableToSystem(dip_switch_modified_ip_configs);
  }

  if (is_applicable_to_system) {
    status = ip_manager_.ValidateIPConfigIsApplicableToSystem(dip_switch_modified_ip_configs);

    // Apply ip config to system.
    if (status.Ok()) {
      status = ip_manager_.Configure(dip_switch_modified_ip_configs);
    }

    if (status.Ok()) {
      event_manager_.NotifyNetworkChanges();
      for (auto& ip_config : dip_switch_modified_ip_configs) {
        event_manager_.NotifyIpChange(ip_config.interface_);
      }
    }
  }

  if(status.NotOk()) {
    return status;
  }

  // PERSISTENCE

  // 1. validate IPConfig

  // Validate ip configs to persist.
  Status persist_status;
  IPConfigs persisted_ip_configs;
  persist_status = persistence_provider_.Read(persisted_ip_configs);

  auto new_ip_configs_to_persist = persisted_ip_configs;
  if (persist_status.Ok()) {
    Bridges persisted_bridges;
    std::transform(persisted_ip_configs.begin(), persisted_ip_configs.end(), std::back_inserter(persisted_bridges),
                   [](IPConfig c) -> Bridge {return c.interface_;});
    for (auto& ip_config : ip_configs) {
      if (HasToBePersisted(ip_config)) {
        AddIPConfig(ip_config, new_ip_configs_to_persist);
      }
    }
    persist_status = ip_manager_.ValidateIPConfigs(new_ip_configs_to_persist, true);
  }

  // Apply ip config to persistence.
  if (persist_status.Ok()) {
    persist_status = persistence_provider_.Write(new_ip_configs_to_persist);
  }

  if (!persist_status.Ok()) {
    persist_status.Prepend("Failed to persist ip config. ");

    if (status.Ok()) {
      status = persist_status;
    } else {
      status.Append(persist_status.GetMessage());
    }
  }

  if (status.NotOk()) {
    LogError(status.GetMessage());
  }

  return status;
}

Status NetworkConfigBrain::SetIPConfig(::std::string const& config) {

  return SetAllIPConfigs(config);
}

uint32_t NetworkConfigBrain::GetBackupParamterCount() const {
  return persistence_provider_.GetBackupParameterCount();
}

Status NetworkConfigBrain::Backup(const std::string& file_path) const {

  Status status = persistence_provider_.Backup(file_path);

  if (status.NotOk()) {
    LogError(status.GetMessage());
  }

  return status;
}

Status NetworkConfigBrain::Restore(const std::string& file_path) {

  BridgeConfig persisted_bridge_config;
  IPConfigs persisted_ip_configs;
  Status status = persistence_provider_.Read(persisted_bridge_config, persisted_ip_configs);
  BridgeConfig product_config;
  IPConfigs ip_configs;
  InterfaceConfigs interface_configs;
  if (status.Ok()) {
    status = persistence_provider_.Restore(file_path, product_config, ip_configs, interface_configs);
  }

  if (status.Ok()) {
    interface_config_manager_.Configure(interface_configs);

    status = ApplyConfig(product_config, ip_configs);

    // Try rollback if status is not ok
    if (status.NotOk()) {
      status = ApplyConfig(persisted_bridge_config, persisted_ip_configs);
      if (status.NotOk()) {
        status.Prepend("Failed to rollback after restore error");
      }
    }
  }

  if (status.NotOk()) {
    status.Prepend("Restore configurations. ");
    LogError(status.GetMessage());
  }

  return status;
}

void NetworkConfigBrain::LinkChangeAsync(const std::__cxx11::string& ifName,
    __attribute((unused))                          eth::InterfaceLinkState new_state) {

  auto involvedBridge = itf_info_.IsBridge(ifName) ? Bridge { ifName } : itf_info_.GetBridgeOfInterface(ifName);
  if (!involvedBridge.empty()) {
    auto any_if_up = itf_info_.HasAnyInterfacesUp(involvedBridge);
    if (bridge_changes_.Change(involvedBridge, any_if_up)) {
      auto logString = (boost::format("LinkChange: Reapply IP config on %1% after link change of %2%") % involvedBridge
          % ifName).str();
      LogDebug(logString);
      IPConfigs reapplied_configs;
      auto status = persistence_provider_.Read(reapplied_configs);
      if (status.Ok()) {

        IpConfigsIntersection(reapplied_configs, bridge_manager_.GetBridges());

        ModifyIpConfigByDipSwitch(reapplied_configs, ip_dip_switch_);

        status = ip_manager_.Configure(reapplied_configs);

        event_manager_.NotifyIpChange(involvedBridge);
      }
      if (status.NotOk()) {
        LogError("LinkChangeAsync: "s.append(status.GetMessage()));
      }
    }
  }
}

void NetworkConfigBrain::LinkChange(::std::uint32_t if_index,
    eth::InterfaceLinkState __attribute__((unused)) new_state) {
  auto ifName = EthernetInterface::IndexToName(if_index);
  if (ifName.find(os_interface_prefix) != ::std::string::npos || if_monitor_.GetAddressFamily(if_index) == PF_BRIDGE) {

    auto isLinkUp = itf_info_.IsInterfaceUp(ifName) ? eth::InterfaceLinkState::Up : eth::InterfaceLinkState::Down;

    if (interface_link_changes_.Change(ifName, isLinkUp)) {
      LogLinkChanges(ifName, isLinkUp);
      LinkChangeAsync(ifName, isLinkUp);
    }
  }
}

void NetworkConfigBrain::LogLinkChanges(Interface os_interface, eth::InterfaceLinkState new_state) {
  auto logString = (boost::format("Link of interface %1% is %2%") % os_interface
      % (new_state == eth::InterfaceLinkState::Up ? "'up'" : "'down'")).str();
  LogInfo(logString);

}

} /* namespace netconfd */
