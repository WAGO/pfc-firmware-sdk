// SPDX-License-Identifier: GPL-2.0-or-later

#include "IPConfig.hpp"
#include "JsonConverter.hpp"
#include "DipSwitchConfig.hpp"
#include "NetconfdDbusClient.hpp"

namespace netconf {
namespace api {

/*
 * The part of the implementation of IPConfig.hpp that can be mocked by means of link time substitution
 */


IPConfigs GetIPConfigs() {
  NetconfdDbusClient client;
  return MakeIPConfigs(client.GetIpConfigs());
}

IPConfigs GetCurrentIPConfigs() {
  NetconfdDbusClient client;
  return MakeIPConfigs(client.GetCurrentIpConfigs());
}


Status SetIPConfigs(const IPConfigs &config) {
  NetconfdDbusClient client;
  ::std::string json_config = ToJson(config);
  if (client.SetIpConfigs(json_config)) {
    return Status::OK;
  }
  return Status::ERROR;
}

void SetTempFixIp() {
  NetconfdDbusClient client;
  client.SetTemporaryFixedIpAddress();
}

}  // namespace api
}  // namespace netconfd
