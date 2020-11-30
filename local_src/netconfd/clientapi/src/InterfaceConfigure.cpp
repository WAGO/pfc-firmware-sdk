// SPDX-License-Identifier: LGPL-3.0-or-later

#include "InterfaceConfig.hpp"

#include <boost/algorithm/string/join.hpp>
#include <vector>
#include <sstream>

#include "NetconfdDbusClient.hpp"
#include "EthernetInterface.hpp"
#include "JsonConverter.hpp"
#include "Convert.hpp"

namespace netconf {
namespace api {

Error SetInterfaceConfigs(const InterfaceConfigs &config) {
  NetconfdDbusClient client;
  auto result = client.SetInterfaceConfigs(ToJson(config));
  return result.error_;
}

Error GetInterfaceConfigs(InterfaceConfigs& config) {
  NetconfdDbusClient client;
  auto result = client.GetInterfaceConfigs();

  auto error = result.error_;
  if(error.Ok()){
    JsonConverter jc;
    netconf::InterfaceConfigs nic;
    error = jc.FromJsonString(result.value_json_, nic);
    config = InterfaceConfigs{nic};
  }

  return error;
}

}  // namespace api
}  // namespace netconf
