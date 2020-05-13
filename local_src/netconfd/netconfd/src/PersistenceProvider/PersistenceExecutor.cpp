// SPDX-License-Identifier: GPL-2.0-or-later

#include "PersistenceExecutor.hpp"

#include <boost/format.hpp>
#include <iostream>

#include "NetworkInterfacesXML.hpp"
#include "JsonConfigConverter.hpp"
#include "JsonBuilder.hpp"
#include "JsonRestorer.hpp"
#include "Logger.hpp"
#include "TypesHelper.hpp"
#include "FirmwareVersion.hpp"

namespace netconfd {

constexpr auto bridge_backup_key = "bridge-config";
constexpr auto ip_backup_key = "ip-config";
constexpr auto dip_ip_backup_key = "dip-ip-config";
constexpr auto interface_backup_key = "interface-config";

PersistenceExecutor::PersistenceExecutor(const ::std::string &persistence_path,
                                         IPersistenceJsonConfigConverter &json_config_converter,
                                         IJsonConvert<InterfaceConfigs> &port_configs_converter,
                                         IFileEditor &file_editor, IBackupRestore &backup_restore,
                                         IBackupRestore &legacy_restore, IDipSwitch &dip_switch)
    :
    persistence_path_ { persistence_path + "/netconfd.json" },
    interface_config_file_path_ { persistence_path + "/netconfd_interface_config.json" },
    json_config_converter_ { json_config_converter },
    port_configs_converter_ { port_configs_converter },
    file_editor_ { file_editor },
    backup_restore_ { backup_restore },
    legacy_restore_ { legacy_restore },
    dip_switch_ { dip_switch } {

  ReadNetconfdJson();
  ReadInterfaceConfigJson();
  UpdateInterfacesXml();
}

Status PersistenceExecutor::UpdateNetconfdJson() const {
  Status status;
  ::std::string new_json;
  if (status.Ok()) {
    if (dip_switch_.GetMode() == DipSwitchMode::HW_NOT_AVAILABLE || current_dip_switch_config_.IsIncomplete()) {
      status = json_config_converter_.ToJson(current_bridge_config_, current_ip_configs_, true, new_json);
    } else {
      status = json_config_converter_.ToJson(current_bridge_config_, current_ip_configs_, current_dip_switch_config_,
                                             true, new_json);
    }
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

Status PersistenceExecutor::Write(const BridgeConfig &config) {
  if (current_bridge_config_ == config) {
    return Status();
  }

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
    current_dip_switch_config_.Clear();

    if (dip_switch_.GetMode() == DipSwitchMode::HW_NOT_AVAILABLE) {
      status = json_config_converter_.ToConfigs(json, current_bridge_config_, current_ip_configs_);
    } else {
      status = json_config_converter_.ToConfigs(json, current_bridge_config_, current_ip_configs_,
                                                current_dip_switch_config_);
    }
  }
  return status;
}

Status PersistenceExecutor::Read(BridgeConfig &config) {
  config = current_bridge_config_;
  if (current_bridge_config_.empty()) {
    return Status { StatusCode::FILE_READ_ERROR, "Empty persistence content" };
  }
  return Status { };
}

Status PersistenceExecutor::Read(BridgeConfig &config, IPConfigs &configs) {
  config = current_bridge_config_;
  configs = current_ip_configs_;

  if (current_bridge_config_.empty() || current_ip_configs_.empty()) {
    return Status { StatusCode::FILE_READ_ERROR, "Empty persistence content" };
  }

  return Status { };
}

Status PersistenceExecutor::Read(DipSwitchIpConfig &config) {
  config = current_dip_switch_config_;
  if (current_dip_switch_config_.IsIncomplete()) {
    return Status { StatusCode::FILE_READ_ERROR, "Empty persistence content" };
  }
  return Status { };
}

Status PersistenceExecutor::Write(const DipSwitchIpConfig &config) {
  if (current_dip_switch_config_ == config) {
    return Status();
  }

  current_dip_switch_config_ = config;
  Status status = UpdateNetconfdJson();

  if (status.NotOk()) {
    status.Prepend("Write dip ip config to persistence failed: ");
  }

  //UpdateInterfacesXml();

  return status;
}

Status PersistenceExecutor::Write(const IPConfigs &configs) {
  if (current_ip_configs_ == configs) {
    return Status();
  }

  current_ip_configs_ = configs;
  Status status = UpdateNetconfdJson();

  if (status.NotOk()) {
    status.Prepend("Write ip config to persistence failed: ");
  }

  UpdateInterfacesXml();

  return status;
}

Status PersistenceExecutor::Read(IPConfigs &configs) {
  configs = current_ip_configs_;

  if (current_ip_configs_.empty()) {
    return Status { StatusCode::FILE_READ_ERROR, "Empty persistence content" };
  }
  return Status { };
}

Status PersistenceExecutor::Write(const InterfaceConfigs &configs) {

  if (current_interface_configs_ == configs) {
    return Status();
  }

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

Status PersistenceExecutor::Read(InterfaceConfigs &configs) {
  configs = current_interface_configs_;
  return Status { };
}

static void AddLegacyIpParameter(IPConfigs &ip_configs, ::std::string bridge, ::std::string &legacy_ip_config) {

  auto bridge_index = ::std::string(1, bridge.back());

  auto find_bridge = [&](IPConfig ip_config) {
    return ip_config.interface_ == bridge;
  };

  auto it = ::std::find_if(ip_configs.begin(), ip_configs.end(), find_bridge);
  if (it != ip_configs.end()) {
    legacy_ip_config.append("ip-config-type-eth" + bridge_index + "=" + IPSourceToString(it->source_) + "\n");
    legacy_ip_config.append("ip-address-eth" + bridge_index + "=" + it->address_ + "\n");
    legacy_ip_config.append("subnet-mask-eth" + bridge_index + "=" + it->netmask_ + "\n");
  } else {
    legacy_ip_config.append("ip-config-type-eth" + bridge_index + "=static\n");
    legacy_ip_config.append("ip-address-eth" + bridge_index + "=0.0.0.0\n");
    legacy_ip_config.append("subnet-mask-eth" + bridge_index + "=0.0.0.0\n");
  }
}

void PersistenceExecutor::ModifyBr0AddressToDipSwitch(IPConfigs &ip_configs) {

  auto find_br0 = [&](IPConfig &ip_config) {
    return ip_config.interface_ == "br0";
  };

  auto it = ::std::find_if(ip_configs.begin(), ip_configs.end(), find_br0);
  if (it != ip_configs.end()) {
    it->address_ = current_dip_switch_config_.address_;
    it->netmask_ = current_dip_switch_config_.netmask_;
    it->broadcast_ = "";
  }
}

static ::std::string GenerateLegacyIPBackup(IPConfigs &ip_configs) {

  ::std::string legacy_ip_parameter;
  AddLegacyIpParameter(ip_configs, "br0", legacy_ip_parameter);
  AddLegacyIpParameter(ip_configs, "br1", legacy_ip_parameter);
  return legacy_ip_parameter;
}

Status PersistenceExecutor::Backup(const std::string &file_path, const std::string &targetversion) {
  IPConfigs current_ip_configs = current_ip_configs_;

  FirmwareVersion fw_target { targetversion };
  FirmwareVersion fw_with_dipswitch { "03.04.02(16)" };

  if (fw_target.IsValid() && (fw_target < fw_with_dipswitch) && (dip_switch_.GetMode() == DipSwitchMode::STATIC)) {
    ModifyBr0AddressToDipSwitch(current_ip_configs);
  }

  JsonBuilder jb_network_data;
  jb_network_data.Append(bridge_backup_key, current_bridge_config_).Append(ip_backup_key, current_ip_configs).Append(
      interface_backup_key, current_interface_configs_);

  ::std::string dipswitch_data;
  if (dip_switch_.GetMode() != DipSwitchMode::HW_NOT_AVAILABLE) {
    JsonBuilder jb_dipswitch_data;
    jb_dipswitch_data.Append(dip_ip_backup_key, current_dip_switch_config_);
    dipswitch_data = jb_dipswitch_data.ToString();
  }

  Status status = backup_restore_.Backup(file_path, jb_network_data.ToString(), dipswitch_data, persistence_version_);

  if (status.Ok()) {
    ::std::string legacy_ip_parameter = GenerateLegacyIPBackup(current_ip_configs);
    status = legacy_restore_.Backup(file_path, legacy_ip_parameter, "", 0);
  }

  return status;
}

Status PersistenceExecutor::Restore(const ::std::string &file_path, BridgeConfig &bridge_config, IPConfigs &ip_configs,
                                    InterfaceConfigs &interface_configs, DipSwitchIpConfig &dip_switch_config) {
  bridge_config.clear();
  ip_configs.clear();
  interface_configs.clear();
  dip_switch_config.Clear();

  uint32_t version = 0;
  ::std::string network_data_json;
  ::std::string dipswitch_data_json;

  Status status = backup_restore_.Restore(file_path, network_data_json, dipswitch_data_json, version);
  if (status.NotOk()) {
    status = legacy_restore_.Restore(file_path, network_data_json, dipswitch_data_json, version);
  }

  if (status.Ok() && (version != persistence_version_)) {
    auto message = str(
        boost::format("Backup Version not supported, need (%1%) have (%2%)") % persistence_version_ % version);
    status = Status(StatusCode::BACKUP_FILE_ERROR, message);
  }

  JsonRestorer data_restorer { ::std::move(network_data_json) };
  if (status.Ok()) {
    status = data_restorer.Restore(bridge_backup_key, bridge_config);
  }
  if (status.Ok()) {
    status = data_restorer.Restore(ip_backup_key, ip_configs);
  }
  if (status.Ok()) {
    status = data_restorer.Restore(interface_backup_key, interface_configs);
  }

  if (status.Ok() && not dipswitch_data_json.empty()) {
    JsonRestorer dip_switch_restorer { ::std::move(dipswitch_data_json) };
    status = dip_switch_restorer.Restore(dip_ip_backup_key, dip_switch_config);
  }

  ::std::string json_pretty;
  if (status.Ok()) {
    if (dip_switch_.GetMode() == DipSwitchMode::HW_NOT_AVAILABLE) {
      status = json_config_converter_.ToJson(bridge_config, ip_configs, true, json_pretty);
    } else {
      status = json_config_converter_.ToJson(bridge_config, ip_configs, dip_switch_config, true, json_pretty);
    }
  }

  if (status.Ok()) {
    status = file_editor_.Write(persistence_path_, json_pretty);
    current_bridge_config_ = bridge_config;
    current_ip_configs_ = ip_configs;
    current_dip_switch_config_ = dip_switch_config;
  }

  if (status.Ok()) {
    auto json_string = port_configs_converter_.ToJsonString(interface_configs);
    current_interface_configs_ = interface_configs;
    status = file_editor_.Write(interface_config_file_path_, json_string);
  }

  if (status.NotOk()) {
    bridge_config.clear();
    ip_configs.clear();
    dip_switch_config.Clear();
    status.Prepend("Restore failed: ");
  }

  UpdateInterfacesXml();

  return status;
}

uint32_t PersistenceExecutor::GetBackupParameterCount() const {
  return backup_restore_.GetBackupParameterCount();
}

} /* namespace netconfd */
