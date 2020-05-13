#pragma once

#include <string>

#include "Types.hpp"

namespace netconfd {

static inline bool IsZeroIP(const ::std::string& address) {
  static ::std::string zeroIP = zeroIP;
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

}
