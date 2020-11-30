// SPDX-License-Identifier: GPL-2.0-or-later

#include "MockBridgeConfig.hpp"
#include "Error.hpp"
#include <memory>

namespace netconf {
namespace api {

::std::unique_ptr<MockBridgeConfig, void (*)(MockBridgeConfig*)> activeMock { nullptr, [](MockBridgeConfig*) {
} };

Error GetBridgeConfig(BridgeConfig& config) {
  if (activeMock){
    return activeMock->GetBridgeConfig(config);
  }
  return Error::Ok();
}

Error SetBridgeConfig(const BridgeConfig &config) {
  if (activeMock){
    return activeMock->SetBridgeConfig(config);
  }
  return Error { ErrorCode::BRIDGE_NAME_INVALID };
}

MockBridgeConfig::MockBridgeConfig() {
  activeMock.reset(this);
}

MockBridgeConfig::~MockBridgeConfig() {
  activeMock.reset();
}

}
}
