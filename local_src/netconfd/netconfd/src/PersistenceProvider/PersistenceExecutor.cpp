// SPDX-License-Identifier: GPL-2.0-or-later

#include "PersistenceExecutor.hpp"

#include <boost/format.hpp>
#include <iostream>

#include "NetworkInterfacesXML.hpp"
#include "JsonConfigConverter.hpp"
#include "JsonBuilder.hpp"
#include "JsonRestorer.hpp"
#include "Logger.hpp"

namespace netconfd {

constexpr auto bridge_backup_key = "bridge-config";
constexpr auto ip_backup_key = "ip-config";
constexpr auto interface_backup_key = "interface-config";

PersistenceExecutor::PersistenceExecutor(const ::std::string& persistence_path,
                                         IPersistenceJsonConfigConverter& json_config_converter,
                                         IJsonConvert<InterfaceConfigs>& port_configs_converter,
                                         IFileEditor& file_editor, IBackupRestore& backup_restore,
                                         IBackupRestore& legacy_restore)
    : persistence_path_ { persistence_path + "/netconfd.json" },
      interface_config_file_path_ { persistence_path + "/netconfd_interface_config.json" },
      json_config_converter_ { json_config_converter },
      port_configs_converter_ { port_configs_converter },
      file_editor_ { file_editor },
      backup_restore_ { backup_restore },
      legacy_restore_ { legacy_restore } {

  ReadNetconfdJson();
  ReadInterfaceConfigJson();
  UpdateInterfacesXml();
}

Status PersistenceExecutor::UpdateNetconfdJson() const {
  Status status;
  ::std::string new_json;
  if (status.Ok()) {
    status = json_config_converter_.ToJson(current_bridge_config_, current_ip_configs_, true, new_json);
  }
  if (status.Ok()) {
    status = file_editor_.Write(persistence_path_, new_json);
  }
  return status;
}

void PersistenceExecutor::UpdateInterfacesXml() const {
  auto status = WriteNetworkInterfacesXML(file_editor_, current_bridge_config_, current_ip_configs_,
                                          current_interface_configs_);
  if (status.NotOk()) {
    LogWarning("PersistenceExecutor: " + status.GetMessage());
  }
}

Status PersistenceExecutor::Write(const BridgeConfig& config) {

  current_bridge_config_ = config;

  Status status = UpdateNetconfdJson();

  UpdateInterfacesXml();

  if (status.NotOk()) {
    status.Prepend("Write bridge config to persistence failed: ");
  }
  return status;
}

Status PersistenceExecutor::ReadNetconfdJson() {
  ::std::string json;
  Status status = file_editor_.Read(persistence_path_, json);

  if (status.Ok()) {
    current_bridge_config_.clear();
    current_ip_configs_.clear();
    status = json_config_converter_.ToConfigs(json, current_bridge_config_, current_ip_configs_);
  }
  return status;
}

Status PersistenceExecutor::Read(BridgeConfig& config) {
  config = current_bridge_config_;
  if (current_bridge_config_.empty()) {
    return Status { StatusCode::FILE_READ_ERROR, "Empty persistence content" };
  }
  return Status { };
}

Status PersistenceExecutor::Read(BridgeConfig& config, IPConfigs& configs) {
  config = current_bridge_config_;
  configs = current_ip_configs_;

  if (current_bridge_config_.empty() || current_ip_configs_.empty()) {
    return Status { StatusCode::FILE_READ_ERROR, "Empty persistence content" };
  }

  return Status { };
}

Status PersistenceExecutor::Write(const IPConfigs& configs) {

  current_ip_configs_ = configs;
  Status status = UpdateNetconfdJson();

  if (status.NotOk()) {
    status.Prepend("Write ip config to persistence failed: ");
  }

  UpdateInterfacesXml();

  return status;
}

Status PersistenceExecutor::Read(IPConfigs& configs) {
  configs = current_ip_configs_;

  if (current_ip_configs_.empty()) {
    return Status { StatusCode::FILE_READ_ERROR, "Empty persistence content" };
  }
  return Status { };
}

Status PersistenceExecutor::Write(const InterfaceConfigs& configs) {

  current_interface_configs_ = configs;

  auto json_string = port_configs_converter_.ToJsonString(current_interface_configs_);

  Status status = file_editor_.Write(interface_config_file_path_, json_string);

  UpdateInterfacesXml();

  return status;
}

Status PersistenceExecutor::ReadInterfaceConfigJson() {
  ::std::string json;
  Status status = file_editor_.Read(interface_config_file_path_, json);
  if (status.Ok()) {
    current_interface_configs_.clear();
    port_configs_converter_.FromJsonString(json, current_interface_configs_);
  }
  return status;
}

Status PersistenceExecutor::Read(InterfaceConfigs& configs) {
  configs = current_interface_configs_;
  return Status { };
}

Status PersistenceExecutor::Backup(const ::std::string& file_path) {
  JsonBuilder builder;
  builder.Append(bridge_backup_key, current_bridge_config_).Append(ip_backup_key, current_ip_configs_).Append(
      interface_backup_key, current_interface_configs_);

  Status status = backup_restore_.Backup(file_path, builder.ToString(), persistence_version_);

  if (status.NotOk()) {
    status.Prepend("Backup failed: ");
  }

  return status;
}

Status PersistenceExecutor::Restore(const ::std::string& file_path, BridgeConfig& bridge_config, IPConfigs& ip_configs,
                                    InterfaceConfigs& interface_configs) {
  bridge_config.clear();
  ip_configs.clear();
  interface_configs.clear();

  ::std::string json;
  uint32_t version = 0;
  Status status = backup_restore_.Restore(file_path, json, version);
  if (status.NotOk()) {
    status = legacy_restore_.Restore(file_path, json, version);
  }

  if (status.Ok() && (version != persistence_version_)) {
    auto message = str(
        boost::format("Backup Version not supported, need (%1%) have (%2%)") % persistence_version_ % version);
    status = Status(StatusCode::BACKUP_FILE_ERROR, message);
  }

  JsonRestorer json_restorer { ::std::move(json) };
  if (status.Ok()) {
    status = json_restorer.Restore(bridge_backup_key, bridge_config);
  }
  if (status.Ok()) {
    status = json_restorer.Restore(ip_backup_key, ip_configs);
  }
  if (status.Ok()) {
    status = json_restorer.Restore(interface_backup_key, interface_configs);
  }

  ::std::string json_pretty;
  if (status.Ok()) {
    status = json_config_converter_.ToJson(bridge_config, ip_configs, true, json_pretty);
  }

  if (status.Ok()) {
    status = file_editor_.Write(persistence_path_, json_pretty);
    current_bridge_config_ = bridge_config;
    current_ip_configs_ = ip_configs;
  }

  if (status.Ok()) {
    auto json_string = port_configs_converter_.ToJsonString(interface_configs);
    current_interface_configs_ = interface_configs;
    status = file_editor_.Write(interface_config_file_path_, json_string);
  }

  if (status.NotOk()) {
    bridge_config.clear();
    ip_configs.clear();
    status.Prepend("Restore failed: ");
  }

  UpdateInterfacesXml();

  return status;
}

uint32_t PersistenceExecutor::GetBackupParameterCount() const {
  return backup_restore_.GetBackupParameterCount();
}

} /* namespace netconfd */
