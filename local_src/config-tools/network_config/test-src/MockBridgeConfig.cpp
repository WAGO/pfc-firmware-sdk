/*
 * MockBridgeConfig.cpp
 *
 *  Created on: 17.02.2020
 *      Author: u014487
 */

#include "MockBridgeConfig.hpp"
#include <memory>

namespace netconf {

::std::unique_ptr<MockBridgeConfig, void(*)(MockBridgeConfig*)> activeMock{nullptr, [](MockBridgeConfig*){}};

BridgeConfig GetBridgeConfig() {
  if(activeMock)
    return activeMock->GetBridgeConfig();
  return BridgeConfig{};
}

Status SetBridgeConfig(const BridgeConfig &config) {
  if(activeMock)
    return activeMock->SetBridgeConfig(config);
  return Status::ERROR;
}


MockBridgeConfig::MockBridgeConfig() {
  activeMock.reset(this);
}

MockBridgeConfig::~MockBridgeConfig() {
  activeMock.reset();
}

}
