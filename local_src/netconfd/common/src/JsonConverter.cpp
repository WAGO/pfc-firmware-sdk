// SPDX-License-Identifier: GPL-2.0-or-later

#include "JsonConverter.hpp"
#include <nlohmann/json.hpp>
#include <boost/asio/ip/address.hpp>
#include <boost/asio/ip/network_v4.hpp>
#include <boost/system/error_code.hpp>
#include "NetworkHelper.hpp"
#include "TypesHelper.hpp"
#include "Status.hpp"
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
    return json_config.dump(format == JsonFormat::COMPACT? JSON_DUMP : JSON_PRETTY_DUMP);
  } catch (...) {
    return std::string { };
  }
}

Status JsonConverter::FromJsonString(const ::std::string &str, netconf::DipSwitchConfig &out_obj) const noexcept {
  Status status;
  try {
    return JsonToDipSwitchConfig(str, out_obj);
  } catch (std::exception &e) {
    status.Append(StatusCode::JSON_CONVERT_ERROR, "Parse DipSwitchIPConfig failed! "s + e.what());
  }
  return status;
}

::std::string JsonConverter::ToJsonString(const netconf::DipSwitchIpConfig &obj, JsonFormat format) const noexcept {
  return json { { "ipaddr", obj.address_ }, { "netmask", obj.netmask_ } }.dump(format == JsonFormat::COMPACT? JSON_DUMP : JSON_PRETTY_DUMP);
}

Status JsonConverter::FromJsonString(const ::std::string &str, netconf::DipSwitchIpConfig &out_obj) const noexcept {
  json json_object;
  auto status = JsonToNJson(str, json_object);
  if (status.Ok()) {
    status = get_to_or_error("ipaddr", json_object, out_obj.address_);
  }
  if (status.Ok()) {
    status = get_to_or_error("netmask", json_object, out_obj.netmask_);
  }
  return status;
}

::std::string JsonConverter::ToJsonString(const BridgeConfig &obj, JsonFormat format) const noexcept {
  json bridge_config_json(obj);
  return bridge_config_json.dump(format == JsonFormat::COMPACT? JSON_DUMP : JSON_PRETTY_DUMP);
}

Status JsonConverter::FromJsonString(const ::std::string &str, BridgeConfig &out_obj) const noexcept {
  Status status;
  try {
    json whole_bridge_json;
    status = JsonToNJson(str, whole_bridge_json);
    if (status.Ok()) {
      status = NJsonToBridgeConfig(whole_bridge_json, out_obj);
    }
  } catch (std::exception &e) {
    out_obj.clear();
    status.Append(StatusCode::JSON_CONVERT_ERROR, "Parse BridgeConfig failed! "s + e.what());
  }

  return status;
}

::std::string JsonConverter::ToJsonString(const IPConfigs &obj, JsonFormat format) const noexcept {
  try {
    return IPConfigsToNJson(obj).dump(format == JsonFormat::COMPACT? JSON_DUMP : JSON_PRETTY_DUMP);
  } catch (std::exception &ex) {
    return ::std::string { };
  }
}

Status JsonConverter::FromJsonString(const ::std::string &str, IPConfigs &out_obj) const noexcept {
  Status status;
  try {
    nlohmann::json j;
    status = JsonToNJson(str, j);
    if (status.Ok()) {
      status = NJsonToIPConfigs(j, out_obj);
    }
  } catch (std::exception &e) {
    out_obj.clear();
    status.Append(StatusCode::JSON_CONVERT_ERROR, "Parse IpConfig failed! "s + e.what());
  }
  return status;
}

::std::string JsonConverter::ToJsonString(const InterfaceConfigs &obj, JsonFormat format) const noexcept {
  if (obj.size() > 1) {
    nlohmann::json jarray { };
    for (const auto &config : obj) {
      jarray.push_back(InterfaceConfigToNJson(config));
    }
    return jarray.dump(format == JsonFormat::COMPACT? JSON_DUMP : JSON_PRETTY_DUMP);
  }

  return (obj.size() == 1 ? InterfaceConfigToNJson(obj.at(0)) : nlohmann::json( { })).dump(format == JsonFormat::COMPACT? JSON_DUMP : JSON_PRETTY_DUMP);
}

Status JsonConverter::FromJsonString(const ::std::string &str, InterfaceConfigs &out_obj) const noexcept {
  json json_out;
  auto status = JsonToNJson(str, json_out);
  if (status.Ok()) {
    status = NJsonToInterfaceConfigs(json_out, out_obj);
  }
  return status;
}

::std::string JsonConverter::ToJsonString(const IPConfig &obj, JsonFormat format) const noexcept{
  return json{{obj.interface_, {{ "ipaddr", obj.address_ },
                               {"netmask", obj.netmask_},
                               {"source", obj.source_ }}}
  }.dump(format == JsonFormat::COMPACT? JSON_DUMP : JSON_PRETTY_DUMP);
}

Status JsonConverter::FromJsonString(const ::std::string &str, IPConfig &out_obj) const noexcept{
  json j;
  auto status = JsonToNJson(str, j);
  if(status.Ok()){
    out_obj = parse_ip_config(j.items().begin());
  }
  return status;
}

::std::string JsonConverter::ToJsonString(const InterfaceConfig &obj, JsonFormat format) const noexcept{
  return InterfaceConfigToNJson(obj).dump(format == JsonFormat::COMPACT? JSON_DUMP : JSON_PRETTY_DUMP);
}



Status JsonConverter::FromJsonString(const ::std::string &str, InterfaceConfig &out_obj) const noexcept{
  json j;
  auto status = JsonToNJson(str, j);
  if(status.Ok()){
   out_obj = InterfaceConfigFromJson(j);
  }
  return status;
}

::std::string JsonConverter::ToJsonString(const InterfaceInformation &obj, JsonFormat format) const noexcept {
  return InterfaceInformationToNJson(obj).dump(format == JsonFormat::COMPACT? JSON_DUMP : JSON_PRETTY_DUMP);
}

Status JsonConverter::FromJsonString(const ::std::string &str, InterfaceInformation &out_obj) const noexcept {
  json j;
  auto status = JsonToNJson(str, j);
  if (status.Ok()) {
    status = NJsonToInterfaceInformation(j, out_obj);
  }
  return status;
}

::std::string JsonConverter::ToJsonString(const InterfaceInformations &obj, JsonFormat format) const noexcept{
  auto j = json::array();
  for (const auto &o : obj) {
    j.push_back(InterfaceInformationToNJson(o));
  }
  return j.dump(format == JsonFormat::COMPACT? JSON_DUMP : JSON_PRETTY_DUMP);
}

Status JsonConverter::FromJsonString(const ::std::string &str, InterfaceInformations &out_obj) const noexcept{
  json j_array;
  auto status = JsonToNJson(str, j_array);
  if (j_array.is_array()) {
    for (const auto &j_item : j_array) {
      InterfaceInformation ii;
      status = NJsonToInterfaceInformation(j_item, ii);
      out_obj.push_back(ii);
    }
    if (status.NotOk()) {
      out_obj.clear();
    }
  } else {
    status.Append(StatusCode::JSON_CONVERT_ERROR, "Expected a json array");
  }

  return status;
}

}  // namespace netconf
