// SPDX-License-Identifier: GPL-2.0-or-later

#include <PersistenceProvider.hpp>

namespace netconfd {

PersistenceProvider::PersistenceProvider(const ::std::string& persistence_path)
    : backup_restore_ { file_editor_, 75 },
      restore_legacy_ { file_editor_ },
      persistence_executor_ { persistence_path, json_config_converter_, json_config_converter_, file_editor_,
          backup_restore_, restore_legacy_ } {
}

Status PersistenceProvider::Write(const BridgeConfig& config)  {
  return persistence_executor_.Write(config);
}
Status PersistenceProvider::Read(BridgeConfig& config)  {
  return persistence_executor_.Read(config);
}
Status PersistenceProvider::Write(const IPConfigs& configs)  {
  return persistence_executor_.Write(configs);
}
Status PersistenceProvider::Read(IPConfigs& configs)  {
  return persistence_executor_.Read(configs);
}
Status PersistenceProvider::Read(BridgeConfig& config, IPConfigs& configs)  {
  return persistence_executor_.Read(config, configs);
}
Status PersistenceProvider::Backup(const ::std::string& file_path) {
  return persistence_executor_.Backup(file_path);
}
Status PersistenceProvider::Restore(const ::std::string& file_path, BridgeConfig& bridge_config, IPConfigs& ip_configs,
                                    InterfaceConfigs& interface_configs) {
  return persistence_executor_.Restore(file_path, bridge_config, ip_configs, interface_configs);
}
uint32_t PersistenceProvider::GetBackupParameterCount() const {
  return persistence_executor_.GetBackupParameterCount();
}

Status PersistenceProvider::Read(InterfaceConfigs& port_configs) {
  return persistence_executor_.Read(port_configs);
}

Status PersistenceProvider::Write(const InterfaceConfigs& port_configs) {
  return persistence_executor_.Write(port_configs);
}

} /* namespace netconfd */
