// SPDX-License-Identifier: LGPL-3.0-or-later

#include "IPConfig.hpp"

#include <boost/asio/ip/address.hpp>
#include <boost/asio/ip/network_v4.hpp>
#include "ConfigBase.hpp"
#include "NetconfdDbusClient.hpp"
#include "JsonConverter.hpp"
#include "DipSwitchConfig.hpp"
#include "InterfaceInformationApi.hpp"

namespace netconf {
namespace api {

::std::string ToString(IPSource source) {
  switch (source) {
    case IPSource::DHCP:
      return "dhcp";
    case IPSource::BOOTP:
      return "bootp";
    case IPSource::STATIC:
      return "static";
    default:
      return "none";
  }
}

IPConfigs::IPConfigs(const netconf::IPConfigs &configs) : detail::ConfigBase<netconf::IPConfig>(configs) {

}

void IPConfigs::AddIPConfig(IPConfig config) {
  AddConfig(config);
}

void IPConfigs::RemoveIPConfig(const ::std::string &interface_name) {
  RemoveConfig(interface_name);
}

boost::optional<IPConfig> IPConfigs::GetIPConfig(const ::std::string &interface_name) const {
  return GetConfig(interface_name);
}

::std::string IPConfigs::GetCompareValue(const IPConfig & config) const noexcept {
  return config.interface_;
}


::std::string ToJson(const IPConfigs& configs) noexcept {
  JsonConverter jc;
  return jc.ToJsonString(configs.GetConfig());
}

static IPConfigs FilterCopyByType(const IPConfigs &ip_configs, DeviceType type) {
  auto interface_infos = GetInterfaceInformation(type);

  auto inner_configs = ip_configs.GetConfig();

  auto interface_name_not_in_infos = [&](const auto &ip_cfg) -> bool {
    return ::std::find_if(interface_infos.begin(), interface_infos.end(), [&](const auto &info) {
      return info.GetInterfaceName() == ip_cfg.interface_;
    }) == interface_infos.end();
  };

  inner_configs.erase(std::remove_if(inner_configs.begin(), inner_configs.end(), interface_name_not_in_infos),
                      inner_configs.end());

  return IPConfigs { inner_configs };
}


Error GetIPConfigs(DeviceType type, IPConfigs& configs) {
  IPConfigs c;
  auto error = GetIPConfigs(c);
  if(error.IsNotOk())
  {
    return error;
  }
  configs = FilterCopyByType(c, type);
  return error;
}

Error GetCurrentIPConfigs(DeviceType type, IPConfigs& configs) {
  IPConfigs c;
  auto error = GetCurrentIPConfigs(c);
  if(error.IsNotOk())
  {
    return error;
  }
  configs = FilterCopyByType(c, type);
  return error;
}

void DeleteIPConfig(::std::string interface_name) {
  IPConfigs ip_configs;
  IPConfig ip_config { ::std::move(interface_name), IPSource::NONE, "", "" };
  ip_configs.AddIPConfig(ip_config);
  SetIPConfigs(ip_configs);
}
::std::string CalculateBroadcast(const IPConfig &config) noexcept
{
  namespace bip = ::boost::asio::ip;
  try {
    auto netv4 = bip::make_network_v4(bip::make_address_v4(config.address_), bip::make_address_v4(config.netmask_));
    return netv4.network().to_string();
  } catch (...) {
    return std::string { };
  }
}

Error MakeIPConfigs(const ::std::string& json_str, IPConfigs& config) noexcept{
  JsonConverter jc;
  netconf::IPConfigs c;
  Error error = jc.FromJsonString(json_str, c);
  config = IPConfigs(c);
  return error;

}

::std::string ToJson(const netconf::IPConfig &ip_config) noexcept{
  JsonConverter jc;
  return jc.ToJsonString(ip_config);
}



}  // api
}  // netconf
