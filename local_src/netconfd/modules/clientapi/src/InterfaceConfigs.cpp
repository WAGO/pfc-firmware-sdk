// SPDX-License-Identifier: LGPL-3.0-or-later

#include "InterfaceConfig.hpp"

#include <boost/algorithm/string/join.hpp>
#include <algorithm>
#include <sstream>

#include "ConfigBase.hpp"
#include "JsonConverter.hpp"
#include "EthernetInterface.hpp"
#include "InterfaceConfigurationValidator.hpp"

namespace netconf {
namespace api {

InterfaceConfigs::InterfaceConfigs(const netconf::InterfaceConfigs &config)
    : ConfigBase<netconf::InterfaceConfig>(config) {
}

std::string InterfaceConfigs::GetCompareValue(const netconf::InterfaceConfig &ic) const noexcept {
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

::std::string ToJson(const InterfaceConfigs &configs) noexcept {
  JsonConverter jc;
  return jc.ToJsonString(configs.GetConfig());
}

::std::string ToPrettyJson(const InterfaceConfigs &configs) noexcept {
  JsonConverter jc;
  return jc.ToJsonString(configs.GetConfig(), JsonFormat::PRETTY);
}

::std::string ToString(const InterfaceConfigs &configs) noexcept {
  ::std::stringstream ss;
  for (const auto &config : configs.GetConfig()) {
    ss << api::ToString(config) << " ";
  }
  return ss.str();
}

static ::std::string InterfaceConfigToString(const netconf::InterfaceConfig &config, const ::std::string &sep) {
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
    auto duplex = config.duplex_ == Duplex::FULL ? ::std::string("full") : ::std::string("half");
    values.push_back("duplex=" + duplex);
  }
  if (config.speed_ != InterfaceConfig::UNKNOWN_SPEED) {
    values.push_back("speed=" + std::to_string(config.speed_));
  }
  if (config.mac_learning_ != MacLearning::UNKNOWN) {
    auto learning = ::std::string(config.mac_learning_ == MacLearning::ON ? "on" : "off");
    values.push_back("state=" + learning);
  }
  return boost::algorithm::join(values, sep);
}

::std::string ToString(const netconf::InterfaceConfig &config, const ::std::string &sep) noexcept {
  return InterfaceConfigToString(config, sep);
}

::std::string ToJson(const netconf::InterfaceConfig &config) noexcept {
  JsonConverter jc;
  return jc.ToJsonString(config);
}

::std::string ToPrettyJson(const netconf::InterfaceConfig &config) noexcept {
  JsonConverter jc;
  return jc.ToJsonString(config, JsonFormat::PRETTY);
}

::std::string ToJson(const InterfaceStatuses &states) noexcept {
  JsonConverter jc;
  return jc.ToJsonString(states);
}

::std::string ToPrettyJson(const InterfaceStatuses &states) noexcept {
  JsonConverter jc;
  return jc.ToJsonString(states, JsonFormat::PRETTY);
}

::std::string ToString(const netconf::InterfaceStatus &state, const ::std::string &sep) noexcept {
  ::std::vector<::std::string> values;

  values.push_back(InterfaceConfigToString(state, sep));

  values.push_back("mac-address=" + state.mac_.ToString());

  if (state.link_state_ != LinkState::UNKNOWN) {
    auto link_state = ::std::string(state.link_state_ == LinkState::UP ? "up" : "down");
    values.push_back("link-state=" + link_state);
  }

  return boost::algorithm::join(values, sep);
}

::std::string ToString(const InterfaceStatuses &states) noexcept {
  ::std::stringstream ss;
  for (const auto &state : states) {
    ss << api::ToString(state) << " ";
  }
  return ss.str();
}

Status MakeInterfaceConfigs(const ::std::string &json_str, InterfaceConfigs &config) {
  JsonConverter jc;
  netconf::InterfaceConfigs ic;
  Status status = jc.FromJsonString(json_str, ic);
  config = InterfaceConfigs { ic };
  return status;
}

Status ValidateInterfaceConfigs(const InterfaceConfigs &configs, const InterfaceInformations &interface_infos) {
  return netconf::InterfaceConfigurationValidator::Validate(configs.GetConfig(), interface_infos);
}

}  //namespace api
}  //namespace netconf
