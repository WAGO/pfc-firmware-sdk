// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include "JsonConfigConverter.hpp"
#include "IPersistenceJsonConfigConverter.hpp"
#include "IJsonConvert.hpp"

namespace netconfd {

class PersistenceJsonConfigConverter : public JsonConfigConverter,
    public IPersistenceJsonConfigConverter, public IJsonConvert<InterfaceConfigs> {
 public:
  PersistenceJsonConfigConverter() = default;
  virtual ~PersistenceJsonConfigConverter() = default;

  PersistenceJsonConfigConverter(const PersistenceJsonConfigConverter&) = delete;
  PersistenceJsonConfigConverter& operator=(const PersistenceJsonConfigConverter&) = delete;
  PersistenceJsonConfigConverter(const PersistenceJsonConfigConverter&&) = delete;
  PersistenceJsonConfigConverter& operator=(const PersistenceJsonConfigConverter&&) = delete;

  Status ToJson(const BridgeConfig& bridge_config, const IPConfigs& ip_configs, bool pretty, ::std::string& json_out) const override;
  Status ToConfigs(const ::std::string& json_str, BridgeConfig& bridge_config, IPConfigs& ip_configs) const override;

  Status ToJson(const BridgeConfig& bridge_config, const IPConfigs& ip_configs, const DipSwitchIpConfig& dip_ip_config,
                        bool pretty, ::std::string& json_out) const override;

  Status ToConfigs(const ::std::string& json, BridgeConfig& bridge_config, IPConfigs& ip_configs,
                   DipSwitchIpConfig& dip_ip_config) const override;

  ::std::string ToJsonString(const InterfaceConfigs& obj) override;

  Status FromJsonString(const ::std::string& str, InterfaceConfigs& out_obj) override;

};

} /* namespace netconfd */
