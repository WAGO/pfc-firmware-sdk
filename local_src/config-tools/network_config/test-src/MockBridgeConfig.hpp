// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include <BridgeConfig.hpp>
#include <gmock/gmock.h>

namespace netconf {
namespace api {

struct IMockBridgeConfig {
  IMockBridgeConfig() = default;
  virtual ~IMockBridgeConfig() = default;

  virtual Error GetBridgeConfig(BridgeConfig& config) = 0;
  virtual Error SetBridgeConfig(const BridgeConfig &config) = 0;
};

struct MockBridgeConfig : public IMockBridgeConfig {
  MockBridgeConfig();
  virtual ~MockBridgeConfig();

  MOCK_METHOD1(GetBridgeConfig,Error(BridgeConfig& config) )
  ;MOCK_METHOD1(SetBridgeConfig, Error(const BridgeConfig &config) )
  ;

};

}  // namespace api
}  // namespace netconf

//---- End of header file ------------------------------------------------------

