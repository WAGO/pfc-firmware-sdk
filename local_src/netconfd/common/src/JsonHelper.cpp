/*
 * JsonConverterBase.cpp
 *
 *  Created on: 20.05.2020
 *      Author: u016903
 */

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
#include "JsonHelper.hpp"

namespace netconf {

using namespace ::std::literals;
using nlohmann::json;

using boost_address = boost::asio::ip::address;
using boost_error = boost::system::error_code;

constexpr auto PORT_CFG_KEY_DEVICE = "device";
constexpr auto PORT_CFG_KEY_STATE = "state";
constexpr auto PORT_CFG_KEY_AUTONEG = "autonegotiation";
constexpr auto PORT_CFG_KEY_SPEED = "speed";
constexpr auto PORT_CFG_KEY_DUPLEX = "duplex";

JsonKeyList interface_config_keys_ { PORT_CFG_KEY_DEVICE, PORT_CFG_KEY_STATE, PORT_CFG_KEY_AUTONEG, PORT_CFG_KEY_SPEED,
    PORT_CFG_KEY_DUPLEX };

json DipSwitchIpConfigToNJson(const DipSwitchIpConfig &ip_config) {
  return json { { "ipaddr", ip_config.address_ }, { "netmask", ip_config.netmask_ } };
}

json IpConfigToNJson(const IPConfig &ip_config) {
  json json_ip_config = json { { "ipaddr", ip_config.address_ }, { "netmask", ip_config.netmask_ }, { "bcast",
      GetBroadcast(ip_config.address_, ip_config.netmask_) } };
  to_json(json_ip_config["source"], ip_config.source_);
  return json_ip_config;
}

json IPConfigsToNJson(const IPConfigs &ip_configs) {
  json json_out( { });
  for (const auto &ip_config : ip_configs) {
    json_out[ip_config.interface_] = IpConfigToNJson(ip_config);
  }
  return json_out;
}

json BridgeConfigToNJson(const BridgeConfig &bridge_config, const FirmwareVersion &target_fw) {
  auto j = json(bridge_config);
  if (target_fw.GetFirmwareIndex() == 15) {
    /* FW 15 needs br1 in bridge config in order to work cerrectly */
    if (not j.contains("br1")) {
      j["br1"] = json::array();
    }
  }
  return j;
}

::std::string DipSwitchIPConfigToJson(const DipSwitchIpConfig &config) {
  return DipSwitchIpConfigToNJson(config).dump();
}

json DipSwitchConfigToNJson(const DipSwitchConfig &config) {
  return json { { "ipaddr", config.ip_config_.address_ }, { "netmask", config.ip_config_.netmask_ }, { "value", config
      .value_ }, { "mode", config.mode_ } };
}

::std::string DipSwitchConfigToJson(const DipSwitchConfig &config) {
  return DipSwitchConfigToNJson(config).dump();
}

Status JsonToNJson(::std::string const &json_str, json &json_object) {

  Status status;

  try {

    class throwing_sax : public nlohmann::detail::json_sax_dom_parser<nlohmann::json> {
     public:
      explicit throwing_sax(nlohmann::json &j)
          :
          nlohmann::detail::json_sax_dom_parser<nlohmann::json>(j) {
      }
      ;

      bool key(json::string_t &val) {
        if (ref_stack.back()->contains(val)) {
          throw std::invalid_argument("key " + val + " was already stored");
        }
        return nlohmann::detail::json_sax_dom_parser<nlohmann::json>::key(val);
      }
    };
    throwing_sax sax_consumer(json_object);
    json::sax_parse(json_str, &sax_consumer);

  } catch (std::exception const &e) {
    status.Prepend(StatusCode::JSON_CONVERT_ERROR, "Json parse error " + std::string(e.what()));
  } catch (...) {
    status.Prepend(StatusCode::JSON_CONVERT_ERROR, "Json parse error unexpected exception");
  }

  return status;

}

::std::string InterfacesToJson(const Interfaces &interfaces) {

  ::std::string json_string = "["s;
  if (!interfaces.empty()) {

    for (uint32_t i = 0; i < interfaces.size() - 1; i++) {
      json_string += "\"" + interfaces[i] + "\",";
    }
    json_string += "\"" + interfaces[interfaces.size() - 1] + "\"";

  }

  json_string += "]";
  return json_string;
}

Status GetAddressFromJson(const std::string &json_field, const json &from, std::string &to) {
  Status status;

  try {
    auto iter = from.find(json_field);
    if (iter != from.end()) {
      iter.value().get_to(to);

      boost_error error_code;
      boost_address::from_string(to, error_code);
      if (error_code) {
        status.Append(StatusCode::JSON_CONVERT_ERROR, "Found invalid IP address.");
        to = "";
      }
    }

  } catch (std::exception const &e) {
    to = "";
    status.Prepend(StatusCode::JSON_CONVERT_ERROR, "Convert json to IP address error: " + std::string(e.what()));
  } catch (...) {
    to = "";
    status.Prepend(StatusCode::JSON_CONVERT_ERROR, "Convert json to IP address error unexpected exception");
  }

  return status;

}

Status NJsonToBridgeConfig(const json &json_object, BridgeConfig &bridge_config) {
  Status status;

  for (const auto &bridge_cfg_json : json_object.items()) {
    auto bridge_cfg_pair = std::make_pair<Bridge, Interfaces>(bridge_cfg_json.key(), bridge_cfg_json.value());  // NOLINT: Need to express types in make_pair to get it working.

    if (bridge_cfg_pair.first.empty()) {
      status.Append(StatusCode::JSON_CONVERT_ERROR, "Bridge Name is empty");
    } else if (bridge_config.count(bridge_cfg_pair.first) > 0) {
      status.Append(StatusCode::JSON_CONVERT_ERROR, "Bridge Name duplicate!");
    }

    if (status.NotOk()) {
      bridge_config.clear();
      break;
    }
    bridge_config.insert(bridge_cfg_pair);
  }

  return status;
}

Status NJsonToDipIPConfig(const nlohmann::json &json_object, DipSwitchIpConfig &ip_config) {
  Status status;

  status = GetAddressFromJson("ipaddr", json_object, ip_config.address_);

  if (status.Ok()) {
    status = GetAddressFromJson("netmask", json_object, ip_config.netmask_);
  }

  return status;
}

Status JsonToDipSwitchConfig(const ::std::string &json_string, DipSwitchConfig &out_obj) {
  json json_object;
  auto status = JsonToNJson(json_string, json_object);
  if (status.Ok()) {
    status = get_to_or_error("mode", json_object, out_obj.mode_);
  }
  if (status.Ok()) {
    status = get_to_or_error("value", json_object, out_obj.value_);
  }
  if (status.Ok()) {
    status = get_to_or_error("ipaddr", json_object, out_obj.ip_config_.address_);
  }
  if (status.Ok()) {
    status = get_to_or_error("netmask", json_object, out_obj.ip_config_.netmask_);
  }

  return status;
}

Status NJsonToIPConfigs(const json &json_object, IPConfigs &ip_configs) {

  Status status;

  try {
    auto items = json_object.items();
    ::std::transform(items.begin(), items.end(), ::std::back_inserter(ip_configs), parse_ip_config);
  } catch (std::exception const &e) {
    ip_configs.clear();
    status.Prepend(StatusCode::JSON_CONVERT_ERROR, "Convert json to Ip config error: " + std::string(e.what()));
  } catch (...) {
    ip_configs.clear();
    status.Prepend(StatusCode::JSON_CONVERT_ERROR, "Convert json to Ip config error unexpected exception");
  }

  return status;
}

json InterfaceConfigToNJson(const InterfaceConfig &interface_config) {
  json j;

  j[PORT_CFG_KEY_DEVICE] = interface_config.device_name_;

  if (interface_config.state_ != InterfaceState::UNKNOWN) {
    j[PORT_CFG_KEY_STATE] = interface_config.state_;
  }

  if (interface_config.autoneg_ != Autonegotiation::UNKNOWN) {
    j[PORT_CFG_KEY_AUTONEG] = interface_config.autoneg_;
  }

  if (interface_config.speed_ > 0) {
    j[PORT_CFG_KEY_SPEED] = interface_config.speed_;
  }

  if (interface_config.duplex_ != Duplex::UNKNOWN) {
    j[PORT_CFG_KEY_DUPLEX] = interface_config.duplex_;
  }
  return j;
}

static json InterfaceConfigToNJson_fw15(const InterfaceConfig &interface_config) {
  json j;

  j[PORT_CFG_KEY_DEVICE] = interface_config.device_name_;

  if (interface_config.state_ != InterfaceState::UNKNOWN) {
    j[PORT_CFG_KEY_STATE] = interface_config.state_;
  }

  if (interface_config.autoneg_ != Autonegotiation::UNKNOWN) {
    j[PORT_CFG_KEY_AUTONEG] = (interface_config.autoneg_ == Autonegotiation::ON) ? true : false;
  }

  if (interface_config.speed_ > 0) {
    j[PORT_CFG_KEY_SPEED] = interface_config.speed_;
  }

  if (interface_config.duplex_ != Duplex::UNKNOWN) {
    j[PORT_CFG_KEY_DUPLEX] = interface_config.duplex_;
  }
  return j;
}

json InterfaceConfigsToNJson(const InterfaceConfigs &interface_configs, const FirmwareVersion &target_fw) {

  /* We changed the interface config json format in fw 16, so we need this ugly hack to generate a different config */

  auto interfaceConfigToNJsonFn =
      (target_fw.GetFirmwareIndex() == 15) ? &InterfaceConfigToNJson_fw15 : &InterfaceConfigToNJson;

  if (interface_configs.size() > 1) {
    nlohmann::json jarray { };
    for (const auto &config : interface_configs) {
      jarray.push_back(interfaceConfigToNJsonFn(config));
    }
    return jarray;
  }

  return interface_configs.size() == 1 ? InterfaceConfigToNJson(interface_configs.at(0)) : nlohmann::json( { });
}

InterfaceConfig InterfaceConfigFromJson(const json &config) {

  InterfaceConfig p { config.at(PORT_CFG_KEY_DEVICE).get<::std::string>() };

  auto autoneg_opt = config.find(PORT_CFG_KEY_AUTONEG) != config.end();
  if (autoneg_opt) {
    p.autoneg_ = config.at(PORT_CFG_KEY_AUTONEG).get<Autonegotiation>();
  }

  auto duplex_opt = config.find(PORT_CFG_KEY_DUPLEX) != config.end();
  if (duplex_opt) {
    p.duplex_ = config.at(PORT_CFG_KEY_DUPLEX).get<Duplex>();
  }

  auto state_opt = config.find(PORT_CFG_KEY_STATE) != config.end();
  if (state_opt) {
    p.state_ = config.at(PORT_CFG_KEY_STATE).get<InterfaceState>();
  }

  auto speed_opt = config.find(PORT_CFG_KEY_SPEED) != config.end();
  if (speed_opt) {
    p.speed_ = config.at(PORT_CFG_KEY_SPEED).get<int>();
  }
  return p;
}

Status NJsonToInterfaceConfigs(const nlohmann::json &json_object, InterfaceConfigs &interface_configs) {
  if (json_object.empty()) {
    return Status { StatusCode::JSON_CONVERT_ERROR, "Empty JSON object" };
  }
  if (json_object.is_array()) {
    for (auto &jentry : json_object) {
      if (!interface_config_keys_.matchesJsonObject(jentry)) {
        return Status { StatusCode::JSON_CONVERT_ERROR, "Invalid JSON object" };
      }
      interface_configs.push_back(InterfaceConfigFromJson(jentry));
    }
  } else {
    if (!interface_config_keys_.matchesJsonObject(json_object)) {
      return Status { StatusCode::JSON_CONVERT_ERROR, "Invalid JSON object" };
    }
    interface_configs.push_back(InterfaceConfigFromJson(json_object));
  }
  return Status { StatusCode::OK };
}

json InterfaceInformationToNJson(const InterfaceInformation &obj) {
  return json { { "name", obj.GetInterfaceName() }, { "label", obj.GetInterfaceLabel() }, { "type", obj.GetType() }, {
      "ip-ro", obj.IsIpConfigurationReadonly() } };
}

Status NJsonToInterfaceInformation(const json &json_object, InterfaceInformation &interface_information) {
  ::std::string name;
  ::std::string label;
  DeviceType type = DeviceType::Other;
  bool ip_config_ro = true;
  Status status;
  if (status.Ok()) {
    status = get_to_or_error("name", json_object, name);
  }
  if (status.Ok()) {
    status = get_to_or_error("label", json_object, label);
  }
  if (status.Ok()) {
    status = get_to_or_error("type", json_object, type);
  }
  if (status.Ok()) {
    status = get_to_or_error("ip-ro", json_object, ip_config_ro);
  }

  if (status.Ok()) {
    interface_information = InterfaceInformation { name, label, type, ip_config_ro };
  }

  return status;
}

}
