// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include <boost/foreach.hpp>
#include <nlohmann/json_fwd.hpp>

#include "Types.hpp"
#include "Status.hpp"
#include "IJsonConfigConverter.hpp"
#include "IBridgeConfigurator.hpp"
#include "JsonKeyList.hpp"
#include "JsonBuilder.hpp"


namespace netconfd {

class JsonConfigConverter : public IJsonConfigConverter {
  friend class JsonBuilder;
  friend class JsonRestorer;
 public:

  JsonConfigConverter();
  ~JsonConfigConverter() override = default;
  JsonConfigConverter(const JsonConfigConverter&) = default;
  JsonConfigConverter& operator=(const JsonConfigConverter&) = delete;
  JsonConfigConverter(JsonConfigConverter&&) = default;
  JsonConfigConverter& operator=(JsonConfigConverter&&) = delete;

  Status JsonToBridgeConfig(const ::std::string& json, BridgeConfig& bridge_config) const override;
  Status JsonToIPConfigs(const ::std::string& json, IPConfigs& ip_configs) const override;
  Status BridgeConfigToJson(const BridgeConfig& bridge_config, ::std::string& json) const override;
  ::std::string IPConfigsToJson(const IPConfigs& ip_configs) const override;
  ::std::string DipSwitchIPConfigToJson(const DipSwitchIpConfig& config) const override;
  ::std::string DipSwitchConfigToJson(const DipSwitchConfig& config) const override;
  Status JsonToDipSwitchIPConfig(const ::std::string& json, DipSwitchIpConfig& config) const override;
  ::std::string InterfacesToJson(const Interfaces& interfaces) const override;
  ::std::string InterfaceConfigToJson(const InterfaceConfigs& port_configs) const override;
  Status InterfaceConfigFromJson(const ::std::string& json, InterfaceConfigs& interface_configs) const override;

 protected:
  Status NJsonToBridgeConfig(const nlohmann::json& json_object, BridgeConfig& bridge_config) const;
  Status NJsonToDipIPConfig(const nlohmann::json& json_object, DipSwitchIpConfig& ip_config) const;
  Status NJsonToIPConfigs(const nlohmann::json& json_object, IPConfigs& ip_configs) const;
  Status NJsonToInterfaceConfigs(const nlohmann::json& json_object, InterfaceConfigs& interface_configs) const;
  nlohmann::json DipSwitchIpConfigToNJson(const DipSwitchIpConfig& ip_config) const;
  nlohmann::json DipSwitchConfigToNJson(const DipSwitchConfig& ip_config) const;
  nlohmann::json IpConfigToNJson(const IPConfig& ip_config) const;
  nlohmann::json IPConfigsToNJson(const IPConfigs& ip_configs) const;
  nlohmann::json BridgeConfigToNJson(const BridgeConfig& bridge_config) const;
  Status JsonToNJson(::std::string const& json_str, nlohmann::json& json) const;
  InterfaceConfig PortConfigFromJson(const nlohmann::json& config_pt) const;
  nlohmann::json InterfaceConfigsToNJson(const InterfaceConfigs& interface_configs) const;
  nlohmann::json PortConfigToNJson(const InterfaceConfig& interface_config) const;

  JsonKeyList interface_config_keys_;
};

}  // namespace netconfd
