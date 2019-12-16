// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include <string>

#include "Types.hpp"
#include "Status.hpp"

namespace netconfd {

using ::std::string;

class IJsonConfigConverter {

 public:
  IJsonConfigConverter() = default;
  virtual ~IJsonConfigConverter() = default;

  IJsonConfigConverter(const IJsonConfigConverter&) = default;
  IJsonConfigConverter& operator=(const IJsonConfigConverter&) = default;
  IJsonConfigConverter(IJsonConfigConverter&&) = default;
  IJsonConfigConverter& operator=(IJsonConfigConverter&&) = default;

  virtual Status JsonToBridgeConfig(const ::std::string& json, BridgeConfig& config) const = 0;
  virtual Status JsonToIPConfigs(const ::std::string& json, IPConfigs& config) const = 0;
  virtual Status BridgeConfigToJson(const BridgeConfig& config, ::std::string& json) const = 0;
  virtual ::std::string IPConfigsToJson(const IPConfigs& config) const = 0;
  virtual ::std::string InterfacesToJson(const Interfaces& interfaces) const = 0;
  virtual ::std::string InterfaceConfigToJson(const InterfaceConfigs& port_configs) const = 0;
  virtual Status InterfaceConfigFromJson(const ::std::string& json, InterfaceConfigs& port_configs) const = 0;

};

}  // namespace netconfd
