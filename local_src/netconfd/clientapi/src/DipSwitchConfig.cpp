// SPDX-License-Identifier: LGPL-3.0-or-later

#include "DipSwitchConfig.hpp"
#include "JsonConverter.hpp"
#include "NetconfdDbusClient.hpp"
#include "Status.hpp"
#include "Convert.hpp"
#include <sstream>
#include <string>

namespace netconf {
namespace api {

using namespace std::literals;

Status MakeDipSwitchConfig(const ::std::string &json_str, netconf::DipSwitchConfig &config) {
  JsonConverter jc;
  return jc.FromJsonString(json_str, config);
}

Status MakeDipSwitchIpConfig(const ::std::string &json_str, netconf::DipSwitchIpConfig &config) {
  JsonConverter jc;
  return jc.FromJsonString(json_str, config);
}

Status GetDipSwitchConfig(netconf::DipSwitchConfig &config) {

  NetconfdDbusClient client;
  auto result = client.GetDipSwitchConfig();

  auto error = result.error_;
  if (error.Ok()) {
    JsonConverter jc;
    error = jc.FromJsonString(result.value_json_, config);
  }

  return error;

}

Status SetDipSwitchConfig(const netconf::DipSwitchConfig &config) {
  NetconfdDbusClient client;
  auto error_set = client.SetDipSwitchConfig(ToJson(config));
  return error_set.error_;
}

::std::string ToJson(const netconf::DipSwitchConfig &config) noexcept {
  JsonConverter jc;
  return jc.ToJsonString(config);
}

::std::string ToPrettyJson(const netconf::DipSwitchConfig &config) noexcept {
  JsonConverter jc;
  return jc.ToJsonString(config, JsonFormat::PRETTY);
}

::std::string ToString(const netconf::DipSwitchConfig &config, const char sep) noexcept {
  // TODO implement this
  auto &ip_config = config.ip_config_;
  ::std::stringstream output;
  output << "ipaddr=" << ip_config.address_;
  output << sep;
  output << "netmask=" << ip_config.netmask_;
  output << sep;
  output << "value=" << config.value_;
  output << sep;
  output << "mode=" << ToString(config.mode_);
  return output.str();
}

::std::string ToString(const DipSwitchMode mode) noexcept
{
  switch (mode) {
    case DipSwitchMode::OFF:
      return "off"s;
    case DipSwitchMode::STATIC:
      return "static"s;
    case DipSwitchMode::DHCP:
      return "dhcp"s;
    default:
      return "hw-not-available"s;
  }
}

}  // namespace api
}  // namespace neconf

