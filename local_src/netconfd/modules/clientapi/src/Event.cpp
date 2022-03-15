#include "Event.hpp"
#include "JsonConverter.hpp"
#include "NetconfdDbusClient.hpp"

namespace netconf {
namespace api {

Status NotifyDynamicIPAction(const ::std::string &interface, DynamicIPEventAction action) {

  NetconfdDbusClient client;
  JsonConverter jc;
  ::std::string json = jc.ToJsonString(interface, action);

  auto result = client.NotifyDynamicIPAction(json);
  return result.error_;
}

Status MakeDynamicIPAction(const std::string &json_str, ::std::string &interface, DynamicIPEventAction& action) {
  JsonConverter jc;
  return jc.FromJsonString(json_str, interface, action);
}

Status ReloadHostConf() {
  NetconfdDbusClient client;
  auto result = client.ReloadHostConf();
  return result.error_;
}

}
}
