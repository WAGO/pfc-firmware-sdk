// SPDX-License-Identifier: GPL-2.0-or-later
#include "JsonRestorer.hpp"

#include <functional>
#include <string>

#include "JsonConfigConverter.hpp"
#include <nlohmann/json.hpp>
#include "Status.hpp"
#include "Types.hpp"

namespace netconfd {

using namespace ::std::string_literals;
template<>
Status JsonRestorer::Restore<InterfaceConfigs>(const ::std::string& key, InterfaceConfigs& interface_configs) {
  JsonConfigConverter converter;
  return Restore(
      key,
      ::std::bind(&JsonConfigConverter::NJsonToInterfaceConfigs, converter, ::std::placeholders::_1,
                  ::std::ref(interface_configs)));
}

template<>
Status JsonRestorer::Restore<BridgeConfig>(const ::std::string& key, BridgeConfig& bridge_configs) {
  JsonConfigConverter converter;
  return Restore(
      key,
      ::std::bind(&JsonConfigConverter::NJsonToBridgeConfig, converter, ::std::placeholders::_1,
                  ::std::ref(bridge_configs)));
}

template<>
Status JsonRestorer::Restore<IPConfigs>(const ::std::string& key, IPConfigs& ip_configs) {
  JsonConfigConverter converter;
  return Restore(
      key,
      ::std::bind(&JsonConfigConverter::NJsonToIPConfigs, converter, ::std::placeholders::_1, ::std::ref(ip_configs)));
}

Status JsonRestorer::Restore(const ::std::string& key, const ::std::function<Status(const ::nlohmann::json&)>& convert) {
  Status status;
  JsonConfigConverter converter;
  ::nlohmann::json jo;

  status = converter.JsonToNJson(backup_, jo);
  if (status.NotOk()) {
    return status;
  }

  try {
    status = convert(jo.at(key));
  } catch (::std::exception& e) {
    status.Append(StatusCode::JSON_CONVERT_ERROR,
                  "Failed to restore interface configuration of key "s + key + ": "s + e.what());
  }

  return status;
}

} /* namespace netconfd */
