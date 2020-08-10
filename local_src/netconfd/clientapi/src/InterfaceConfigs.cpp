// SPDX-License-Identifier: GPL-2.0-or-later

#include "InterfaceConfig.hpp"

#include <boost/algorithm/string/join.hpp>
#include <algorithm>
#include <sstream>

#include "ConfigBase.hpp"
#include "JsonConverter.hpp"

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

::std::string ToString(const InterfaceConfigs& configs) noexcept {
  ::std::stringstream ss;
  for(const auto& config: configs.GetConfig()){
    ss << api::ToString(config) << " ";
  }
  return ss.str();
}

} //namespace api
} //namespace netconf
