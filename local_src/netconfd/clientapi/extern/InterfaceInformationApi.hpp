// SPDX-License-Identifier: GPL-2.0-or-later
#pragma once

#include "InterfaceInformation.hpp"
#include <string>

namespace netconf {
namespace api {

using ::netconf::InterfaceInformations; // Pull to API namespace
using ::netconf::InterfaceInformation;  // Pull to API namespace

/**
 * Get the InterfaceInformation of each interface of the netconfd.
 *
 * @return a vector of interface configuration objects.
 */
InterfaceInformations GetInterfaceInformation();

/**
 * Get the information of interfaces of a certain type or type combination.
 *
 * @param type The type or combination of types
 * @return vector of @ref InterfaceInformation objects.
 */
InterfaceInformations GetInterfaceInformation(DeviceType type);

::std::string ToString(const InterfaceInformation& info) noexcept;
::std::string ToString(const InterfaceInformations& info) noexcept;

::std::string ToJson(const InterfaceInformation& info) noexcept;
::std::string ToJson(const InterfaceInformations& info) noexcept;



} // namespace api
} // namespace netconf
