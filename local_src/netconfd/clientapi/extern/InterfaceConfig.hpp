// SPDX-License-Identifier: GPL-2.0-or-later
#pragma once

#include "Status.hpp"

#include <boost/optional.hpp>
#include <memory>
#include <string>
#include <cstring>
#include "Types.hpp"
#include "ConfigBase.hpp"

namespace netconf {
namespace api {

using ::netconf::MacAddress; // Pull to api namespace

MacAddress GetMacAddress(const ::std::string &interface_name);

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
InterfaceConfigs MakeInterfaceConfigs(const ::std::string& json_str);

/**
 * @brief Get the Interface Configs object from the netconfd network configuration daemon.
 *
 * @return InterfaceConfigs Current @see InterfaceConfigs
 */
InterfaceConfigs GetInterfaceConfigs();

/**
 * @brief Set the Interface Configs object for the netconfd network
 *
 * @param config
 * @return Status
 */
Status SetInterfaceConfigs(const InterfaceConfigs &config);

}  // namespace api
}  // namespace netconf
