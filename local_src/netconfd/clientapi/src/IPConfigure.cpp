// SPDX-License-Identifier: LGPL-3.0-or-later

#include "IPConfig.hpp"
#include "JsonConverter.hpp"
#include "DipSwitchConfig.hpp"
#include "NetconfdDbusClient.hpp"
#include "Convert.hpp"

namespace netconf {
namespace api {

/*
 * The part of the implementation of IPConfig.hpp that can be mocked by means of link time substitution
 */

static Error ToIPConfigs(const DbusResult& result, IPConfigs& config){
  auto error = result.error_;
  if (error.Ok()) {
    JsonConverter jc;
    netconf::IPConfigs nip;

    error = jc.FromJsonString(result.value_json_, nip);
    config = IPConfigs{nip};
  }
  return error;
}

Error GetIPConfigs(IPConfigs& config) {
  NetconfdDbusClient client;
  auto result = client.GetIpConfigs();
  return ToIPConfigs(result, config);
}

Error GetCurrentIPConfigs(IPConfigs& config) {
  NetconfdDbusClient client;
  auto result = client.GetCurrentIpConfigs();
  return ToIPConfigs(result, config);
}

Error SetIPConfigs(const IPConfigs &config) {
  NetconfdDbusClient client;
  auto result = client.SetIpConfigs(ToJson(config));
  return result.error_;
}

Error SetTempFixIp() {
  NetconfdDbusClient client;
  auto result = client.SetTemporaryFixedIpAddress();
  return result.error_;
}

}  // namespace api
}  // namespace netconfd
