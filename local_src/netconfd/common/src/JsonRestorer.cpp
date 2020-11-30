// SPDX-License-Identifier: LGPL-3.0-or-later
#include "JsonRestorer.hpp"

#include <functional>
#include <string>

#include <nlohmann/json.hpp>

#include "Error.hpp"
#include "Types.hpp"

#include "JsonHelper.hpp"

namespace netconf {

using namespace ::std::string_literals;
template<>
Error JsonRestorer::Restore<InterfaceConfigs>(const ::std::string &key, InterfaceConfigs &interface_configs) {

  ::nlohmann::json jo;
  Error status = JsonToNJson(backup_, jo);
  if (status.IsOk()) {
    try {
      status = NJsonToInterfaceConfigs(jo.at(key), interface_configs);
    } catch (::std::exception &e) {
      return Error { ErrorCode::JSON_CONVERT, e.what() };
    }
  }
  return status;

}

template<>
Error JsonRestorer::Restore<BridgeConfig>(const ::std::string &key, BridgeConfig &bridge_configs) {

  ::nlohmann::json jo;
  Error status = JsonToNJson(backup_, jo);
  if (status.IsOk()) {
    try {
      status = NJsonToBridgeConfig(jo.at(key), bridge_configs);
    } catch (::std::exception &e) {
      return Error { ErrorCode::JSON_CONVERT, e.what() };
    }
  }
  return status;

}

template<>
Error JsonRestorer::Restore<IPConfigs>(const ::std::string &key, IPConfigs &ip_configs) {

  ::nlohmann::json jo;
  Error status = JsonToNJson(backup_, jo);
  if (status.IsOk()) {
    try {
      status = NJsonToIPConfigs(jo.at(key), ip_configs);
    } catch (::std::exception &e) {
      return Error { ErrorCode::JSON_CONVERT, e.what() };
    }
  }
  return status;
}

template<>
Error JsonRestorer::Restore<DipSwitchIpConfig>(const ::std::string &key, DipSwitchIpConfig &dip_switch_ip_config) {

  ::nlohmann::json jo;
  Error status = JsonToNJson(backup_, jo);
  if (status.IsOk()) {
    try {
      status = NJsonToDipIPConfig(jo.at(key), dip_switch_ip_config);
    } catch (::std::exception &e) {
      return Error { ErrorCode::JSON_CONVERT, e.what() };
    }
  }
  return status;
}

} /* namespace netconf */
