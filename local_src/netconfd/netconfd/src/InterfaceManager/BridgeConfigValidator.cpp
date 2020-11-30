// SPDX-License-Identifier: GPL-2.0-or-later

#include "BridgeConfigValidator.hpp"

#include <net/if.h>
#include <algorithm>

#include "Helper.hpp"
#include "Logger.hpp"

namespace netconf {

BridgeConfigValidator::BridgeConfigValidator(IBridgeController &bridge_controller)
    : bridge_controller_(bridge_controller) {
}

static Error CheckBridgeName(const ::std::string &bridge_name) {
  // Hint: A check for duplicated bridges is not required, because we use a map that
  // does not allow duplicate keys.

  // Check bridge name length
  if (bridge_name.size() > IFNAMSIZ - 1) {
    return Error { ErrorCode::BRIDGE_NAME_INVALID, bridge_name };
  }
  // Checks bridge contains blanks
  if (::std::string::npos != bridge_name.find(' ')) {
    return Error { ErrorCode::BRIDGE_NAME_INVALID, bridge_name };
  }

  return Error::Ok();
}

static Error CheckBridgeInterfaces(const Interfaces& bridge_interfaces, const Interfaces& available_interfaces, Interfaces& former_bridge_interfaces){
  for (auto &interface : bridge_interfaces) {
    // Check if interface is available in the system
    if (IsNotIncluded(interface, available_interfaces)) {
      if (interface.empty()) {
        return Error{ErrorCode::NAME_EMPTY};
      } else {
        return Error{ErrorCode::INTERFACE_NOT_EXISTING ,interface};
      }
    }

    // Check if interfaces are uniquely assigned to bridge(s)
    if (IsNotIncluded(interface, former_bridge_interfaces)) {
      former_bridge_interfaces.push_back(interface);
    } else {
      return Error{ErrorCode::ENTRY_DUPLICATE, interface };
    }
  }
  return Error::Ok();
}

Error BridgeConfigValidator::Validate(BridgeConfig const &os_config) const {
  Error error;
  Interfaces available_interfaces = bridge_controller_.GetInterfaces();
  Interfaces found_interfaces;

  // HINT: Do not exit loop on first invalidity to log all errors.
  for (auto &[bridge_name, interfaces] : os_config) {
    error = CheckBridgeName(bridge_name);
    if (error.IsNotOk()) {
      break;
    }
    error = CheckBridgeInterfaces(interfaces, available_interfaces, found_interfaces);
    if( error.IsNotOk())
    {
      break;
    }
  }

  return error;
}

} /* namespace netconf */
