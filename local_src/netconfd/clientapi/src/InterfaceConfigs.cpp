// SPDX-License-Identifier: LGPL-3.0-or-later

#include "InterfaceConfig.hpp"

#include <boost/algorithm/string/join.hpp>
#include <algorithm>
#include <sstream>

#include "ConfigBase.hpp"
#include "JsonConverter.hpp"
#include "EthernetInterface.hpp"

namespace netconf {
namespace api {


InterfaceConfigs::InterfaceConfigs(const netconf::InterfaceConfigs& config): ConfigBase<netconf::InterfaceConfig>(config){
}

std::string InterfaceConfigs::GetCompareValue(const netconf::InterfaceConfig& ic) const noexcept {
  return ic.device_name_;
}

void InterfaceConfigs::AddInterfaceConfig(InterfaceConfig config) {
  AddConfig(config);
}

void InterfaceConfigs::RemoveInterfaceConfig(const ::std::string &interface_name) {
  RemoveConfig(interface_name);
}

boost::optional<InterfaceConfig> InterfaceConfigs::GetInterfaceConfig(const ::std::string &interface_name) {
  return GetConfig(interface_name);
}

::std::string ToJson(const InterfaceConfigs& configs) noexcept {
  JsonConverter jc;
  return jc.ToJsonString(configs.GetConfig());
}

::std::string ToPrettyJson(const InterfaceConfigs& configs) noexcept {
  JsonConverter jc;
  return jc.ToJsonString(configs.GetConfig(), JsonFormat::PRETTY);
}

::std::string ToString(const InterfaceConfigs& configs) noexcept {
  ::std::stringstream ss;
  for(const auto& config: configs.GetConfig()){
    ss << api::ToString(config) << " ";
  }
  return ss.str();
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

::std::string ToPrettyJson(const netconf::InterfaceConfig &config) noexcept {
  JsonConverter jc;
  return jc.ToJsonString(config, JsonFormat::PRETTY);
}

Status MakeInterfaceConfigs(const ::std::string &json_str, InterfaceConfigs& config) {
  JsonConverter jc;
  netconf::InterfaceConfigs ic;
  Status status = jc.FromJsonString(json_str, ic);
  config = InterfaceConfigs{ic};
  return status;
}


} //namespace api
} //namespace netconf
