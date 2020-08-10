// SPDX-License-Identifier: GPL-2.0-or-later

#include "InterfaceConfig.hpp"

#include <boost/algorithm/string/join.hpp>
#include <vector>
#include <sstream>

#include "NetconfdDbusClient.hpp"
#include "EthernetInterface.hpp"
#include "JsonConverter.hpp"

namespace netconf {
namespace api {

MacAddress GetMacAddress(const ::std::string &interface_name) {
  EthernetInterface interface { interface_name };
  return interface.GetMac();
}

::std::string ToString(const netconf::InterfaceConfig& config, const ::std::string& sep) noexcept{
  ::std::vector<::std::string> values;
  values.push_back("device=" + config.device_name_);

  if (config.state_ != InterfaceState::UNKNOWN) {
    auto state = ::std::string(config.state_ == InterfaceState::UP ? "up" : "down");
    values.push_back("state=" + state);
  }
  if (config.autoneg_ != Autonegotiation::UNKNOWN) {
    auto autoneg = ::std::string(config.autoneg_ == Autonegotiation::ON ? "on" : "off");
    values.push_back("autonegotiation=" + autoneg);
  }
  if (config.duplex_ != Duplex::UNKNOWN) {
    auto duplex = ::std::string(config.duplex_ == Duplex::FULL ? "full" : "half");
    values.push_back("duplex=" + duplex);
  }
  if (config.speed_ != InterfaceConfig::UNKNOWN_SPEED) {
    values.push_back("speed=" + std::to_string(config.speed_));
  }
  return boost::algorithm::join(values, sep);
}

::std::string ToJson(const netconf::InterfaceConfig &config) noexcept {
  JsonConverter jc;
  return jc.ToJsonString(config);
}

InterfaceConfigs GetInterfaceConfigs() {
  NetconfdDbusClient client;
  return MakeInterfaceConfigs(client.GetInterfaceConfigs());
}

Status SetInterfaceConfigs(const InterfaceConfigs &config) {
  NetconfdDbusClient client;
  ::std::string json_config = ToJson(config);
  if (client.SetInterfaceConfigs(json_config)) {
    return Status::OK;
  }
  return Status::ERROR;
}

InterfaceConfigs MakeInterfaceConfigs(const ::std::string &json_str) {
  JsonConverter jc;
  netconf::InterfaceConfigs ic;
  auto status = jc.FromJsonString(json_str, ic);
  if (status.NotOk()) {
    // TODO throw something useful
  }
  return InterfaceConfigs { ic };
}


}  // namespace api
}  // namespace netconf
