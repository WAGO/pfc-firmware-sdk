// SPDX-License-Identifier: LGPL-3.0-or-later
#pragma once

#include "BaseTypes.hpp"
#include "Error.hpp"

namespace netconf {
namespace api {

::std::string ToJson(const netconf::DipSwitchConfig& config) noexcept;

::std::string ToString(const netconf::DipSwitchConfig& config, const char sep = ' ') noexcept;

::std::string ToString(const netconf::DipSwitchMode mode) noexcept;

/**
 * Create a DipSwitchConfig from a json string.
 *
 *  e.g.
 * {"mode":"static","value":123,"ipaddr":"192.168.1.1","netmask":"255.255.255.0"}
 *
 */
Error MakeDipSwitchConfig(const ::std::string &json_str, DipSwitchConfig& config);

/**
 * Create a DipSwitchIpConfig form json string.
 * @param {"ipaddr":"192.168.1.1","netmask":"255.255.255.0"}
 */
Error MakeDipSwitchIpConfig(const ::std::string &json_str, DipSwitchIpConfig& config);

/**
 * @brief Get DIP switch configuration.
 *
 * @return DIP switch configuration containing:
 *         - IP address, e.g. 192.168.1.0 (the first three octets matter)
 *         - netmask
 *         - mode, (off, dhcp, static, hw-not-available)
 *         - value, the value of the last octet (0 - 255)
 */
Error GetDipSwitchConfig(netconf::DipSwitchConfig& config);

/**
 * @brief Set DIP switch configuration.
 *
 * @param config The DIP switch configuration containing IP address and netmask.
 * @return Status @see Status::Ok on success.
 */
Error SetDipSwitchConfig(const netconf::DipSwitchConfig& config);


}  // namespace api
}  // namespace netconf

//---- End of header file ------------------------------------------------------

