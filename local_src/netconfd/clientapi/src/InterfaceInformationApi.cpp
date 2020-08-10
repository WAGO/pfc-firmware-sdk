#include "InterfaceInformationApi.hpp"
#include "NetconfdDbusClient.hpp"
#include "JsonConverter.hpp"
#include <boost/algorithm/string/join.hpp>
#include <algorithm>
#include <sstream>
#include <vector>

namespace netconf {
namespace api {

InterfaceInformations GetInterfaceInformation() {
  NetconfdDbusClient client;
  JsonConverter jc;
  InterfaceInformations iis;
  auto status = jc.FromJsonString(client.GetDeviceInterfaces(), iis);
  if (status.NotOk()) {
    /* TODO PND: appropriate error response */
  }
  return iis;
}

InterfaceInformations GetInterfaceInformation(DeviceType type) {
  NetconfdDbusClient client;
  JsonConverter jc;
  InterfaceInformations iis;
  auto status = jc.FromJsonString(client.GetDeviceInterfaces(), iis);
  if (status.NotOk()) {
    /* TODO PND: appropriate error response */
    return iis;
  }

  iis.erase(std::remove_if(iis.begin(), iis.end(), [type=type](const auto &i) {
    return !(i.GetType() && type);
  }), iis.end());
  return iis;
}

::std::string ToString(const InterfaceInformation &info) noexcept{
  ::std::stringstream ss;
  ss << "device=" << info.GetInterfaceName();
  ss << " label=" << info.GetInterfaceLabel();
  ss << " type=" << ToString(info.GetType());
  ss << " ip-readonly=" << ::std::boolalpha << info.IsIpConfigurationReadonly();
  return ss.str();
}

::std::string ToString(const InterfaceInformations &info) noexcept{
  ::std::vector<::std::string> text_infos;
  ::std::transform(info.cbegin(), info.cend(), ::std::back_inserter(text_infos), [](const auto& i){return ToString(i);});
  return ::boost::algorithm::join(text_infos, ",");
}

::std::string ToJson(const InterfaceInformation &info) noexcept {
  JsonConverter jc;
  return jc.ToJsonString(info);
}

::std::string ToJson(const InterfaceInformations &info) noexcept{
  JsonConverter jc;
  return jc.ToJsonString(info);
}

}  // namespace api
}  // namespace netconf

