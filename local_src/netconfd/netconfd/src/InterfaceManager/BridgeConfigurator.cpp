// SPDX-License-Identifier: GPL-2.0-or-later

#include "BridgeConfigurator.hpp"

#include <algorithm>

#include "CommandExecutor.hpp"
#include "Logger.hpp"
#include "Helper.hpp"

namespace netconf {

BridgeConfigurator::BridgeConfigurator(IBridgeController &bridge_controller)
    : bridge_controller_ { bridge_controller } {

}

Error BridgeConfigurator::SetBridgeDownAndDelete(const Bridge &bridge) const {

  Error status = bridge_controller_.SetInterfaceDown(bridge);

  if (status.IsOk()) {
    status = bridge_controller_.DeleteBridge(bridge);
  }

  return status;
}

Error BridgeConfigurator::AddBridge(const Bridge &bridge) const {
  Error status = bridge_controller_.AddBridge(bridge);

  return status;
}

Error BridgeConfigurator::RemoveAllActualBridgesThatAreNotNeeded(BridgeConfig const &config_os,
                                                                 Bridges &adapted_target_bridges) const {

  Error status;
  adapted_target_bridges.clear();
  Bridges actual_bridges = bridge_controller_.GetBridges();

  for (const auto &actual_bridge : actual_bridges) {
    if (config_os.find(actual_bridge) == config_os.end()) {

      status = SetBridgeDownAndDelete(actual_bridge);
      if (status.IsNotOk()) {
        return status;
      }
    } else {
      adapted_target_bridges.push_back(actual_bridge);
    }
  }

  return status;
}

Error BridgeConfigurator::RemoveAllActualBridgeInterfacesThatAreNotNeeded(Bridges const &actual_bridges,
                                                                          BridgeConfig const &config_os) const {

  Error status;
  for (const auto &actual_bridge : actual_bridges) {

    Interfaces actual_interfaces = bridge_controller_.GetBridgeInterfaces(actual_bridge);
    Interfaces const &target_interfaces = config_os.at(actual_bridge);

    for (const auto &actual_interface : actual_interfaces) {
      if (IsNotIncluded(actual_interface, target_interfaces)) {
        status = bridge_controller_.DeleteInterface(actual_bridge, actual_interface);
        if (status.IsNotOk()) {
          return status;
        }
      }
    }
  }

  return status;

}

Error BridgeConfigurator::AddMissingInterfacesToActualBridges(Bridges const &actual_bridges,
                                                              BridgeConfig const &config_os) const {

  Error status;
  for (const auto &actual_bridge : actual_bridges) {

    Interfaces actual_interfaces = bridge_controller_.GetBridgeInterfaces(actual_bridge);
    Interfaces const &target_interfaces = config_os.at(actual_bridge);

    for (const auto &target_interface : target_interfaces) {
      if (IsNotIncluded(target_interface, actual_interfaces)) {
        status = bridge_controller_.AddInterface(actual_bridge, target_interface);
        if (status.IsNotOk()) {
          return status;
        }
      }
    }
  }
  return status;
}

Error BridgeConfigurator::AddMissingBridgesAndTheirInterfaces(const Bridges &actual_bridges,
                                                              BridgeConfig const &config_os) const {

  Error status;
  for (const auto& [bridge, interfaces] : config_os) {

    auto target_bridge = bridge;
    if (IsNotIncluded(target_bridge, actual_bridges)) {

      status = AddBridge(target_bridge);
      if (status.IsNotOk()) {
        return status;
      }
      auto &target_interfaces = interfaces;

      for (const auto &target_interface : target_interfaces) {
        status = bridge_controller_.AddInterface(target_bridge, target_interface);
        if (status.IsNotOk()) {
          return status;
        }
      }
    }
  }

  return status;

}

BridgeConfig BridgeConfigurator::GetConfiguration() const {

  BridgeConfig config_os;

  Bridges bridges = bridge_controller_.GetBridges();

  for (const auto &bridge : bridges) {
    BridgePair bridge_pair;

    bridge_pair.first = bridge;
    bridge_pair.second = bridge_controller_.GetBridgeInterfaces(bridge);

    config_os.insert(bridge_pair);
  }

  return config_os;

}

Error BridgeConfigurator::SetAllBridgesUp(BridgeConfig const &config_os) const {

  Error error;
  for (auto& [bridge, interfaces] : config_os) {
    Error er = bridge_controller_.SetInterfaceUp(bridge);
    if (er.IsNotOk()) {
      Error user_error { ErrorCode::SET_INTERFACE_STATE, bridge };
      LogError(user_error.ToString() + " " + er.ToString());
      error = user_error;
    }
  }
  return error;
}

Error BridgeConfigurator::Configure(BridgeConfig const &config_os) const {

  Bridges actual_bridges;

  Error status = RemoveAllActualBridgesThatAreNotNeeded(config_os, actual_bridges);
  if (status.IsOk()) {
    status = RemoveAllActualBridgeInterfacesThatAreNotNeeded(actual_bridges, config_os);
  }
  if (status.IsOk()) {
    status = AddMissingInterfacesToActualBridges(actual_bridges, config_os);
  }
  if (status.IsOk()) {
    status = AddMissingBridgesAndTheirInterfaces(actual_bridges, config_os);
  }
  if (status.IsOk()) {
    status = SetAllBridgesUp(config_os);
  }

  return status;

}

Error BridgeConfigurator::SetInterfaceDown(const Interface &interface) const {

  Error system_error = bridge_controller_.SetInterfaceDown(interface);
  if (system_error.IsNotOk()) {
    Error user_error { ErrorCode::SET_INTERFACE_STATE, interface };
    LogError(user_error.ToString() + " " + system_error.ToString());
    return user_error;
  }

  return Error::Ok();
}

Interfaces BridgeConfigurator::GetBridgeAssignedInterfaces() const {
  Interfaces interfaces;
  BridgeConfig bridges = GetConfiguration();
  for (const auto &bridge : bridges) {
    for (const auto &interface : bridge.second) {
      interfaces.push_back(interface);
    }
  }
  return interfaces;
}

}

//---- End of source file ------------------------------------------------------
