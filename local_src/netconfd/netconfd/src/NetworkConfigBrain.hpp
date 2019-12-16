// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include "IPersistenceProvider.hpp"
#include "IIPManager.hpp"
#include "IEventManager.hpp"
#include "IDevicePropertiesProvider.hpp"
#include "IJsonConfigConverter.hpp"
#include "IDipSwitch.hpp"
#include "IInterfaceEvent.hpp"
#include "InterfaceConfigManager.hpp"
#include "MappedChangeDetector.hpp"
#include "IControllable.hpp"
#include "IBridgeManager.hpp"

namespace netconfd {

class IInterfaceMonitor;
class IInterfaceInformation;
class ITaskQueue;

class NetworkConfigBrain : private IInterfaceEvent, public IControllable {
 public:
  NetworkConfigBrain(IBridgeManager& interface_manager,
                     IInterfaceInformation& itf_info, IIPManager& ip_manager,
                     IEventManager& event_manager,
                     IDevicePropertiesProvider& device_properties_provider,
                     IJsonConfigConverter& json_config_converter,
                     IPersistenceProvider& persistence_provider, IDipSwitch& ip_dip_switch,
                     IInterfaceMonitor& if_monitor, InterfaceConfigManager& interface_config_manager);

  virtual ~NetworkConfigBrain() = default;

  NetworkConfigBrain(const NetworkConfigBrain&) = delete;
  NetworkConfigBrain& operator=(const NetworkConfigBrain&) = delete;
  NetworkConfigBrain(const NetworkConfigBrain&&) = delete;
  NetworkConfigBrain& operator=(const NetworkConfigBrain&&) = delete;

  Status Start() override;

  Status SetBridgeConfig(::std::string const& product_config);
  ::std::string GetBridgeConfig() const;
  ::std::string GetDeviceInterfaces() const;
  Status SetInterfaceConfig(::std::string const& config);
  ::std::string GetInterfaceConfig() const;

  ::std::string GetAllIPConfigs() const;
  ::std::string GetIPConfig(const ::std::string& config) const;
  Status SetAllIPConfigs(const ::std::string& config);
  Status SetIPConfig(const ::std::string& config);

  uint32_t GetBackupParamterCount() const;
  Status Backup(const std::string& file_path) const;
  Status Restore(const std::string& file_path);

 private:
  void ModifyIpConfigByDipSwitch(IPConfigs& ip_configs,
                                 IDipSwitch& ip_dip_switch);
  Status ApplyConfig(const BridgeConfig& product_config,
                     const IPConfigs& ip_configs) const;
  Status ApplyFallbackConfig();
  Status StopDynamicIPClientsOfBridgesToBeRemoved(
      const BridgeConfig& product_config) const;
  Status FilterRequiredBridgeConfigs(BridgeConfig& bridge_config) const;
  IPConfigs FilterRequiredIPConfigs(const IPConfigs& ip_configs,
                                    const BridgeConfig& product_config) const;
  bool HasToBePersisted(const IPConfig& ip_config) const;

  void LinkChange(::std::uint32_t if_index, eth::InterfaceLinkState new_state) override;
  void LogLinkChanges(Interface os_interface, eth::InterfaceLinkState new_state);
  void LinkChangeAsync(const std::__cxx11::string& ifName, eth::InterfaceLinkState new_state);

  IBridgeManager& bridge_manager_;
  IInterfaceInformation& itf_info_;
  IInterfaceMonitor& if_monitor_;
  InterfaceConfigManager& interface_config_manager_;
  IIPManager& ip_manager_;
  IEventManager& event_manager_;
  IDevicePropertiesProvider& device_properties_provider_;
  IJsonConfigConverter& json_config_converter_;
  IPersistenceProvider& persistence_provider_;
  IDipSwitch & ip_dip_switch_;
  MappedChangeDetector<Bridge, bool> bridge_changes_;
  MappedChangeDetector<Interface, eth::InterfaceLinkState> interface_link_changes_;
};

} /* namespace netconfd */
