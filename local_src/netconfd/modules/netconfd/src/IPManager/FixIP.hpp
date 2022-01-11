
#pragma once

#include "Types.hpp"

namespace netconf {

/**
 * Sets Temporary IP addresses of each IPConfig to 192.168.(1+n).17
 * where n is the interface name index, i.e. index of 'br2' index is '2' hence n=2.
 * The source of each IPConfig is set to 'temporary', the netmask is set to 255.255.255.0
 */
void SetFixedIps(IPConfigs &ip_configs);

}  // namespace netconfd
