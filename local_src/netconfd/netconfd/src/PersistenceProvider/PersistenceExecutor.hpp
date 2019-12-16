// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include "Types.hpp"
#include "IPersistenceProvider.hpp"
#include "IPersistenceJsonConfigConverter.hpp"
#include "IFileEditor.hpp"
#include "IBackupRestore.hpp"
#include "IPersistence.hpp"
#include "IJsonConvert.hpp"

namespace netconfd {

class PersistenceExecutor : public IPersistenceProvider, public IPersistence<InterfaceConfigs> {
 public:
  PersistenceExecutor(const ::std::string& persistence_path, IPersistenceJsonConfigConverter& json_config_converter,
                      IJsonConvert<InterfaceConfigs>& port_configs_converter, IFileEditor& file_editor,
                      IBackupRestore& backup_restore, IBackupRestore& legacy_restore);
  virtual ~PersistenceExecutor() = default;

  PersistenceExecutor(const PersistenceExecutor&) = delete;
  PersistenceExecutor& operator=(const PersistenceExecutor&) = delete;
  PersistenceExecutor(const PersistenceExecutor&&) = delete;
  PersistenceExecutor& operator=(const PersistenceExecutor&&) = delete;

  Status Write(const BridgeConfig& config) override;
  Status Read(BridgeConfig& config) override;

  Status Write(const IPConfigs& configs) override;
  Status Read(IPConfigs& configs) override;

  Status Write(const InterfaceConfigs& configs) override;
  Status Read(InterfaceConfigs& configs) override;

  Status Read(BridgeConfig& config, IPConfigs& configs) override;

  Status Backup(const ::std::string& file_path) override;
  Status Restore(const ::std::string& file_path, BridgeConfig& bridge_config, IPConfigs& ip_configs,
                 InterfaceConfigs& interface_configs) override;
  uint32_t GetBackupParameterCount() const override;

 private:
  const ::std::uint32_t persistence_version_ = 1;
  ::std::string persistence_path_;
  ::std::string interface_config_file_path_;
  IPersistenceJsonConfigConverter& json_config_converter_;
  IJsonConvert<InterfaceConfigs>& port_configs_converter_;
  IFileEditor& file_editor_;
  IBackupRestore& backup_restore_;
  IBackupRestore& legacy_restore_;

  InterfaceConfigs current_interface_configs_;
  IPConfigs current_ip_configs_;
  BridgeConfig current_bridge_config_;

  void UpdateNeconfdJson() const;
  Status UpdateNetconfdJson() const;
  void UpdateInterfacesXml() const;
  Status ReadNetconfdJson();
  Status ReadInterfaceConfigJson();
};

} /* namespace netconfd */
