// SPDX-License-Identifier: LGPL-3.0-or-later
#pragma once

#include "Types.hpp"
#include "ConfigBase.hpp"
#include "Error.hpp"

#include <boost/optional.hpp>
#include <memory>
#include <string>
#include <vector>
#include <functional>
#include <initializer_list>


namespace netconf {
namespace api {


/**
 * @brief Container class to manage the @see IPConfig
 *
 */
class IPConfigs: public detail::ConfigBase<netconf::IPConfig> {
 public:
  IPConfigs() = default;
  explicit IPConfigs(const netconf::IPConfigs& configs);
  ~IPConfigs() = default;

  /**
   * @brief Add or replace an @see IPConfig
   * @note The config is not validated.
   *
   * @param config The IPConfig
   */
  void AddIPConfig(netconf::IPConfig config);

  /**
   * @brief Remove the configuration of an interface.
   * @note When there is no for the interface, this is a no-op.
   *
   * @param interface_name Name of the interface.
   */
  void RemoveIPConfig(const ::std::string& interface_name);

  /**
   * @brief Return the @see IPConfig of an interface
   *
   * @param interface_name Name of the interface
   * @return boost::optional<IPConfig>
   */
  boost::optional<netconf::IPConfig> GetIPConfig(const ::std::string& interface_name) const;

 private:
  ::std::string GetCompareValue(const netconf::IPConfig& config) const noexcept override;

};

/**
 * Simple builder of IP configuration from JSON string.
 *
 * @note In case of error it will only return empty object.
 *
 * @param json_str JSON input string
 * @param IPConfig object filled from JSON input, or empty in case of errors.
 * @return error [out] error of the creation operation.
 */
Error MakeIPConfigs(const ::std::string& json_str, IPConfigs& config) noexcept;

/**
 * @brief Retruns the json representation of the ip configurations.
 *
 * @return json string
 */
::std::string ToJson(const IPConfigs& configs) noexcept;

/**
 * Convert a single IP configuration to its JSON representation.
 * @param ip_config the IP config base object.
 * @return json string
 */
::std::string ToJson(const netconf::IPConfig& ip_config) noexcept;

::std::string ToString(netconf::IPSource source);

::std::string CalculateBroadcast(const netconf::IPConfig& config) noexcept;

/**
 * @brief Returns the @see IPConfigs from the netconfd network configuration daemon.
 *
 * @param IPConfigs The @see IPConfigs
 * @return Error
 */
Error GetIPConfigs(IPConfigs& config);

/**
 * @brief Returns the @see IPConfigs of a specific device type.
 * @param type filter by this type.
 *
 * @note filter can also be an oration of types, .e.g (DeviceType::Bridge|DeviceType::Wwan)
 *
 * @return The @see IPConfigs filtered by type.
 */
Error GetIPConfigs(DeviceType type, IPConfigs& configs);

/**
 * @brief Returns the current @see IPConfigs from the netconfd network configuration daemon.
 *
 * @return IPConfigs The @see IPConfigs
 */
Error GetCurrentIPConfigs(IPConfigs& configs);

/**
 * @brief Returns the current @see IPConfigs filtered by device type
 * @param type filter by this type.
 *
 * @note filter can also be an oration of types, .e.g (DeviceType::Bridge|DeviceType::Wwan)
 *
 * @return The curent @see IPConfigs filtered by type.
 */
Error GetCurrentIPConfigs(DeviceType type, IPConfigs& configs);


/**
 * @brief Set the ip configuration for the netconfd network configuration daemon.
 * @note check /var/log/messages when the ip config failed to apply.
 *
 * @param config The configuration to apply.
 * @return Status @see Status::Ok on success.
 */
Error SetIPConfigs(const IPConfigs& config);

/**
 * @brief Delete the ip configuration for one interface.
 *
 * @param interface_name
 */
void DeleteIPConfig(::std::string interface_name);

/**
 * @brief Set the Temp Fix Ip
 *
 */
Error SetTempFixIp();

}  // namespace api
}  // namespace netconf
