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

static Status ToIPConfigs(const DbusResult& result, IPConfigs& config){
  auto status = result.error_;
  if (status.Ok()) {
    JsonConverter jc;
    netconf::IPConfigs nip;

    status = jc.FromJsonString(result.value_json_, nip);
    config = IPConfigs{nip};
  }
  return status;
}

Status GetIPConfigs(IPConfigs& config) {
  NetconfdDbusClient client;
  auto result = client.GetIpConfigs();
  return ToIPConfigs(result, config);
}

Status GetCurrentIPConfigs(IPConfigs& config) {
  NetconfdDbusClient client;
  auto result = client.GetCurrentIpConfigs();
  return ToIPConfigs(result, config);
}

Status SetIPConfigs(const IPConfigs &config) {
  NetconfdDbusClient client;
  auto result = client.SetIpConfigs(ToJson(config));
  return result.error_;
}

Status SetTempFixIp() {
  NetconfdDbusClient client;
  auto result = client.SetTemporaryFixedIpAddress();
  return result.error_;
}

}  // namespace api
}  // namespace netconfd
