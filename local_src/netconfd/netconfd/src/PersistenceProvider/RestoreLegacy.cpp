// SPDX-License-Identifier: GPL-2.0-or-later

#include "RestoreLegacy.hpp"
#include <string>
#include <sstream>
#include <boost/algorithm/string/predicate.hpp>
#include <boost/format.hpp>
#include <boost/asio.hpp>

namespace netconfd {

using ::std::string;

using boost_address = boost::asio::ip::address;
using boost_error = boost::system::error_code;

RestoreLegacy::RestoreLegacy(IFileEditor &file_editor, IDeviceProperties &properties_provider)
    : file_editor_ { file_editor },
      properties_provider_ { properties_provider } {
}

static Status GetKeyValue(const ::std::string &backup_content, const ::std::string &key, ::std::string &value) {

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

static Status CalculateBroadcast(const ::std::string &address, const ::std::string &netmask, ::std::string &broadcast) {

  Status status;
  boost_error error_code;

  if (address == ZeroIP && netmask == ZeroIP) {
    broadcast = ZeroIP;
    return status;
  }

  auto binary_address = boost_address::from_string(address, error_code).to_v4().to_uint();
  if (error_code) {
    status.Append(StatusCode::BACKUP_FILE_ERROR, "Failed read ip address");
  }
  auto binary_netmask = boost_address::from_string(netmask, error_code).to_v4().to_uint();
  if (error_code) {
    status.Append(StatusCode::BACKUP_FILE_ERROR, "Failed read netmask address");
  }

  if (status.Ok()) {
    auto binary_network = binary_address & binary_netmask;
    auto binary_broadcast = binary_network | (~binary_netmask);

    boost::asio::ip::address_v4 bc(binary_broadcast);
    if (error_code) {
      status.Append(StatusCode::BACKUP_FILE_ERROR, "Failed to calculate broadcast address");
    } else {
      broadcast = bc.to_string(error_code);
    }
  }

  return status;
}

static Status GetIPConfigsFromAPreviousFirmware(const ::std::string &backup_content, ::std::string &ip_config) {

  Status status;

  ::std::string br0_config = { };
  ::std::string eth0_state;
  ::std::string eth0_config_type;
  ::std::string eth0_ip;
  ::std::string eth0_subnet;
  ::std::string eth0_broadcast;

  status = GetKeyValue(backup_content, "ip-config-type-eth0=", eth0_config_type);

  if (status.Ok()) {
    status = GetKeyValue(backup_content, "ip-address-eth0=", eth0_ip);
  }
  if (status.Ok()) {
    status = GetKeyValue(backup_content, "subnet-mask-eth0=", eth0_subnet);
  }
  if (status.Ok()) {
    status = CalculateBroadcast(eth0_ip, eth0_subnet, eth0_broadcast);
  }

  if (status.Ok()) {
    ip_config = R"("br0": {"source": ")" + eth0_config_type + R"(","ipaddr": ")" + eth0_ip + R"(","netmask": ")"
        + eth0_subnet + R"(","bcast": ")" + eth0_broadcast + R"("})";
//      if(){
//        backup_dipswitch_data = R"({})";
//      }
  }

  ::std::string br1_config = { };
  ::std::string eth1_state;
  ::std::string eth1_config_type;
  ::std::string eth1_ip;
  ::std::string eth1_subnet;
  ::std::string eth1_broadcast;

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
    status = CalculateBroadcast(eth1_ip, eth1_subnet, eth1_broadcast);
  }

  if (status.Ok()) {

    ip_config += R"(,"br1": {"source": ")" + eth1_config_type + R"(","ipaddr": ")" + eth1_ip + R"(","netmask": ")"
        + eth1_subnet + R"(","bcast": ")" + eth1_broadcast + R"("})";

  }

  return status;
}

static Status GetBridgeConfigFromAPreviousFirmware(const ::std::string &backup_content, ::std::string &bridge_config) {

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

  return status;
}

static Status AppendInterfaceConfig(const string &device, const string &backup_device,
                                    const ::std::string &backup_content, ::std::string &interface_config) {
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
  autoneg = (autoneg == "enabled") ? "on" : "off";
  state = (state == "enabled") ? "up" : "down";

  interface_config += str(
      boost::format(R"({"device":"%1%","autonegotiation":"%2%","state":"%3%","speed":%4%,"duplex":"%5%"})") % device
          % autoneg % state % speed % duplex);
  return status;
}

Status RestoreLegacy::GetInterfaceConfigFromAPreviousFirmware(const ::std::string &backup_content,
                                                              ::std::string &interface_config) const {

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

  if (properties_provider_.GetProductInterfaces().size() == 4) {
    interface_config += ",";
    interface_config += str(
        boost::format(R"({"device":"%1%","autonegotiation":"%2%","state":"%3%","speed":%4%,"duplex":"%5%"})") % "X11"
            % "on" % "up" % "100" % "full");
    interface_config += ",";
    interface_config += str(
        boost::format(R"({"device":"%1%","autonegotiation":"%2%","state":"%3%","speed":%4%,"duplex":"%5%"})") % "X12"
            % "on" % "up" % "100" % "full");
  }

  interface_config += "]";
  return status;
}

Status RestoreLegacy::GetConfigsFromAPreviousFirmwareBackup(const ::std::string &backup_content,
                                                            ::std::string &backup_network_data) const {

  Status status;

  ::std::string bridge_config;
  status = GetBridgeConfigFromAPreviousFirmware(backup_content, bridge_config);

  ::std::string ip_configs;
  if (status.Ok()) {
    status = GetIPConfigsFromAPreviousFirmware(backup_content, ip_configs);
  }

  string interface_config_json;
  if (status.Ok()) {
    status = GetInterfaceConfigFromAPreviousFirmware(backup_content, interface_config_json);
  }

  if (status.Ok()) {
    backup_network_data = R"( { "bridge-config" : { )" + bridge_config + R"(}, "ip-config" : { )" + ip_configs + R"(})"
        + R"(,"interface-config":)" + interface_config_json + "}";
  }
  return status;
}

Status RestoreLegacy::Restore(const ::std::string &file_path, ::std::string &backup_network_data,
                              ::std::string &backup_dipswitch_data, uint32_t &version) const {
  (void) backup_dipswitch_data;
  version = 1;

  auto backup_data = ::std::string { };
  auto status = file_editor_.Read(file_path, backup_data);
  if (status.NotOk()) {
    return status;
  }

  return GetConfigsFromAPreviousFirmwareBackup(backup_data, backup_network_data);
}

[[gnu::const]]
Status RestoreLegacy::Backup(const ::std::string &file_path, const ::std::string &data, const ::std::string &data_key,
                             uint32_t version) const {
  (void) version;
  (void) data_key;

  return file_editor_.Append(file_path, data);

}

} /* namespace netconfd */
