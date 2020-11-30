// SPDX-License-Identifier: LGPL-3.0-or-later
#pragma once

#include <boost/optional.hpp>
#include <memory>
#include <string>
#include <cstring>
#include "Types.hpp"
#include "ConfigBase.hpp"
#include "Error.hpp"

namespace netconf {
namespace api {

using ::netconf::MacAddress; // Pull to api namespace

/**
 * Get the MAC address of a system interface.
 * The full system interface name must be given (e.g. wwan0)
 *
 * @param interface_name system interface name
 * @return MacAddress object.
 */
MacAddress GetMacAddress(const ::std::string &interface_name);

/**
 * Set the state of any interface in the system.
 * The full system interface name must be given (e.g. wwan0)
 * @note The InterfaceState is not saved persistently
 *
 * @param interface_name The system interface name to be set
 * @param state The new state of the interface
 * @return Error object containing status of the operation.
 */
Error SetInterfaceState(const ::std::string &interface_name, InterfaceState state);

/**
 * Get the current state of any interface in the system.
 * The full system interface name must be given (e.g. wwan0)
 *
 * @param interface_name The system interface name to be set
 * @param state Destination of the current interface state
 * @return Error object containing status of the operation.
 */
Error GetInterfaceState(const ::std::string &interface_name, InterfaceState &state);

/**
 * @brief Container class for the @see InterfaceConfig
 *
 */
class InterfaceConfigs: public detail::ConfigBase<netconf::InterfaceConfig> {
 public:
  InterfaceConfigs() = default;
  explicit InterfaceConfigs(const netconf::InterfaceConfigs& config);

  void AddInterfaceConfig(InterfaceConfig config);
  void RemoveInterfaceConfig(const ::std::string &interface_name);
  boost::optional<InterfaceConfig> GetInterfaceConfig(const ::std::string &interface_name);


 private:
  ::std::string GetCompareValue(const netconf::InterfaceConfig&) const noexcept override;

};


/**
 * Convert a base interface config to a json string.
 * @return
 */
::std::string ToJson(const netconf::InterfaceConfig& config) noexcept;

::std::string ToString(const netconf::InterfaceConfig& config, const ::std::string& sep = " ") noexcept;

::std::string ToJson(const InterfaceConfigs& configs) noexcept;
::std::string ToString(const InterfaceConfigs& configs) noexcept;

/**
 * Create a InterfaceConfigs from a json string.
 * @return InterfaceConfigs object.
 */
Error MakeInterfaceConfigs(const ::std::string &json_str, InterfaceConfigs& config);

/**
 * @brief Get the Interface Configs object from the netconfd network configuration daemon.
 *
 * @param config
 * @return Error
 */
Error GetInterfaceConfigs(InterfaceConfigs& config);

/**
 * @brief Set the Interface Configs object for the netconfd network
 *
 * @param config
 * @return Status
 */
Error SetInterfaceConfigs(const InterfaceConfigs &config);

}  // namespace api
}  // namespace netconf
