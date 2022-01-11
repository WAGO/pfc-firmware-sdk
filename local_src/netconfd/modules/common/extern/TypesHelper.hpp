// SPDX-License-Identifier: LGPL-3.0-or-later
#pragma once

#include <string>
#include <optional>
#include "Types.hpp"

namespace netconf {

static inline bool IsZeroIP(const ::std::string& address) {
  static const ::std::string zeroIP = "0.0.0.0";
  return address == zeroIP;
}

static inline bool IsEmptyOrZeroIP(const ::std::string& address) {
  return address.empty() || IsZeroIP(address);
}

::std::string IPSourceToString(const IPSource value);
IPSource StringToIPSource(const ::std::string& value);

bool IsEqual(const BridgeConfig& a, const BridgeConfig& b);
bool IsEqual(IPConfigs& lhs, IPConfigs& rhs);

void AddIPConfig(const IPConfig& ip_config, IPConfigs& ip_configs);

Bridges GetBridgesOfBridgeConfig(const BridgeConfig& bridge_config);

bool ConfigContainsBridgeWithSpecificInterfaces(const BridgeConfig& bridge_config, const Bridge& bridge,
                                                const Interfaces& interfaces);

void IpConfigsIntersection(IPConfigs& ip_configs, const Bridges& selection);

IPConfig& ComplementNetmask(IPConfig& ip_config);
IPConfigs& ComplementNetmasks(IPConfigs& ip_configs);

::std::string IPConfigToString(const IPConfig& config);

void RemoveUnnecessaryIPParameter(IPConfigs &ip_configs);

/**
 * Increment the number representation of the given IP address string by the given increment value.
 * @param address The IP address to be incremented.
 * @param increment The increment value
 * @return The incremented address
 */
Address IpAddressV4Increment(const Address& , uint32_t increment = 1);

/**
 * Extract the interface index from an interface name.
 * The name must start with characters followed by any number, e.g. eth0, br2, wwan5, myif42
 *
 * @return The index part of the interface name as optional integer.
 */
std::optional<int> ExtractInterfaceIndex(const std::string& bridgename);

/**
 * Get an iterator to an IPConfig in the IPConfigs matching the desired interface name.
 * @return an interator to the desired IPConfig, or end iterator.
 */
IPConfigs::iterator GetIpConfigByInterface(IPConfigs& configs, const std::string& interfacename);

/**
 * Get the set difference of A \ B by their interface names.
 * @param a IP config A
 * @param b IP config A
 * @return new IP configs with A \ B
 */
IPConfigs GetIpConfigsDifferenceByInterface(const IPConfigs& a, const IPConfigs& b);

/**
 * Sort the IP config based on the interface name
 * @param a The IP config to sort
 */
void SortByInterface(IPConfigs& a);

}
