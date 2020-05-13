// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include "PersistenceJsonConfigConverter.hpp"
#include "FileEditor.hpp"
#include "BackupRestore.hpp"
#include "DipSwitch.hpp"
#include "RestoreLegacy.hpp"
#include "PersistenceExecutor.hpp"
#include "IPersistence.hpp"

namespace netconfd {

class PersistenceProvider : public IPersistenceProvider, public IPersistence<InterfaceConfigs> {
 public:
  PersistenceProvider(const ::std::string& persistence_path,
                      IDeviceProperties& properties_provider,
                      DipSwitch& dip_switch);
  virtual ~PersistenceProvider() = default;

  PersistenceProvider(const PersistenceProvider&) = delete;
  PersistenceProvider& operator=(const PersistenceProvider&) = delete;
  PersistenceProvider(const PersistenceProvider&&) = delete;
  PersistenceProvider& operator=(const PersistenceProvider&&) = delete;

  Status Write(const BridgeConfig& config) override;
  Status Read(BridgeConfig& config) override;

  Status Write(const IPConfigs& configs) override;
  Status Read(IPConfigs& configs) override;

  Status Write(const DipSwitchIpConfig& config) override;
  Status Read(DipSwitchIpConfig& config) override;

  Status Read(BridgeConfig& config, IPConfigs& configs) override;

  Status Read(InterfaceConfigs& port_configs) override;
  Status Write(const InterfaceConfigs& port_configs) override;

  Status Backup(const std::string &file_path,
                const std::string &targetversion) override;
  Status Restore(const ::std::string &file_path,
                 BridgeConfig &bridge_config,
                 IPConfigs &ip_configs,
                 InterfaceConfigs &interface_configs,
                 DipSwitchIpConfig &dip_switch_config) override;
  uint32_t GetBackupParameterCount() const override;

 private:

  IDeviceProperties& properties_provider_;
  PersistenceJsonConfigConverter json_config_converter_;
  FileEditor file_editor_;
  BackupRestore backup_restore_;
  RestoreLegacy restore_legacy_;
  PersistenceExecutor persistence_executor_;

};

} /* namespace netconfd */
