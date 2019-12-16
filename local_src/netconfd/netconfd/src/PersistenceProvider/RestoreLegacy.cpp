// SPDX-License-Identifier: GPL-2.0-or-later

#include "RestoreLegacy.hpp"
#include <string>
#include <sstream>
#include <boost/algorithm/string/predicate.hpp>
#include <boost/format.hpp>

namespace netconfd {

using ::std::string;

RestoreLegacy::RestoreLegacy(IFileEditor& file_editor)
    : file_editor_ { file_editor } {
}

static Status GetKeyValue(const ::std::string& backup_content, const ::std::string& key, ::std::string& value) {

  Status status;

  ::std::stringstream data_stream = ::std::stringstream(backup_content);
  ::std::string subdata;
  while (::std::getline(data_stream, subdata, '\n')) {
    if (boost::starts_with(subdata, key)) {
      subdata.erase(0, key.length());
      value += subdata;
    }
  }

  if (value.empty()) {
    status.Append(StatusCode::BACKUP_FILE_ERROR, "Missing key " + key + " error.");
    return status;
  }
  return status;

}

static Status GetIPConfigsFromAPreviousFirmware(const ::std::string& backup_content, const ::std::string& dsa_mode,
                                                std::string& ip_config) {

  Status status;

  //Get eth0 settings
  ::std::string br0_config = { };

  ::std::string eth0_state;
  ::std::string eth0_config_type;
  ::std::string eth0_ip;
  ::std::string eth0_subnet;

  status = GetKeyValue(backup_content, "ip-config-type-eth0=", eth0_config_type);

  if (status.Ok()) {
    status = GetKeyValue(backup_content, "ip-address-eth0=", eth0_ip);
  }
  if (status.Ok()) {
    status = GetKeyValue(backup_content, "subnet-mask-eth0=", eth0_subnet);
  }

  if (status.Ok()) {

    ip_config = R"("br0": {"source": ")" + eth0_config_type + R"(","ipaddr": ")" + eth0_ip + R"(","netmask": ")"
        + eth0_subnet + R"("})";

  }

  //Get eth1 settings
  ::std::string br1_config = { };
  if (dsa_mode == "1") {
    ::std::string eth1_state;
    ::std::string eth1_config_type;
    ::std::string eth1_ip;
    ::std::string eth1_subnet;

    if (status.Ok()) {
      status = GetKeyValue(backup_content, "ip-config-type-eth1=", eth1_config_type);
    }
    if (status.Ok()) {
      status = GetKeyValue(backup_content, "ip-address-eth1=", eth1_ip);
    }
    if (status.Ok()) {
      status = GetKeyValue(backup_content, "subnet-mask-eth1=", eth1_subnet);
    }

    if (status.Ok()) {

      ip_config += R"(,"br1": {"source": ")" + eth1_config_type + R"(","ipaddr": ")" + eth1_ip + R"(","netmask": ")"
          + eth1_subnet + R"("})";

    }

  }

  return status;
}

static Status GetBridgeConfigFromAPreviousFirmware(const ::std::string& backup_content, ::std::string& dsa_mode,
                                                   ::std::string& bridge_config) {

  Status status;

  ::std::string dsa_tag_value;
  status = GetKeyValue(backup_content, "dsa-mode=", dsa_tag_value);
  if (status.Ok()) {
    if ("0" == dsa_tag_value) {
      bridge_config = R"("br0":["X1","X2"],"br1":[])";
    } else if ("1" == dsa_tag_value) {
      bridge_config = R"("br0":["X1"],"br1":["X2"])";
    } else {
      status.Append(StatusCode::BACKUP_FILE_ERROR, "Invalid dsa-mode " + dsa_tag_value);
    }
  }

  dsa_mode = dsa_tag_value;

  return status;
}

static Status AppendInterfaceConfig(const string& device, const string& backup_device,
                                    const ::std::string& backup_content, ::std::string& interface_config) {
  string state;
  string autoneg;
  string speed;
  string duplex;
  Status status = GetKeyValue(backup_content, backup_device + "-state=", state);
  if (status.Ok()) {
    status = GetKeyValue(backup_content, backup_device + "-autoneg=", autoneg);
  }
  if (status.Ok()) {
    status = GetKeyValue(backup_content, backup_device + "-speed=", speed);
  }
  if (status.Ok()) {
    status = GetKeyValue(backup_content, backup_device + "-duplex=", duplex);
  }
  bool autoneg_bool = "enabled" == autoneg;
  state = (state == "enabled") ? "up" : "down";

  interface_config += str(
      boost::format(R"({"device":"%1%","autonegotiation":%2%,"state":"%3%","speed":%4%,"duplex":"%5%"})") % device
          % boost::io::group(std::boolalpha, autoneg_bool) % state % speed % duplex);
  return status;
}

static Status GetInterfaceConfigFromAPreviousFirmware(const ::std::string& backup_content,
                                                      ::std::string& interface_config) {

  /*
   SaveBackupValue "eth${interfaceNo}-state" "$state" "$backupFile"
   SaveBackupValue "eth${interfaceNo}-autoneg" "$autoneg" "$backupFile"
   SaveBackupValue "eth${interfaceNo}-speed" "${speed}" "$backupFile"
   SaveBackupValue "eth${interfaceNo}-duplex" "${duplex}" "$backupFile"
   */
  interface_config = "[";
  auto status = AppendInterfaceConfig("X1", "eth0", backup_content, interface_config);
  interface_config += ",";
  status = AppendInterfaceConfig("X2", "eth1", backup_content, interface_config);
  interface_config += "]";
  return status;
}

static Status GetConfigsFromAPreviousFirmwareBackup(const ::std::string& backup_content, ::std::string& data) {

  Status status;
  ::std::string dsa_mode;

  ::std::string bridge_config;
  status = GetBridgeConfigFromAPreviousFirmware(backup_content, dsa_mode, bridge_config);

  ::std::string ip_configs;
  if (status.Ok()) {
    status = GetIPConfigsFromAPreviousFirmware(backup_content, dsa_mode, ip_configs);
  }

  string interface_config_json;
  if (status.Ok()) {
    status = GetInterfaceConfigFromAPreviousFirmware(backup_content, interface_config_json);
  }

  if (status.Ok()) {
    data = R"( { "bridge-config" : { )" + bridge_config + R"(}, "ip-config" : { )" + ip_configs + R"(})"
        + R"(,"interface-config":)" + interface_config_json + "}";
  }
  return status;
}

Status RestoreLegacy::Restore(const ::std::string& file_path, ::std::string& data, uint32_t& version) const {
  version = 1;

  auto backup_data = ::std::string { };
  auto status = file_editor_.Read(file_path, backup_data);
  if (status.NotOk()) {
    return status;
  }

  return GetConfigsFromAPreviousFirmwareBackup(backup_data, data);
}

[[gnu::const]]
Status RestoreLegacy::Backup(const ::std::string& file_path, const ::std::string& data, uint32_t version) const {
  (void) file_path;
  (void) version;
  (void) data;
  return Status(StatusCode::ERROR, "Not Implemented");
}

} /* namespace netconfd */
