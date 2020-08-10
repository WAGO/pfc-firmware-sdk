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

#include <cstring>
#include <unistd.h>
#include <glib.h>
#include <boost/algorithm/string.hpp>

#include "error_handling.hpp"
#include "ct_liblog.h"
#include "utilities.hpp"
#include "BridgeConfig.hpp"
#include "IPConfig.hpp"
#include "InterfaceConfig.hpp"

namespace napi = ::netconf::api;

namespace {

char* get_hostname(int debugmode) {
  char buffer[256];
  if (debugmode) {
    strcpy(buffer, "debughostname");
  } else {
    int status = gethostname(buffer, sizeof(buffer));
    if (status != SUCCESS) {
      erh_set_error(SYSTEM_CALL_ERROR, "gethostname failed.", "");
    }
  }
  return g_strdup(buffer);
}

bool are_all_bridge_interfaces_down(
  const ::std::string &bridge_name,
  const napi::BridgeConfig& bridge_config,
  napi::InterfaceConfigs& interface_config) {

  auto interface_names = bridge_config.GetBridgeInterfaces(bridge_name);
  auto up = find_if(interface_names.begin(), interface_names.end(), [& interface_config](const netconf::Interface& name) {
    auto optional_interface = interface_config.GetInterfaceConfig(name);
    return optional_interface ? optional_interface->state_ == netconf::InterfaceState::UP : false;
  });

  return up == interface_names.end();
}

void set_port_name(port_data_t& pd, const char* name) {
  pd.port_name = g_strdup(name);
}

void set_port_state(port_data_t& pd, const ::std::string& bridge_name, const napi::BridgeConfig& bridge_config,
                    napi::InterfaceConfigs interface_config) {
  bool bridge_interfaces_down = are_all_bridge_interfaces_down(bridge_name, bridge_config, interface_config);
  pd.state = g_strdup(bridge_interfaces_down ? "disabled" : "enabled");
}

void set_type(port_data_t& pd, const netconf::IPConfig& ip_config){
  switch (ip_config.source_)
  {
    case netconf::IPSource::STATIC:
      pd.type = g_strdup("static");
      break;
    case netconf::IPSource::DHCP:
      pd.type = g_strdup("dhcp");
      break;
    case netconf::IPSource::BOOTP:
      pd.type = g_strdup("bootp");
      break;
    default:
      pd.type = g_strdup("unknown");
      break;
  }
}

void set_ip_address(port_data_t& pd, const netconf::IPConfig& ip_config) {
  pd.ip_addr = g_strdup(ip_config.address_.c_str());
}

void set_netmask(port_data_t& pd, const netconf::IPConfig& ip_config) {
  pd.netmask = g_strdup(ip_config.netmask_.c_str());
  conv_ip_ascii2bin(pd.ip_addr, &pd.ip_addr_bin);
  conv_ip_ascii2bin(pd.netmask, &pd.netmask_bin);
  pd.network_bin = pd.ip_addr_bin & pd.netmask_bin;
}

void generate_legal_ports(const napi::InterfaceConfigs& interface_configs, char ***legal_ports) {
  size_t count = 0;
  for(auto& ic : interface_configs.GetConfig()) {

    if( ic.device_name_.find("X") == 0) {
      count++;
    }
  }

  *legal_ports = (char **)g_malloc((count+1) * sizeof(char*));

  for(size_t i = 0; i < count; i++) {
    auto name = ::std::string{"br" + ::std::to_string(i)};
    (*legal_ports)[i] = g_strdup(name.c_str());
  }
  (*legal_ports)[count] = NULL;

}

void parse_config_parameter(ip_config_t *data, char **legal_ports, int debugmode,
                            const napi::BridgeConfig& bridge_config, const napi::IPConfigs& ip_configs,
                            const napi::InterfaceConfigs& interface_configs) {

  data->host_name = get_hostname(debugmode);

  auto bridges = bridge_config.GetBridges();
  // Get list of ports configured from network-interfaces.xml.

  ::std::string bridge_list_str = boost::algorithm::join(bridges, ",");
  erh_assert(!bridge_list_str.empty(), eStatusCode::INVALID_PARAMETER, "Error: No bridges found.", "");

  data->port_name_list = (char **)g_malloc0((bridges.size() + 1) * sizeof(char*));
  data->port_data_list = (port_data_t **)g_malloc0((bridges.size() + 1) * sizeof(port_data_t*));

  // Read port specific parameters, IP-Address, Netmask.
  for (const auto& bridge : bridges) {
    if (0 > strv_is_in_set(legal_ports, bridge.c_str())) {
      continue;
    }

    port_data_t *pd = (port_data_t *)g_malloc(sizeof(port_data_t));

    set_port_name(*pd, bridge.c_str());
    set_port_state(*pd, bridge.c_str(), bridge_config, interface_configs);

    auto optional = ip_configs.GetIPConfig(bridge.c_str());
    erh_assert(optional != boost::none, eStatusCode::INVALID_PARAMETER,
               "Failed to get IP config for bridge %s.", bridge.c_str());

    auto ip_config = *optional;
    set_type(*pd, ip_config);
    set_ip_address(*pd, ip_config);
    set_netmask(*pd, ip_config);

    data->port_name_list[data->no_of_ports] = g_strdup(bridge.c_str());
    data->port_data_list[data->no_of_ports] = pd;
    data->no_of_ports++;
  }
}

} // Anonymous namespace

void netcfg_read_settings(ip_config_t *data, char ***legal_ports, prgconf_t const * const prgconf, int debugmode) {
  napi::BridgeConfig bridge_config;
  napi::IPConfigs ip_configs;
  napi::InterfaceConfigs interface_configs;

  if (prgconf->bridge_config[0] == '\0' || prgconf->ip_config[0] == '\0' || prgconf->interface_config[0] == '\0') {
    bridge_config = napi::GetBridgeConfig();
    ip_configs = napi::GetIPConfigs();
    interface_configs = napi::GetInterfaceConfigs();
  } else {
    bridge_config = napi::MakeBridgeConfig(prgconf->bridge_config);
    ip_configs = napi::MakeIPConfigs(prgconf->ip_config);
    interface_configs = napi::MakeInterfaceConfigs(prgconf->interface_config);
  }

  generate_legal_ports(interface_configs, legal_ports);

  parse_config_parameter(data, *legal_ports, debugmode, bridge_config, ip_configs, interface_configs);
}
