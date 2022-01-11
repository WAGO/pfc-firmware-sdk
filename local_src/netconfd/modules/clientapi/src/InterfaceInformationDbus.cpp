// SPDX-License-Identifier: LGPL-3.0-or-later

#include "NetconfdDbusClient.hpp"
#include "JsonConverter.hpp"
#include <InterfaceInformationApi.hpp>
#include <algorithm>
#include <sstream>
#include <vector>

/*
 * This code is separated in order to allow link seams in unit tests.
 */

namespace netconf {
namespace api {

InterfaceInformations GetInterfaceInformation() {
  NetconfdDbusClient client;
  JsonConverter jc;
  InterfaceInformations iis;
  auto result = client.GetDeviceInterfaces();
  auto status = jc.FromJsonString(result.value_json_, iis);
  if (status.IsNotOk()) {
    /* TODO PND: appropriate status response */
  }
  return iis;
}

InterfaceInformations GetInterfaceInformation(DeviceType type) {
  NetconfdDbusClient client;
  JsonConverter jc;
  InterfaceInformations iis;
  auto result = client.GetDeviceInterfaces();
  auto status = jc.FromJsonString(result.value_json_, iis);
  if (status.IsNotOk()) {
    /* TODO PND: appropriate status response */
    return iis;
  }

  iis.erase(std::remove_if(iis.begin(), iis.end(), [type=type](const auto &i) {
    return !(i.GetType() && type);
  }), iis.end());
  return iis;
}

}
}
