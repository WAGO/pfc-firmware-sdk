// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include <BridgeConfig.hpp>
#include <gmock/gmock.h>

namespace netconf {
namespace api {

struct IMockBridgeConfig {
  IMockBridgeConfig() = default;
  virtual ~IMockBridgeConfig() = default;

  virtual BridgeConfig GetBridgeConfig() = 0;
  virtual Status SetBridgeConfig(const BridgeConfig &config) = 0;
};

struct MockBridgeConfig : public IMockBridgeConfig {
  MockBridgeConfig();
  virtual ~MockBridgeConfig();

  MOCK_METHOD0(GetBridgeConfig,BridgeConfig() )
  ;MOCK_METHOD1(SetBridgeConfig, Status(const BridgeConfig &config) )
  ;

};

}  // namespace api
}  // namespace netconf


//---- End of header file ------------------------------------------------------

