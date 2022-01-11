// SPDX-License-Identifier: LGPL-3.0-or-later

#pragma once

#include <nlohmann/json.hpp>

#include "Status.hpp"
#include "Types.hpp"
#include "FirmwareVersion.hpp"

namespace netconf {

using nlohmann::json;

constexpr int JSON_PRETTY_DUMP = 2;
constexpr int JSON_DUMP = -1;

NLOHMANN_JSON_SERIALIZE_ENUM(Duplex, { {Duplex::UNKNOWN, nullptr}, {Duplex::HALF, "half"}, {Duplex::FULL, "full"} })

NLOHMANN_JSON_SERIALIZE_ENUM(Autonegotiation, { {Autonegotiation::UNKNOWN, nullptr}, {Autonegotiation::ON, "on"},
                             {Autonegotiation::OFF, "off"}, {Autonegotiation::ON, true}, {Autonegotiation::OFF,
                             false} })

NLOHMANN_JSON_SERIALIZE_ENUM(AutonegotiationSupported, { {AutonegotiationSupported::UNKNOWN, nullptr}, {AutonegotiationSupported::YES, true},
                             {AutonegotiationSupported::NO, false} })

NLOHMANN_JSON_SERIALIZE_ENUM(InterfaceState, { {InterfaceState::UNKNOWN, nullptr}, {InterfaceState::DOWN, "down"},
                             {InterfaceState::UP, "up"} })

NLOHMANN_JSON_SERIALIZE_ENUM(LinkState, { {LinkState::UNKNOWN, nullptr}, {LinkState::DOWN, "down"}, {LinkState::UP,
                             "up"} })

NLOHMANN_JSON_SERIALIZE_ENUM(IPSource, { {IPSource::UNKNOWN, nullptr }, {IPSource::NONE, "none" }, {IPSource::STATIC,
                             "static" }, {IPSource::DHCP, "dhcp" }, {IPSource::BOOTP, "bootp" }, {IPSource::TEMPORARY,
                             "temporary"}, {IPSource::EXTERNAL, "external"}, {IPSource::FIXIP, "fixip"}});

NLOHMANN_JSON_SERIALIZE_ENUM(DipSwitchMode, { {DipSwitchMode::OFF, "off"}, {DipSwitchMode::STATIC, "static"},
                             {DipSwitchMode::DHCP, "dhcp"}, {DipSwitchMode::HW_NOT_AVAILABLE, "hw-not-available"} });

NLOHMANN_JSON_SERIALIZE_ENUM(DeviceType, {{DeviceType::Ethernet, "ethernet" }, {DeviceType::Bridge, "bridge" },
                             {DeviceType::Virtual, "virtual" }, {DeviceType::Wwan, "wwan" }, {DeviceType::Loopback,
                             "loopback" }, {DeviceType::Port, "port" }, {DeviceType::Service, "service" },
                             {DeviceType::Other, "other" } });

static Status MissingJsonKeyError(const std::string &keyname) {
  return Status { StatusCode::JSON_KEY_MISSING, keyname };
}

static auto get_to_if_exists = [](const ::std::string &key, const nlohmann::json &inner_json, auto &to) -> bool {
  auto iter = inner_json.find(key);
  if (iter != inner_json.end()) {
    iter.value().get_to(to);
    return true;
  }
  return false;
};

static auto parse_ip_config = [](const auto &json_iter) -> IPConfig {
  IPConfig c;
  c.interface_ = json_iter.key();
  auto inner = json_iter.value();

  get_to_if_exists("source", inner, c.source_);
  get_to_if_exists("ipaddr", inner, c.address_);
  get_to_if_exists("netmask", inner, c.netmask_);
  return c;
};

static auto get_to_or_error = [](const ::std::string &key, const nlohmann::json &inner_json, auto &to) -> Status {
  auto iter = inner_json.find(key);
  if (iter != inner_json.end()) {
    iter.value().get_to(to);
    return Status::Ok();
  }
  return MissingJsonKeyError(key);
};

Status JsonToNJson(::std::string const &json_str, json &json_object);
json DipSwitchIpConfigToNJson(const DipSwitchIpConfig &ip_config);
json DipSwitchConfigToNJson(const DipSwitchConfig &config);
json IpConfigToNJson(const IPConfig &ip_config);
json IPConfigsToNJson(const IPConfigs &ip_configs);
json BridgeConfigToNJson(const BridgeConfig &bridge_config, const FirmwareVersion &target_fw = FirmwareVersion { });
json InterfaceConfigToNJson(const InterfaceConfig &interface_config);
json InterfaceConfigsToNJson(const InterfaceConfigs &interface_configs, const FirmwareVersion &target_fw =
                                 FirmwareVersion { });
json InterfaceStatusToNJson(const InterfaceStatus &interface_status);
json InterfaceStatusesToNJson(const InterfaceStatuses &interface_status);
json InterfaceInformationToNJson(const InterfaceInformation &obj);

::std::string DipSwitchIPConfigToJson(const DipSwitchIpConfig &config);
::std::string DipSwitchConfigToJson(const DipSwitchConfig &config);
::std::string InterfacesToJson(const Interfaces &interfaces);

Status JsonToDipSwitchConfig(const ::std::string &json_string, DipSwitchConfig &out_obj);

Status NJsonToBridgeConfig(const json &json_object, BridgeConfig &bridge_config);
Status NJsonToDipIPConfig(const json &json_object, DipSwitchIpConfig &ip_config);
Status NJsonToIPConfigs(const json &json_object, IPConfigs &ip_configs);
Status NJsonToInterfaceConfigs(const json &json_object, InterfaceConfigs &interface_configs);
Status NJsonToInterfaceStatuses(const nlohmann::json &json_object, InterfaceStatuses &interface_statuses);
Status NJsonToInterfaceInformation(const json &json_object, InterfaceInformation &interface_information);

Status GetAddressFromJson(const std::string &json_field, const json &from, std::string &to);
InterfaceConfig InterfaceConfigFromJson(const json &config);
InterfaceStatus InterfaceStatusFromJson(const json &config);

}
