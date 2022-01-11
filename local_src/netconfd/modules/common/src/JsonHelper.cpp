// SPDX-License-Identifier: LGPL-3.0-or-later

#include "JsonConverter.hpp"
#include <nlohmann/json.hpp>
#include <boost/asio/ip/address.hpp>
#include <boost/asio/ip/network_v4.hpp>
#include <boost/system/error_code.hpp>
#include "NetworkHelper.hpp"
#include "TypesHelper.hpp"
#include "Types.hpp"
#include "Helper.hpp"
#include "JsonKeyList.hpp"
#include "JsonHelper.hpp"
#include "JsonHelper.hpp"
#include "Status.hpp"

namespace netconf {

using namespace ::std::literals;
using nlohmann::json;

using boost_address = boost::asio::ip::address;
using boost_error = boost::system::error_code;

constexpr auto PORT_CFG_KEY_DEVICE = "device";
constexpr auto PORT_CFG_KEY_STATE = "state";
constexpr auto PORT_CFG_KEY_AUTONEG = "autonegotiation";
constexpr auto PORT_CFG_KEY_AUTONEG_SUPPORTED = "autonegsupported";
constexpr auto PORT_CFG_KEY_SPEED = "speed";
constexpr auto PORT_CFG_KEY_DUPLEX = "duplex";
constexpr auto PORT_CFG_KEY_LINKSTATE = "link";
constexpr auto PORT_CFG_KEY_MAC = "mac";
constexpr auto PORT_CFG_KEY_SUPPORTED_LINK_MODES = "supportedlinkmodes";


JsonKeyList interface_config_keys_ { PORT_CFG_KEY_DEVICE, PORT_CFG_KEY_STATE, PORT_CFG_KEY_AUTONEG, PORT_CFG_KEY_AUTONEG_SUPPORTED, PORT_CFG_KEY_SPEED,
    PORT_CFG_KEY_DUPLEX, PORT_CFG_KEY_LINKSTATE, PORT_CFG_KEY_MAC };

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

