// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include "Status.hpp"
#include "Types.hpp"

namespace netconf {

class IIPManager {
 public:
  IIPManager() = default;
  virtual ~IIPManager() = default;

  IIPManager(const IIPManager&) = delete;
  IIPManager& operator=(const IIPManager&) = delete;
  IIPManager(IIPManager&&) = delete;
  IIPManager& operator=(IIPManager&&) = delete;

  virtual Status ApplyTempFixIpConfiguration() = 0;
  virtual Status ApplyIpConfiguration(const IPConfigs &config) = 0;
  virtual Status ApplyDipConfiguration(const DipSwitchIpConfig &dip_switch_ip_config) = 0;
  virtual Status ApplyIpConfiguration(const IPConfigs &ip_configs, const DipSwitchIpConfig &dip_switch_ip_config) = 0;

  virtual void ClearIpConfiguration() = 0;

  virtual Status ValidateIPConfigs(const IPConfigs &configs) const = 0;
  virtual IPConfigs GetIPConfigs() const = 0;
  virtual IPConfigs GetIPConfigs(const Bridges &bridges) const = 0;
  virtual IPConfigs GetCurrentIPConfigs() const = 0;

};

} /* namespace netconf */
