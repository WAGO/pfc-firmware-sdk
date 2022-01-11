// SPDX-License-Identifier: GPL-2.0-or-later
#pragma once

#include <gmock/gmock.h>
#include "IPersistenceProvider.hpp"

namespace netconf {

class MockIPersistenceProvider : public IPersistenceProvider {
 public:

  MOCK_METHOD1(Write, Status(const BridgeConfig &config) )
  ;MOCK_METHOD1(Read, Status(BridgeConfig &config))
  ;MOCK_METHOD1(Write, Status(const IPConfigs &configs) )
  ;MOCK_METHOD1(Read, Status(IPConfigs &configs) )
  ;MOCK_METHOD1(Write, Status(const DipSwitchIpConfig &config) )
  ;MOCK_METHOD1(Read, Status(DipSwitchIpConfig &config) )
  ;MOCK_METHOD2(Read, Status(BridgeConfig &config, IPConfigs &configs) )
  ;MOCK_METHOD2(Backup, Status(const std::string &file_path, const std::string &targetversion) )
  ;MOCK_METHOD5(Restore, Status(const ::std::string &file_path, BridgeConfig &bridge_config, IPConfigs &ip_configs,
          InterfaceConfigs &interface_configs, DipSwitchIpConfig &dip_switch_config) )
  ;MOCK_CONST_METHOD0(GetBackupParameterCount, uint32_t() )
  ;
};

} // namespace netconf
