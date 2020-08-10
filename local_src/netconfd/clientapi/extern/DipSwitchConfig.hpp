// SPDX-License-Identifier: GPL-2.0-or-later
#pragma once

#include "BaseTypes.hpp"
#include "Status.hpp"

namespace netconf {
namespace api {


class DipSwitchConfig {
 public:
  DipSwitchConfig() = default;
  explicit DipSwitchConfig(netconf::DipSwitchConfig config);
  explicit DipSwitchConfig(const DipSwitchIpConfig &config, DipSwitchMode mode = DipSwitchMode::OFF, int value = 0);

  int GetDipSwitchValue() const noexcept;
  netconf::DipSwitchIpConfig GetDipSwitchIpConfig() const noexcept;
  netconf::DipSwitchConfig GetDipSwitchConfig() const noexcept;


 private:
  netconf::DipSwitchConfig config_;
};

::std::string ToJson(const DipSwitchConfig& config) noexcept;

::std::string ToString(const DipSwitchConfig& config, const char sep = ' ') noexcept;

::std::string ToString(const DipSwitchMode mode) noexcept;

/**
 * Create a DipSwitchConfig from a json string.
 *
 *  e.g.
 * {"mode":"static","value":123,"ipaddr":"192.168.1.1","netmask":"255.255.255.0"}
 *
 */
DipSwitchConfig MakeDipSwitchConfig(const ::std::string& json_str);

/**
 * Create a DipSwitchIpConfig form json string.
 * @param {"ipaddr":"192.168.1.1","netmask":"255.255.255.0"}
 */
DipSwitchIpConfig MakeDipSwitchIpConfig(const ::std::string& json_str);

/**
 * @brief Get DIP switch configuration.
 *
 * @return DIP switch configuration containing:
 *         - IP address, e.g. 192.168.1.0 (the first three octets matter)
 *         - netmask
 *         - mode, (off, dhcp, static, hw-not-available)
 *         - value, the value of the last octet (0 - 255)
 */
DipSwitchConfig GetDipSwitchConfig();

/**
 * @brief Set DIP switch configuration.
 *
 * @param config The DIP switch configuration containing IP address and netmask.
 * @return Status @see Status::Ok on success.
 */
Status SetDipSwitchConfig(const DipSwitchConfig& config);


}  // namespace api
}  // namespace netconf

//---- End of header file ------------------------------------------------------

