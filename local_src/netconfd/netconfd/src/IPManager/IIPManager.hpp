// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include "Error.hpp"
#include "Types.hpp"

namespace netconf {

class IIPManager {
 public:
  IIPManager() = default;
  virtual ~IIPManager() = default;

  IIPManager(const IIPManager&) = delete;
  IIPManager& operator=(const IIPManager&) = delete;
  IIPManager(const IIPManager&&) = delete;
  IIPManager& operator=(const IIPManager&&) = delete;

  virtual Error ApplyTempFixIpConfiguration(const IPConfigs &config) = 0;
  virtual Error ApplyIpConfiguration(const IPConfigs &config) = 0;
  virtual Error ApplyIpConfiguration(const DipSwitchIpConfig &dip_switch_ip_config) = 0;
  virtual Error ApplyIpConfiguration(const IPConfigs &ip_configs, const DipSwitchIpConfig &dip_switch_ip_config) = 0;

  virtual Error ValidateIPConfigs(const IPConfigs &configs) const = 0;
  virtual IPConfigs GetIPConfigs() const = 0;
  virtual IPConfigs GetIPConfigs(const Bridges &bridges) const = 0;
  virtual IPConfigs GetCurrentIPConfigs() const = 0;

};

} /* namespace netconf */
