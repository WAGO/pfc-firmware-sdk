// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include "IDeviceProperties.hpp"
#include "IPersistenceProvider.hpp"
#include "IEventManager.hpp"
#include "IDipSwitch.hpp"
#include "InterfaceConfigManager.hpp"
#include "MappedChangeDetector.hpp"
#include "IControllable.hpp"
#include "IBridgeManager.hpp"
#include "IIPManager.hpp"
#include "JsonConverter.hpp"


namespace netconf {

class IInterfaceMonitor;
class IInterfaceInformation;
class ITaskQueue;
class INetDevManager;

class NetworkConfigBrain : public IControllable{
 public:
  NetworkConfigBrain(IBridgeManager& interface_manager,
                     IBridgeInformation& itf_info, IIPManager& ip_manager,
                     IEventManager& event_manager,
                     IDeviceProperties& device_properties_provider,
                     IPersistenceProvider& persistence_provider, IDipSwitch& ip_dip_switch,
                     InterfaceConfigManager& interface_config_manager,
                     INetDevManager& netdev_manager);

  virtual ~NetworkConfigBrain() = default;

  NetworkConfigBrain(const NetworkConfigBrain&) = delete;
  NetworkConfigBrain& operator=(const NetworkConfigBrain&) = delete;
  NetworkConfigBrain(const NetworkConfigBrain&&) = delete;
  NetworkConfigBrain& operator=(const NetworkConfigBrain&&) = delete;

  void Start() override;

  ::std::string SetBridgeConfig(::std::string const& product_config);
  ::std::string GetBridgeConfig(::std::string&) const;
  ::std::string GetInterfaceInformation(::std::string&) const;
  ::std::string SetInterfaceConfig(::std::string const& config);
  ::std::string GetInterfaceConfig(::std::string&) const;

  ::std::string GetDipSwitchConfig(::std::string&) const;
  ::std::string GetAllIPConfigs(::std::string&) const;
  ::std::string GetIPConfig(const ::std::string& config, ::std::string&) const;
  ::std::string GetAllCurrentIPConfigs(::std::string&) const;
  ::std::string SetDipSwitchConfig(const ::std::string&);
  ::std::string SetAllIPConfigs(const ::std::string&);
  ::std::string SetIPConfig(const ::std::string&);

  ::std::string GetBackupParamterCount() const;
  ::std::string Backup(const ::std::string& file_path, const ::std::string& targetversion) const;
  ::std::string Restore(const std::string& file_path);

  ::std::string TempFixIp();

 private:
  bool HasToApplyDipSwitchConfig() const;
  void ModifyIpConfigByDipSwitch(IPConfigs& ip_configs);
  void ModifyIpConfigByDipSwitch(IPConfigs& ip_configs, const DipSwitchIpConfig& dip_switch_ip_config);
  Error ApplyConfig(BridgeConfig& product_config, const IPConfigs& ip_configs) const;
  Error ApplyFallbackConfig();
  Error StopDynamicIPClientsOfBridgesToBeRemoved(const BridgeConfig &product_config) const;
  void RemoveEmptyBridges(BridgeConfig& config) const;
  void RemoveUnsupportedInterfaces(BridgeConfig& config, Interfaces &supported_interfaces) const;
  void FilterRequiredIpConfigs(IPConfigs &ip_configs, const BridgeConfig &bridge_config) const;
  IPConfigs FilterRequiredIPConfigs(const IPConfigs &ip_configs, const BridgeConfig &product_config) const;
  bool HasToBePersisted(const IPConfig& ip_config) const;

  void ReplaceSystemToLabelInterfaces(BridgeConfig& config) const;

  void ResetBridgeConfigToDefault(Interfaces product_interfaces, BridgeConfig& config);
  void ResetIpConfigsToDefault(IPConfigs& configs);
  void ResetDIPSwitchIPConfigToDefault(DipSwitchIpConfig& config);

  void GetValidIpConfigsSubset(const IPConfigs& configs, IPConfigs& subset);

  Error ApplyIpConfiguration(const IPConfigs& config);
  Error ApplyIpConfiguration(const IPConfigs& config, const DipSwitchIpConfig& dip_switch_config);
  Error PersistIpConfiguration(const IPConfigs& config);
  JsonConverter jc;
  IBridgeManager& bridge_manager_;
  IBridgeInformation& bridge_information_;
  InterfaceConfigManager& interface_config_manager_;
  IIPManager& ip_manager_;
  IEventManager& event_manager_;
  IDeviceProperties& device_properties_provider_;
  IPersistenceProvider& persistence_provider_;
  INetDevManager& netdev_manager_;
  IDipSwitch & ip_dip_switch_;

};

} /* namespace netconf */
