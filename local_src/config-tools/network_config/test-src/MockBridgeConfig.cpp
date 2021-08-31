// SPDX-License-Identifier: GPL-2.0-or-later

#include "MockBridgeConfig.hpp"
#include "Status.hpp"
#include <memory>

namespace netconf {
namespace api {

::std::unique_ptr<MockBridgeConfig, void (*)(MockBridgeConfig*)> activeMock { nullptr, [](MockBridgeConfig*) {
} };

Status GetBridgeConfig(BridgeConfig& config) {
  if (activeMock){
    return activeMock->GetBridgeConfig(config);
  }
  return Status::Ok();
}

Status SetBridgeConfig(const BridgeConfig &config) {
  if (activeMock){
    return activeMock->SetBridgeConfig(config);
  }
  return Status { StatusCode::BRIDGE_NAME_INVALID };
}

MockBridgeConfig::MockBridgeConfig() {
  activeMock.reset(this);
}

MockBridgeConfig::~MockBridgeConfig() {
  activeMock.reset();
}

}
}
