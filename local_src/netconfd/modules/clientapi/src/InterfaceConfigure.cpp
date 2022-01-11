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

Status SetInterfaceConfigs(const InterfaceConfigs &config) {
  NetconfdDbusClient client;
  auto result = client.SetInterfaceConfigs(ToJson(config));
  return result.error_;
}

Status GetInterfaceConfigs(InterfaceConfigs& config) {
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

Status GetInterfaceStatuses(InterfaceStatuses &statuses){
  NetconfdDbusClient client;
  auto result = client.GetInterfaceStatuses();

  auto error = result.error_;
  if(error.Ok()){
    JsonConverter jc;
    netconf::InterfaceStatuses nis;
    error = jc.FromJsonString(result.value_json_, nis);
    statuses = InterfaceStatuses{nis};
  }

  return error;
}


MacAddress GetMacAddress(const ::std::string &interface_name) {
  EthernetInterface interface { interface_name };
  return interface.GetMac();
}

Status SetInterfaceState(const ::std::string &interface_name, InterfaceState state)
{
  try{
    EthernetInterface interface { interface_name };
    interface.SetState(state);
  }catch(::std::exception& e){
    return Status{StatusCode::SYSTEM_CALL, e.what()};
  }
  return Status::Ok();
}

Status GetInterfaceState(const ::std::string &interface_name, InterfaceState &state)
{
  try{
    EthernetInterface interface { interface_name };
    state = interface.GetState();
  }catch(::std::exception& e){
    return Status{StatusCode::SYSTEM_CALL, e.what()};
  }
  return Status::Ok();
}



}  // namespace api
}  // namespace netconf
