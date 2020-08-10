// SPDX-License-Identifier: GPL-2.0-or-later

#include "BridgeConfig.hpp"
#include "NetconfdDbusClient.hpp"
#include "JsonConverter.hpp"

namespace netconf {
namespace api {

// Those are in this separate compilation unit, so that link time substitution works for the unit tests

BridgeConfig GetBridgeConfig() {

  NetconfdDbusClient client;
  return MakeBridgeConfig(client.GetBridgeConfig());
}

Status SetBridgeConfig(const BridgeConfig &config) {
  NetconfdDbusClient client;
  if (client.SetBridgeConfig(ToJson(config))) {
    return Status::OK;
  }
  return Status::ERROR;
}



}  // namespace api
}  // namespace netconf
