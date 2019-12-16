// SPDX-License-Identifier: GPL-2.0-or-later

#include "JsonBuilder.hpp"
#include "JsonConfigConverter.hpp"
#include "Types.hpp"

namespace netconfd {

using ::std::string;
using namespace ::std::string_literals;

template<>
JsonBuilder& JsonBuilder::Append<InterfaceConfigs>(const string& key, const InterfaceConfigs& configs) {
  JsonConfigConverter converter;
  json_[key] = converter.InterfaceConfigsToNJson(configs);
  return *this;
}

template<>
JsonBuilder& JsonBuilder::Append<IPConfigs>(const string& key, const IPConfigs& configs) {
  JsonConfigConverter converter;
  json_[key] = converter.IPConfigsToNJson(configs);
  return *this;
}

template<>
JsonBuilder& JsonBuilder::Append<BridgeConfig>(const string& key, const BridgeConfig& configs) {
  JsonConfigConverter converter;
  json_[key] = converter.BridgeConfigToNJson(configs);
  return *this;
}

string JsonBuilder::ToString() {
  return json_.dump();
}

}
