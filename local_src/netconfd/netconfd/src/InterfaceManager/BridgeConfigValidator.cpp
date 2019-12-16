// SPDX-License-Identifier: GPL-2.0-or-later

#include "BridgeConfigValidator.hpp"
#include "Logger.hpp"
#include "Helper.hpp"
#include <algorithm>
#include "net/if.h"

namespace netconfd {

BridgeConfigValidator::BridgeConfigValidator(
    IBridgeController& bridge_controller)
    : bridge_controller_(bridge_controller) {

}

static Status CheckBridgeName(const ::std::string& bridge_name) {

  Status status;

  // Hint: A check for duplicated bridges is not required, because we use a map that
  // does not allow duplicate keys.

  // Check bridge name length
  if (bridge_name.size() > IFNAMSIZ - 1) {
    status.Prepend(StatusCode::INVALID_PARAMETER,
                   "Bridge name " + bridge_name + " is too long (>15)");
  }
  // Checks bridge contains blanks
  if (::std::string::npos != bridge_name.find(' ')) {
    status.Prepend(StatusCode::INVALID_PARAMETER,
                   "Bridge name " + bridge_name + " contains blanks");
  }

  return status;
}

static Status CheckMinimalInterfaceAssignmentPrecondition(
    BridgeConfig const& config) {

  if (config.find("br0") == config.end()
      || config.find("br1") == config.end()) {

    return Status(StatusCode::INVALID_CONFIG,
                  "Bridge config has to contain br0 and br1.");
  }

  auto const& br0_itfs = config.at("br0");
  auto const& br1_itfs = config.at("br1");

  if (IsNotIncluded("ethX1", br0_itfs)
      || (IsNotIncluded("ethX2", br0_itfs) && IsNotIncluded("ethX2", br1_itfs))) {

    return Status(StatusCode::INVALID_CONFIG,
                  "Bridges br0 and br1 do not contain interfaces X1 and X2.");
  }

  return Status();
}

Status BridgeConfigValidator::Validate(BridgeConfig const& config) const {

  Status status;
  Interfaces available_interfaces = bridge_controller_.GetInterfaces();
  Interfaces found_interfaces;

  status = CheckMinimalInterfaceAssignmentPrecondition(config);

  if (status.Ok()) {
    // HINT: Do not exit loop on first invalidity to log all errors.
    for (auto& bridge_pair : config) {

      Status check_status = CheckBridgeName(bridge_pair.first);
      if (check_status.NotOk()) {
        status.Append(check_status.GetMessage());
      }

      for (auto& interface : bridge_pair.second) {

        // Check if interface is available in the system
        if (IsNotIncluded(interface, available_interfaces)) {

          if (interface.empty()) {
            status.Append("Interface unnamed");
          } else {
            status.Append("Interface " + interface + " is not available");
          }
        }

        // Check if interfaces are uniquely assigned to bridge(s)
        if (IsNotIncluded(interface, found_interfaces)) {
          found_interfaces.push_back(interface);
        } else {
          status.Append(
              "Interface " + interface + " is assigned several times");
        }
      }
    }
  }

  if (status.NotOk()) {
    status.Prepend("Bridge config validation failed: ");
  }

  return status;
}

} /* namespace netconfd */
