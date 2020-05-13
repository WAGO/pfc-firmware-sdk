// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include "Status.hpp"
#include "Types.hpp"

namespace netconfd {

class IPersistenceProvider {
 public:
  IPersistenceProvider() = default;
  virtual ~IPersistenceProvider() = default;

  IPersistenceProvider(const IPersistenceProvider&) = delete;
  IPersistenceProvider& operator=(const IPersistenceProvider&) = delete;
  IPersistenceProvider(const IPersistenceProvider&&) = delete;
  IPersistenceProvider& operator=(const IPersistenceProvider&&) = delete;

  virtual Status Write(const BridgeConfig& config) = 0;
  virtual Status Read(BridgeConfig& config) = 0;

  virtual Status Write(const IPConfigs& configs) = 0;
  virtual Status Read(IPConfigs& configs) = 0;

  virtual Status Write(const DipSwitchIpConfig& config) = 0;
  virtual Status Read(DipSwitchIpConfig& config) = 0;

  virtual Status Read(BridgeConfig& config, IPConfigs& configs) = 0;

  virtual Status Backup(const std::string& file_path,const std::string& targetversion) = 0;
  virtual Status Restore(const ::std::string &file_path,
                         BridgeConfig &bridge_config,
                         IPConfigs &ip_configs,
                         InterfaceConfigs &interface_configs,
                         DipSwitchIpConfig &dip_switch_config) = 0;
  virtual uint32_t GetBackupParameterCount() const = 0;

};

} /* namespace netconfd */
