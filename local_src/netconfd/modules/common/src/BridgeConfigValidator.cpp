// SPDX-License-Identifier: GPL-2.0-or-later

#include "BridgeConfigValidator.hpp"

#include <net/if.h>
#include <algorithm>

#include "Helper.hpp"

namespace netconf {

static Status CheckBridgeName(const ::std::string &bridge_name) {
  // Hint: A check for duplicated bridges is not required, because we use a map that
  // does not allow duplicate keys.

  // Check bridge name length
  if (bridge_name.size() > IFNAMSIZ - 1) {
    return Status { StatusCode::BRIDGE_NAME_INVALID, bridge_name };
  }
  // Checks bridge contains blanks
  if (::std::string::npos != bridge_name.find(' ')) {
    return Status { StatusCode::BRIDGE_NAME_INVALID, bridge_name };
  }

  return Status::Ok();
}

static Status CheckBridgeInterfaces(const Interfaces &bridge_interfaces, const Interfaces &available_interfaces,
                                    Interfaces &former_bridge_interfaces) {
  for (auto &interface : bridge_interfaces) {
    // Check if interface is available in the system
    if (IsNotIncluded(interface, available_interfaces)) {
      if (interface.empty()) {
        return Status { StatusCode::NAME_EMPTY };
      } else {
        return Status { StatusCode::INTERFACE_NOT_EXISTING, interface };
      }
    }

    // Check if interfaces are uniquely assigned to bridge(s)
    if (IsNotIncluded(interface, former_bridge_interfaces)) {
      former_bridge_interfaces.push_back(interface);
    } else {
      return Status { StatusCode::ENTRY_DUPLICATE, interface };
    }
  }
  return Status::Ok();
}

Status BridgeConfigValidator::Validate(BridgeConfig const &config, const Interfaces &available_interfaces) {
  Status status;
  Interfaces found_interfaces;
  if (config.empty()) {
    status.Set(StatusCode::JSON_INCOMPLETE);
  } else {
    for (auto& [bridge_name, interfaces] : config) {
      status = CheckBridgeName(bridge_name);
      if (status.IsNotOk()) {
        break;
      }
      status = CheckBridgeInterfaces(interfaces, available_interfaces, found_interfaces);
      if (status.IsNotOk()) {
        break;
      }
    }
  }
  return status;
}

} /* namespace netconf */
