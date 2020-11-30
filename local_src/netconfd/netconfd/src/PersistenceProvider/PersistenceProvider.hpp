// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include "FileEditor.hpp"
#include "BackupRestore.hpp"
#include "DipSwitch.hpp"
#include "RestoreLegacy.hpp"
#include "PersistenceExecutor.hpp"
#include "IPersistence.hpp"

namespace netconf {

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

  Error Write(const BridgeConfig& config) override;
  Error Read(BridgeConfig& config) override;

  Error Write(const IPConfigs& configs) override;
  Error Read(IPConfigs& configs) override;

  Error Write(const DipSwitchIpConfig& config) override;
  Error Read(DipSwitchIpConfig& config) override;

  Error Read(BridgeConfig& config, IPConfigs& configs) override;

  Error Read(InterfaceConfigs& port_configs) override;
  Error Write(const InterfaceConfigs& port_configs) override;

  Error Backup(const std::string &file_path,
                const std::string &targetversion) override;
  Error Restore(const ::std::string &file_path,
                 BridgeConfig &bridge_config,
                 IPConfigs &ip_configs,
                 InterfaceConfigs &interface_configs,
                 DipSwitchIpConfig &dip_switch_config) override;
  uint32_t GetBackupParameterCount() const override;

 private:

  IDeviceProperties& properties_provider_;
  FileEditor file_editor_;
  BackupRestore backup_restore_;
  RestoreLegacy restore_legacy_;
  PersistenceExecutor persistence_executor_;

};

} /* namespace netconf */
