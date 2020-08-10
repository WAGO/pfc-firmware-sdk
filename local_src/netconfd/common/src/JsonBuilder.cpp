// SPDX-License-Identifier: GPL-2.0-or-later

#include "JsonBuilder.hpp"
#include "Types.hpp"
#include "JsonConverter.hpp"
#include "JsonHelper.hpp"

namespace netconf {

using ::std::string;
using namespace ::std::string_literals;

template<>
JsonBuilder& JsonBuilder::Append<InterfaceConfigs>(const string& key, const InterfaceConfigs& configs, const FirmwareVersion& target_version) {

  json_[key] = InterfaceConfigsToNJson(configs, target_version);
  return *this;
}

template<>
JsonBuilder& JsonBuilder::Append<IPConfigs>(const string& key, const IPConfigs& configs, const FirmwareVersion& target_version) {
  (void) target_version;
  json_[key] = IPConfigsToNJson(configs);
  return *this;
}

template<>
JsonBuilder& JsonBuilder::Append<BridgeConfig>(const string& key, const BridgeConfig& configs, const FirmwareVersion& target_version) {
  json_[key] = BridgeConfigToNJson(configs, target_version);
  return *this;
}

template<>
JsonBuilder& JsonBuilder::Append<DipSwitchIpConfig>(const string& key, const DipSwitchIpConfig& config, const FirmwareVersion& target_version) {
  (void) target_version;
  json_[key] = DipSwitchIpConfigToNJson(config);
  return *this;
}

string JsonBuilder::ToString(JsonFormat format) {
  return json_.dump(format == JsonFormat::COMPACT ? JSON_DUMP : JSON_PRETTY_DUMP);
}

}
