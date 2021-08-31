//------------------------------------------------------------------------------
/// Copyright (c) 2020 WAGO Kontakttechnik GmbH & Co. KG
///
/// PROPRIETARY RIGHTS of WAGO Kontakttechnik GmbH & Co. KG are involved in
/// the subject matter of this material. All manufacturing, reproduction,
/// use, and sales rights pertaining to this subject matter are governed
/// by the license agreement. The recipient of this software implicitly
/// accepts the terms of the license.
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
///
///  \brief    Reading of network information (ethernet, ip and interfaces).
///
///  \author   MSc, WAGO Kontakttechnik GmbH & Co. KG.
///  \author   MOe, WAGO Kontakttechnik GmbH & Co. KG.
//------------------------------------------------------------------------------
#include "network_config.hpp"

#include <boost/algorithm/string.hpp>
#include <ct_liblog.h>
#include <BridgeConfig.hpp>
#include <IPConfig.hpp>
#include <InterfaceConfig.hpp>

#include "error_handling.hpp"
#include "utilities.hpp"

namespace napi = ::netconf::api;

namespace {

::std::string get_hostname(int debugmode) {
  if (debugmode) {
    return "debughostname";
  }

  char buffer[256];
  int status = gethostname(buffer, sizeof(buffer));
  if (status != SUCCESS) {
    erh_set_error(SYSTEM_CALL_ERROR, "gethostname failed.", "");
    return "";
  }

  return ::std::string{buffer};
}

void generate_legal_ports(const napi::InterfaceConfigs& interface_configs, ::std::vector<::std::string> &legal_ports) {
  size_t count = 0;
  for(auto& ic : interface_configs.GetConfig()) {

    if( ic.device_name_.find("X") == 0) {
      legal_ports.push_back("br" + ::std::to_string(count));
      count++;
    }
  }
}

void parse_config_parameter(IpConfiguration &ipConfig, std::vector<std::string> &legal_ports, int debugmode,
                            const napi::BridgeConfig& bridgeConfig, const napi::IPConfigs& ip_configs,
                            const napi::InterfaceConfigs& interfaceConfigs) {

  ipConfig.host_name = get_hostname(debugmode);

  auto bridges = bridgeConfig.GetBridges();
  // Get list of ports configured from network-interfaces.xml.

  ::std::string bridge_list_str = boost::algorithm::join(bridges, ",");
  erh_assert(!bridge_list_str.empty(), eStatusCode::INVALID_PARAMETER, "Error: No bridges found.", "");

  // Read port specific parameters, IP-Address, Netmask.
  for (const auto& bridge : bridges) {
    auto it = ::std::find(legal_ports.begin(), legal_ports.end(), bridge);
    if (it == legal_ports.end()) {
      continue;
    }

    PortData pd;
    pd.port_name_ = bridge;
    pd.setState(bridge, bridgeConfig, interfaceConfigs);

    auto optional = ip_configs.GetIPConfig(bridge.c_str());
    erh_assert(optional != boost::none, eStatusCode::INVALID_PARAMETER,
               "Failed to get IP config for bridge %s.", bridge.c_str());

    auto c = *optional;
    pd.setType(c);
    pd.setIpAddress(c.address_);
    pd.setNetmask(c.netmask_);

    ipConfig.port_name_list.push_back(bridge);
    ipConfig.port_data_list.push_back(pd);
  }
}

} // Anonymous namespace

void netcfg_read_settings(IpConfiguration &data, std::vector<std::string> &legal_bridges,
                          const prgconf_t &prgconf, int debugmode) {
  napi::BridgeConfig bridge_config;
  napi::IPConfigs ip_configs;
  napi::InterfaceConfigs interface_configs;

  ::netconf::Status status;

  if (prgconf.bridge_config.empty() || prgconf.ip_config.empty() || prgconf.interface_config.empty()) {
    status = napi::GetBridgeConfig(bridge_config);
    erh_assert(status.IsOk(), eStatusCode::INVALID_PARAMETER, status.ToString());

    status = napi::GetCurrentIPConfigs(ip_configs);
    erh_assert(status.IsOk(), eStatusCode::INVALID_PARAMETER, status.ToString());

    status = napi::GetInterfaceConfigs(interface_configs);
    erh_assert(status.IsOk(), eStatusCode::INVALID_PARAMETER, status.ToString());
  } else {
    status = napi::MakeBridgeConfig(prgconf.bridge_config, bridge_config);
    erh_assert(status.IsOk(), eStatusCode::INVALID_PARAMETER, status.ToString());

    status = napi::MakeIPConfigs(prgconf.ip_config, ip_configs);
    erh_assert(status.IsOk(), eStatusCode::INVALID_PARAMETER, status.ToString());

    status = napi::MakeInterfaceConfigs(prgconf.interface_config, interface_configs);
    erh_assert(status.IsOk(), eStatusCode::INVALID_PARAMETER, status.ToString());
  }

  generate_legal_ports(interface_configs, legal_bridges);

  parse_config_parameter(data, legal_bridges, debugmode, bridge_config, ip_configs, interface_configs);
}
