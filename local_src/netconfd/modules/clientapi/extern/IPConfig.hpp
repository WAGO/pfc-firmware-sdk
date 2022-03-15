// SPDX-License-Identifier: LGPL-3.0-or-later
#pragma once

#include "Types.hpp"
#include "ConfigBase.hpp"
#include "Status.hpp"

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
  virtual ~IPConfigs() = default;

  IPConfigs(const IPConfigs&) = default;
  IPConfigs& operator=(const IPConfigs&) = default;
  IPConfigs(IPConfigs&&) = default;
  IPConfigs& operator=(IPConfigs&&) = default;

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
 * @note In case of status it will only return empty object.
 *
 * @param json_str JSON input string
 * @param IPConfig object filled from JSON input, or empty in case of statuses.
 * @return status [out] status of the creation operation.
 */
Status MakeIPConfigs(const ::std::string& json_str, IPConfigs& config) noexcept;

/**
 * @brief Returns the compact json representation of the ip configurations.
 *
 * @return json string
 */
::std::string ToJson(const IPConfigs& configs) noexcept;

/**
 * @brief Returns the human readable json representation of the ip configurations.
 *
 * @return json string
 */
::std::string ToPrettyJson(const IPConfigs& configs) noexcept;

/**
 * Converts a single IP configuration to its compact json representation.
 *
 * @param ip_config the IP config base object.
 * @return json string
 */
::std::string ToJson(const netconf::IPConfig& ip_config) noexcept;

/**
 * Converts a single IP configuration to its human readable json representation.
 *
 * @param ip_config the IP config base object.
 * @return json string
 */
::std::string ToPrettyJson(const netconf::IPConfig& ip_config) noexcept;

::std::string ToString(netconf::IPSource source);

::std::string CalculateBroadcast(const netconf::IPConfig& config) noexcept;

/**
 * @brief Returns the @see IPConfigs from the netconfd network configuration daemon.
 *
 * @param IPConfigs The @see IPConfigs
 * @return Status
 */
Status GetIPConfigs(IPConfigs& config);

/**
 * @brief Returns the @see IPConfigs of a specific device type.
 * @param type filter by this type.
 *
 * @note filter can also be an oration of types, .e.g (DeviceType::Bridge|DeviceType::Wwan)
 *
 * @return The @see IPConfigs filtered by type.
 */
Status GetIPConfigs(DeviceType type, IPConfigs& configs);

/**
 * @brief Returns the current @see IPConfigs from the netconfd network configuration daemon.
 *
 * @return IPConfigs The @see IPConfigs
 */
Status GetCurrentIPConfigs(IPConfigs& configs);

/**
 * @brief Returns the current @see IPConfigs filtered by device type
 * @param type filter by this type.
 *
 * @note filter can also be an oration of types, .e.g (DeviceType::Bridge|DeviceType::Wwan)
 *
 * @return The curent @see IPConfigs filtered by type.
 */
Status GetCurrentIPConfigs(DeviceType type, IPConfigs& configs);


/**
 * @brief Set the ip configuration for the netconfd network configuration daemon.
 * @note check /var/log/messages when the ip config failed to apply.
 *
 * @param config The configuration to apply.
 * @return Status @see Status::Ok on success.
 */
Status SetIPConfigs(const IPConfigs& config);

/**
 * @brief Delete the ip configuration for one interface.
 *
 * @param interface_name
 */
void DeleteIPConfig(const ::std::string& interface_name);

/**
 * @brief Set the Temp Fix Ip
 *
 */
Status SetTempFixIp();


/**
 * Validate a candidate of new IP configuration.
 * The candidate is checked for consistency in itself and against the current configuration.
 * If the current IP configuration is empty the candidate in only check for consistency in itself.
 *
 * The current configuration should be obtained by GetCurrentIPConfigs() and passed to this function as is.
 *
 * Checks for conflicts in the candidate itself.
 * Checks for conflicts to the current IP configuration.
 * Checks for access and existence by consulting the interface information.
 *
 * @param candidate A candidate IP configuration to be validated.
 * @param current An IP configuration which is obtained by e.g. GetCurrentIPConfigs()
 * @param interface_information The interface information obtained by GetInterfaceInformation()
 * @return Result of the validation.
 */
Status ValidateIpConfigs(const IPConfigs& candidate, const IPConfigs& current, const InterfaceInformations& interface_information );

}  // namespace api
}  // namespace netconf