  try {

    class throwing_sax : public nlohmann::detail::json_sax_dom_parser<nlohmann::json> {
     public:
      explicit throwing_sax(nlohmann::json &j)
          : nlohmann::detail::json_sax_dom_parser<nlohmann::json>(j) {
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
    return Status { StatusCode::JSON_CONVERT, e.what() };
  }
  return Status::Ok();

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

  try {
    auto iter = from.find(json_field);
    if (iter != from.end()) {
      iter.value().get_to(to);

      boost_error error_code;
      boost_address::from_string(to, error_code);
      if (error_code) {
        return Status { StatusCode::IPV4_FORMAT, ::std::to_string(error_code.value()) };
      }
    }

  } catch (std::exception const &e) {
    to.clear();
    return Status { StatusCode::JSON_CONVERT, e.what() };
  } catch (...) {
    to.clear();
    return Status { StatusCode::JSON_CONVERT, "General JSON error" };
  }

  return Status::Ok();

}

Status NJsonToBridgeConfig(const json &json_object, BridgeConfig &bridge_config) {
  Status status;

  for (const auto &bridge_cfg_json : json_object.items()) {
    auto bridge_cfg_pair = std::make_pair<Bridge, Interfaces>(bridge_cfg_json.key(), bridge_cfg_json.value());  // NOLINT: Need to express types in make_pair to get it working.

    if (bridge_cfg_pair.first.empty()) {
      status.Set(StatusCode::NAME_EMPTY);
    } else if (bridge_config.count(bridge_cfg_pair.first) > 0) {
      status.Set(StatusCode::ENTRY_DUPLICATE, bridge_cfg_pair.first);
    }

    if (status.IsNotOk()) {
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

  if (status.IsOk()) {
    status = GetAddressFromJson("netmask", json_object, ip_config.netmask_);
  }

  return status;
}

Status JsonToDipSwitchConfig(const ::std::string &json_string, DipSwitchConfig &out_obj) {
  json json_object;
  auto status = JsonToNJson(json_string, json_object);
  if (status.IsOk()) {
    status = get_to_or_error("mode", json_object, out_obj.mode_);
  }
  if (status.IsOk()) {
    status = get_to_or_error("value", json_object, out_obj.value_);
  }
  if (status.IsOk()) {
    status = get_to_or_error("ipaddr", json_object, out_obj.ip_config_.address_);
  }
  if (status.IsOk()) {
    status = get_to_or_error("netmask", json_object, out_obj.ip_config_.netmask_);
  }

  return status;
}

Status NJsonToIPConfigs(const json &json_object, IPConfigs &ip_configs) {

  try {
    auto items = json_object.items();
    ::std::transform(items.begin(), items.end(), ::std::back_inserter(ip_configs), parse_ip_config);
  } catch (std::exception const &e) {
    return Status { StatusCode::JSON_CONVERT, e.what() };
  } catch (...) {
    return Status { StatusCode::JSON_CONVERT, "General JSON error" };
  }

  return Status::Ok();
}

static json InterfaceBaseToNJson(const InterfaceBase &interface_config) {
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

json InterfaceConfigToNJson(const InterfaceConfig &interface_config) {
  return InterfaceBaseToNJson(interface_config);
}

json InterfaceStatusToNJson(const InterfaceStatus &interface_status) {

  json j = InterfaceBaseToNJson(interface_status);

  if (interface_status.link_state_ != LinkState::UNKNOWN) {
    j[PORT_CFG_KEY_LINKSTATE] = interface_status.link_state_;
  }

  j[PORT_CFG_KEY_MAC] = interface_status.mac_.ToString();

  return j;
}

static json InterfaceConfigToNJson_fw15(const InterfaceConfig &interface_config) {
  json j;

  j[PORT_CFG_KEY_DEVICE] = interface_config.device_name_;

  if (interface_config.state_ != InterfaceState::UNKNOWN) {
    j[PORT_CFG_KEY_STATE] = interface_config.state_;
  }

  if (interface_config.autoneg_ != Autonegotiation::UNKNOWN) {
    j[PORT_CFG_KEY_AUTONEG] = (interface_config.autoneg_ == Autonegotiation::ON);
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

json InterfaceStatusesToNJson(const InterfaceStatuses &interface_status) {

  if (interface_status.size() > 1) {
    nlohmann::json jarray { };
    for (const auto &config : interface_status) {
      jarray.push_back(InterfaceStatusToNJson(config));
    }
    return jarray;
  }

  return interface_status.size() == 1 ? InterfaceStatusToNJson(interface_status.at(0)) : nlohmann::json( { });
}

static InterfaceBase InterfaceBaseFromJson(const json &config) {
  InterfaceBase p { config.at(PORT_CFG_KEY_DEVICE).get<::std::string>() };

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

InterfaceConfig InterfaceConfigFromJson(const json &config) {

  InterfaceConfig p { ::std::move(InterfaceBaseFromJson(config)) };
  return p;
}

InterfaceStatus InterfaceStatusFromJson(const json &config) {

  InterfaceStatus p { ::std::move(InterfaceBaseFromJson(config)) };

  auto link_opt = config.find(PORT_CFG_KEY_LINKSTATE) != config.end();
  if (link_opt) {
    p.link_state_ = config.at(PORT_CFG_KEY_LINKSTATE).get<LinkState>();
  }

  auto mac_opt = config.find(PORT_CFG_KEY_MAC) != config.end();
  if (mac_opt) {
    auto mac = config.at(PORT_CFG_KEY_MAC).get<::std::string>();
    p.mac_ = MacAddress::FromString(mac);
  }

  return p;
}

Status NJsonToInterfaceConfigs(const nlohmann::json &json_object, InterfaceConfigs &interface_configs) {
  if (json_object.empty()) {
    return Status { StatusCode::JSON_INCOMPLETE };
  }
  if (json_object.is_array()) {
    for (auto &jentry : json_object) {
      if (!interface_config_keys_.matchesJsonObject(jentry)) {
        return Status { StatusCode::JSON_INCOMPLETE };
      }
      interface_configs.push_back(InterfaceConfigFromJson(jentry));
    }
  } else {
    if (!interface_config_keys_.matchesJsonObject(json_object)) {
      return Status { StatusCode::JSON_INCOMPLETE };
    }
    interface_configs.push_back(InterfaceConfigFromJson(json_object));
  }
  return Status::Ok();
}

Status NJsonToInterfaceStatuses(const nlohmann::json &json_object, InterfaceStatuses &interface_statuses) {
  if (json_object.empty()) {
    return Status { StatusCode::JSON_INCOMPLETE };
  }
  if (json_object.is_array()) {
    for (auto &jentry : json_object) {
      if (!interface_config_keys_.matchesJsonObject(jentry)) {
        return Status { StatusCode::JSON_INCOMPLETE };
      }
      interface_statuses.push_back(InterfaceStatusFromJson(jentry));
    }
  } else {
    if (!interface_config_keys_.matchesJsonObject(json_object)) {
      return Status { StatusCode::JSON_INCOMPLETE };
    }
    interface_statuses.push_back(InterfaceStatusFromJson(json_object));
  }
  return Status::Ok();
}

static json LinkModesToNJson(const LinkModes &link_modes) {
  json array;
  for (auto link_mode : link_modes) {

    json j;
    if (link_mode.speed_ > 0) {
      j[PORT_CFG_KEY_SPEED] = link_mode.speed_;
    }
    if (link_mode.duplex_ != Duplex::UNKNOWN) {
      j[PORT_CFG_KEY_DUPLEX] = link_mode.duplex_;
    }

    array.push_back(j);
  }
  return array;
}

json InterfaceInformationToNJson(const InterfaceInformation &obj) {
  json j { { "name", obj.GetInterfaceName() }, { "label", obj.GetInterfaceLabel() }, { "type", obj.GetType() }, {
      "ipreadonly", obj.IsIpConfigurationReadonly() } };

  if(obj.GetAutonegSupported() != AutonegotiationSupported::UNKNOWN){
    j[PORT_CFG_KEY_AUTONEG_SUPPORTED] = obj.GetAutonegSupported();
  }

  auto link_modes = obj.GetSupportedlinkModes();
  if (!link_modes.empty()) {
    j[PORT_CFG_KEY_SUPPORTED_LINK_MODES] = LinkModesToNJson(link_modes);
  }
  return j;
}

static void parse_link_modes(const json &json_object, LinkModes &link_modes) {

  if (json_object.find(PORT_CFG_KEY_SUPPORTED_LINK_MODES) != json_object.end()) {
    for (auto &link_mode : json_object[PORT_CFG_KEY_SUPPORTED_LINK_MODES]) {

      LinkMode lm;
      auto duplex_opt = link_mode.find(PORT_CFG_KEY_DUPLEX) != link_mode.end();
      if (duplex_opt) {
        lm.duplex_ = link_mode.at(PORT_CFG_KEY_DUPLEX).get<Duplex>();
      }

      auto speed_opt = link_mode.find(PORT_CFG_KEY_SPEED) != link_mode.end();
      if (speed_opt) {
        lm.speed_ = link_mode.at(PORT_CFG_KEY_SPEED).get<int>();
      }

      link_modes.emplace_back(lm);
    }
  }

}

Status NJsonToInterfaceInformation(const json &json_object, InterfaceInformation &interface_information) {
  ::std::string name;
  ::std::string label;
  DeviceType type = DeviceType::Other;
  bool ip_config_ro = true;
  AutonegotiationSupported autoneg_supported = AutonegotiationSupported::UNKNOWN;
  LinkModes link_modes { };
  Status status = get_to_or_error("name", json_object, name);
  if (status.IsOk()) {
    status = get_to_or_error("label", json_object, label);
  }
  if (status.IsOk()) {
    status = get_to_or_error("type", json_object, type);
  }
  if (status.IsOk()) {
    status = get_to_or_error("ipreadonly", json_object, ip_config_ro);
  }
  if (status.IsOk()) {
    get_to_if_exists(PORT_CFG_KEY_AUTONEG_SUPPORTED, json_object, autoneg_supported);
  }
  if (status.IsOk()) {
    parse_link_modes(json_object, link_modes);
  }

  if (status.IsOk()) {
    interface_information = InterfaceInformation { name, label, type, ip_config_ro, autoneg_supported, link_modes };
  }

  return status;
}

}
