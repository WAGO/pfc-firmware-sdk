// SPDX-License-Identifier: GPL-2.0-or-later

#include "DipSwitchConfig.hpp"
#include "JsonConverter.hpp"
#include "NetconfdDbusClient.hpp"
#include <sstream>
#include <string>

namespace netconf {
namespace api {

using namespace std::literals;

DipSwitchConfig MakeDipSwitchConfig(const ::std::string& json_str)
{
  JsonConverter jc;
  netconf::DipSwitchConfig config;
  auto status = jc.FromJsonString(json_str, config);
  if(status.NotOk()){
    // TODO throw something useful
  }
  return DipSwitchConfig{config};
}

DipSwitchIpConfig MakeDipSwitchIpConfig(const ::std::string& json_str)
{
  JsonConverter jc;
  DipSwitchIpConfig config;
  auto status = jc.FromJsonString(json_str, config);
  if (status.NotOk()) {
    // TODO: throw something useful
  }
  return config;
}

DipSwitchConfig GetDipSwitchConfig() {
  NetconfdDbusClient client;
  return MakeDipSwitchConfig(client.GetDipSwitchConfig());
}

DipSwitchConfig::DipSwitchConfig(const DipSwitchIpConfig &config, DipSwitchMode mode, int value)
  : config_{config, mode, value}
{

}

Status SetDipSwitchConfig(const DipSwitchConfig &config) {
  NetconfdDbusClient client;
  auto success = client.SetDipSwitchConfig(ToJson(config));
  return success ? Status::OK : Status::ERROR;
}

DipSwitchConfig::DipSwitchConfig(netconf::DipSwitchConfig config):config_{config} {
}

int DipSwitchConfig::GetDipSwitchValue() const noexcept {
  return config_.value_;
}

netconf::DipSwitchIpConfig DipSwitchConfig::GetDipSwitchIpConfig() const noexcept {
  return config_.ip_config_;
}

::std::string ToJson(const DipSwitchConfig& config) noexcept {
  JsonConverter jc;
  return jc.ToJsonString(config.GetDipSwitchConfig());
}

::std::string ToString(const DipSwitchConfig& config, const char sep) noexcept {
  // TODO implement this
  auto ip_config = config.GetDipSwitchIpConfig();
  auto dip_config = config.GetDipSwitchConfig();
  ::std::stringstream output;
    output << "ipaddr=" << ip_config.address_;
    output << sep;
    output << "netmask=" << ip_config.netmask_;
    output << sep;
    output << "value=" << dip_config.value_;
    output << sep;
    output << "mode=" << ToString(dip_config.mode_);
  return output.str();
}

::std::string ToString(const DipSwitchMode mode) noexcept
{
  switch(mode){
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

netconf::DipSwitchConfig DipSwitchConfig::GetDipSwitchConfig() const noexcept{
  return config_;
}

} // namespace api
}  // namespace neconf

