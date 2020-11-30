// SPDX-License-Identifier: LGPL-3.0-or-later

#include "JsonConverter.hpp"
#include <utility>
#include <nlohmann/json.hpp>
#include <boost/asio/ip/address.hpp>
#include <boost/asio/ip/network_v4.hpp>
#include <boost/system/error_code.hpp>

#include "Error.hpp"
#include "NetworkHelper.hpp"
#include "TypesHelper.hpp"
#include "Types.hpp"
#include "Helper.hpp"
#include "JsonKeyList.hpp"
#include "JsonHelper.hpp"

namespace netconf {

using namespace ::std::literals;

::std::string JsonConverter::ToJsonString(const netconf::DipSwitchConfig &obj, JsonFormat format) const noexcept {
  json json_config = json { { "ipaddr", obj.ip_config_.address_ }, { "netmask", obj.ip_config_.netmask_ }, { "value",
      obj.value_ } };
  to_json(json_config["mode"], obj.mode_);
  try {
    return json_config.dump(format == JsonFormat::COMPACT ? JSON_DUMP : JSON_PRETTY_DUMP);
  } catch (...) {
    return std::string { };
  }
}

Error JsonConverter::FromJsonString(const ::std::string &str, netconf::DipSwitchConfig &out_obj) const noexcept {
  try {
    return JsonToDipSwitchConfig(str, out_obj);
  } catch (std::exception &e) {
    return Error { ErrorCode::JSON_CONVERT, e.what() };
  }
  return Error::Ok();
}

::std::string JsonConverter::ToJsonString(const netconf::DipSwitchIpConfig &obj, JsonFormat format) const noexcept {
  return json { { "ipaddr", obj.address_ }, { "netmask", obj.netmask_ } }.dump(
      format == JsonFormat::COMPACT ? JSON_DUMP : JSON_PRETTY_DUMP);
}

Error JsonConverter::FromJsonString(const ::std::string &str, netconf::DipSwitchIpConfig &out_obj) const noexcept {
  json json_object;
  auto status = JsonToNJson(str, json_object);
  if (status.IsOk()) {
    status = get_to_or_error("ipaddr", json_object, out_obj.address_);
  }
  if (status.IsOk()) {
    status = get_to_or_error("netmask", json_object, out_obj.netmask_);
  }
  return status;
}

::std::string JsonConverter::ToJsonString(const BridgeConfig &obj, JsonFormat format) const noexcept {
  json bridge_config_json(obj);
  return bridge_config_json.dump(format == JsonFormat::COMPACT ? JSON_DUMP : JSON_PRETTY_DUMP);
}

Error JsonConverter::FromJsonString(const ::std::string &str, BridgeConfig &out_obj) const noexcept {
  try {
    json whole_bridge_json;
    Error error = JsonToNJson(str, whole_bridge_json);
    if (error.IsNotOk()) {
      return error;
    }
    return NJsonToBridgeConfig(whole_bridge_json, out_obj);
  } catch (std::exception &e) {
    out_obj.clear();
    return Error { ErrorCode::JSON_CONVERT, e.what() };
  }

}

::std::string JsonConverter::ToJsonString(const IPConfigs &obj, JsonFormat format) const noexcept {
  try {
    return IPConfigsToNJson(obj).dump(format == JsonFormat::COMPACT ? JSON_DUMP : JSON_PRETTY_DUMP);
  } catch (std::exception &ex) {
    return ::std::string { };
  }
}

Error JsonConverter::FromJsonString(const ::std::string &str, IPConfigs &out_obj) const noexcept {
  try {
    nlohmann::json j;
    auto error = JsonToNJson(str, j);
    if (error.IsNotOk()) {
      return error;
    }
    return NJsonToIPConfigs(j, out_obj);
  } catch (std::exception &e) {
    out_obj.clear();
    return Error { ErrorCode::JSON_CONVERT, e.what() };
  }
}

::std::string JsonConverter::ToJsonString(const InterfaceConfigs &obj, JsonFormat format) const noexcept {
  if (obj.size() > 1) {
    nlohmann::json jarray { };
    for (const auto &config : obj) {
      jarray.push_back(InterfaceConfigToNJson(config));
    }
    return jarray.dump(format == JsonFormat::COMPACT ? JSON_DUMP : JSON_PRETTY_DUMP);
  }

  return (obj.size() == 1 ? InterfaceConfigToNJson(obj.at(0)) : nlohmann::json( { })).dump(
      format == JsonFormat::COMPACT ? JSON_DUMP : JSON_PRETTY_DUMP);
}

Error JsonConverter::FromJsonString(const ::std::string &str, InterfaceConfigs &out_obj) const noexcept {
  json json_out;
  auto error = JsonToNJson(str, json_out);
  if (error.IsNotOk()) {
    return error;
  }
  return NJsonToInterfaceConfigs(json_out, out_obj);
}

::std::string JsonConverter::ToJsonString(const IPConfig &obj, JsonFormat format) const noexcept {
  return json {
      { obj.interface_, { { "ipaddr", obj.address_ }, { "netmask", obj.netmask_ }, { "source", obj.source_ } } } }.dump(
      format == JsonFormat::COMPACT ? JSON_DUMP : JSON_PRETTY_DUMP);
}

Error JsonConverter::FromJsonString(const ::std::string &str, IPConfig &out_obj) const noexcept {
  json j;
  auto error = JsonToNJson(str, j);
  if (error.IsNotOk()) {
    return error;
  }
  out_obj = parse_ip_config(j.items().begin());
  return Error::Ok();
}

::std::string JsonConverter::ToJsonString(const InterfaceConfig &obj, JsonFormat format) const noexcept {
  return InterfaceConfigToNJson(obj).dump(format == JsonFormat::COMPACT ? JSON_DUMP : JSON_PRETTY_DUMP);
}

Error JsonConverter::FromJsonString(const ::std::string &str, InterfaceConfig &out_obj) const noexcept {
  json j;
  auto error = JsonToNJson(str, j);
  if (error.IsNotOk()) {
    return error;
  }
  out_obj = InterfaceConfigFromJson(j);
  return Error::Ok();
}

::std::string JsonConverter::ToJsonString(const InterfaceInformation &obj, JsonFormat format) const noexcept {
  return InterfaceInformationToNJson(obj).dump(format == JsonFormat::COMPACT ? JSON_DUMP : JSON_PRETTY_DUMP);
}

Error JsonConverter::FromJsonString(const ::std::string &str, InterfaceInformation &out_obj) const noexcept {
  json j;
  auto error = JsonToNJson(str, j);
  if (error.IsNotOk()) {
    return error;
  }
  return NJsonToInterfaceInformation(j, out_obj);
}

::std::string JsonConverter::ToJsonString(const InterfaceInformations &obj, JsonFormat format) const noexcept {
  auto j = json::array();
  for (const auto &o : obj) {
    j.push_back(InterfaceInformationToNJson(o));
  }
  return j.dump(format == JsonFormat::COMPACT ? JSON_DUMP : JSON_PRETTY_DUMP);
}

Error JsonConverter::FromJsonString(const ::std::string &str, InterfaceInformations &out_obj) const noexcept {
  json j_array;
  auto error = JsonToNJson(str, j_array);
  if (error.IsNotOk()) {
    return error;
  }

  if (j_array.is_array()) {
    for (const auto &j_item : j_array) {
      InterfaceInformation ii;
      error = NJsonToInterfaceInformation(j_item, ii);
      if (error.IsNotOk()) {
        return error;
      }
      out_obj.push_back(ii);
    }
    if (error.IsNotOk()) {
      out_obj.clear();
    }
  } else {
    return Error{ErrorCode::JSON_CONVERT};
  }
  return Error::Ok();
}

::std::string JsonConverter::ToJsonString(const Error& obj, JsonFormat format)const noexcept{
  json j{};

  j["errorcode"] = static_cast<uint32_t>(obj.GetErrorCode());

  auto& params = obj.GetParameter();
  if(params.size() > 0)
  {
    j["parameter"] = params;
  }

  return j.dump(format == JsonFormat::COMPACT ? JSON_DUMP : JSON_PRETTY_DUMP);
}

Error JsonConverter::FromJsonString(const ::std::string& str, Error& out_obj)const noexcept {
  json j;
  auto error = JsonToNJson(str, j);
  if (error.IsNotOk()) {
    return error;
  }
  ErrorCode code;
  error = get_to_or_error("errorcode", j, code);
  if (error.IsNotOk()) {
      return error;
    }
  Error::Parameters params;
  get_to_if_exists("parameter", j, params);

  out_obj.Set(code, ::std::move(params));
  return error;
}

}  // namespace netconf
