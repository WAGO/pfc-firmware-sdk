// SPDX-License-Identifier: GPL-2.0-or-later

#include <PersistenceProvider.hpp>
#include "TypesHelper.hpp"

namespace netconf {

PersistenceProvider::PersistenceProvider(const ::std::string &persistence_path, IDeviceProperties &properties_provider,
                                         DipSwitch &dip_switch)
    : properties_provider_ { properties_provider },
      backup_restore_ { file_editor_, 75 },
      restore_legacy_ { file_editor_, properties_provider },
      persistence_executor_ { persistence_path, file_editor_, backup_restore_, restore_legacy_, dip_switch } {
}

Error PersistenceProvider::Write(const BridgeConfig &config) {
  return persistence_executor_.Write(config);
}

Error PersistenceProvider::Read(BridgeConfig &config) {
  return persistence_executor_.Read(config);
}

Error PersistenceProvider::Write(const IPConfigs &configs) {
  return persistence_executor_.Write(configs);
}

Error PersistenceProvider::Read(IPConfigs &configs) {
  Error status = persistence_executor_.Read(configs);
  RemoveUnnecessaryIPParameter(configs);
  return status;
}

Error PersistenceProvider::Read(BridgeConfig &config, IPConfigs &configs) {
  return persistence_executor_.Read(config, configs);
}

Error PersistenceProvider::Backup(const std::string &file_path, const std::string &targetversion) {
  return persistence_executor_.Backup(file_path, targetversion);
}

Error PersistenceProvider::Restore(const ::std::string &file_path, BridgeConfig &bridge_config, IPConfigs &ip_configs,
                                    InterfaceConfigs &interface_configs, DipSwitchIpConfig &dip_switch_config) {
  return persistence_executor_.Restore(file_path, bridge_config, ip_configs, interface_configs, dip_switch_config);
}

uint32_t PersistenceProvider::GetBackupParameterCount() const {
  return persistence_executor_.GetBackupParameterCount();
}

Error PersistenceProvider::Read(InterfaceConfigs &port_configs) {
  return persistence_executor_.Read(port_configs);
}

Error PersistenceProvider::Write(const InterfaceConfigs &port_configs) {
  return persistence_executor_.Write(port_configs);
}

Error PersistenceProvider::Read(DipSwitchIpConfig &config) {
  return persistence_executor_.Read(config);
}

Error PersistenceProvider::Write(const DipSwitchIpConfig &config) {
  return persistence_executor_.Write(config);
}

}
