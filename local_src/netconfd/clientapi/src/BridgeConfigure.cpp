// SPDX-License-Identifier: LGPL-3.0-or-later

#include "BridgeConfig.hpp"
#include "NetconfdDbusClient.hpp"
#include "JsonConverter.hpp"
#include "Convert.hpp"

namespace netconf {
namespace api {

// Those are in this separate compilation unit, so that link time substitution works for the unit tests

Status GetBridgeConfig(BridgeConfig& config) {

  NetconfdDbusClient client;
  auto result = client.GetBridgeConfig();
  auto error = result.error_;
  if (error.Ok()) {
    JsonConverter jc;
    netconf::BridgeConfig nbc;
    error = jc.FromJsonString(result.value_json_, nbc);
    config = BridgeConfig{nbc};
  }

  return error;

}

Status SetBridgeConfig(const BridgeConfig &config) {
  NetconfdDbusClient client;
  auto result = client.SetBridgeConfig(ToJson(config));
  return result.error_;
}

}  // namespace api
}  // namespace netconf
