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

Status BridgeConfigurator::SetBridgeDownAndDelete(const Bridge &bridge) const {

  Status status = bridge_controller_.SetInterfaceDown(bridge);

  if (status.Ok()) {
    status = bridge_controller_.DeleteBridge(bridge);
  }

  return status;
}

Status BridgeConfigurator::AddBridge(const Bridge &bridge) const {
  Status status = bridge_controller_.AddBridge(bridge);

  return status;
}

Status BridgeConfigurator::RemoveAllActualBridgesThatAreNotNeeded(BridgeConfig const &config_os,
                                                                  Bridges &adapted_target_bridges) const {

  Status status;
  adapted_target_bridges.clear();
  Bridges actual_bridges = bridge_controller_.GetBridges();

  for (const auto &actual_bridge : actual_bridges) {
    if (config_os.find(actual_bridge) == config_os.end()) {

      status = SetBridgeDownAndDelete(actual_bridge);
      if (status.NotOk()) {
        return status;
      }
    } else {
      adapted_target_bridges.push_back(actual_bridge);
    }
  }

  return status;
}

Status BridgeConfigurator::RemoveAllActualBridgeInterfacesThatAreNotNeeded(Bridges const &actual_bridges,
                                                                           BridgeConfig const &config_os) const {

  Status status;
  for (const auto &actual_bridge : actual_bridges) {

    Interfaces actual_interfaces = bridge_controller_.GetBridgeInterfaces(actual_bridge);
    Interfaces const &target_interfaces = config_os.at(actual_bridge);

    for (const auto &actual_interface : actual_interfaces) {
      if (IsNotIncluded(actual_interface, target_interfaces)) {
        status = bridge_controller_.DeleteInterface(actual_bridge, actual_interface);
        if (status.NotOk()) {
          return status;
        }
      }
    }
  }

  return status;

}

Status BridgeConfigurator::AddMissingInterfacesToActualBridges(Bridges const &actual_bridges,
                                                               BridgeConfig const &config_os) const {

  Status status;
  for (const auto &actual_bridge : actual_bridges) {

    Interfaces actual_interfaces = bridge_controller_.GetBridgeInterfaces(actual_bridge);
    Interfaces const &target_interfaces = config_os.at(actual_bridge);

    for (const auto &target_interface : target_interfaces) {
      if (IsNotIncluded(target_interface, actual_interfaces)) {
        status = bridge_controller_.AddInterface(actual_bridge, target_interface);
        if (status.NotOk()) {
          return status;
        }
      }
    }
  }
  return status;
}

Status BridgeConfigurator::AddMissingBridgesAndTheirInterfaces(const Bridges &actual_bridges,
                                                               BridgeConfig const &config_os) const {

  Status status;
  for (const auto &bridge_pair : config_os) {

    auto target_bridge = bridge_pair.first;
    if (IsNotIncluded(target_bridge, actual_bridges)) {

      status = AddBridge(target_bridge);
      if (status.NotOk()) {
        return status;
      }
      auto &target_interfaces = bridge_pair.second;

      for (const auto &target_interface : target_interfaces) {
        status = bridge_controller_.AddInterface(target_bridge, target_interface);
        if (status.NotOk()) {
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

Status BridgeConfigurator::SetAllBridgesUp(BridgeConfig const &config_os) const {

  Status status;
  for (auto &bridge : config_os) {
    Status s = bridge_controller_.SetInterfaceUp(bridge.first);
    if (s.NotOk()) {
      status.Append(s.Get(), s.GetMessage());
    }
  }
  return status;
}

Status BridgeConfigurator::Configure(BridgeConfig const &config_os) const {

  Bridges actual_bridges;

  Status status = RemoveAllActualBridgesThatAreNotNeeded(config_os, actual_bridges);
  if (status.Ok()) {
    status = RemoveAllActualBridgeInterfacesThatAreNotNeeded(actual_bridges, config_os);
  }
  if (status.Ok()) {
    status = AddMissingInterfacesToActualBridges(actual_bridges, config_os);
  }
  if (status.Ok()) {
    status = AddMissingBridgesAndTheirInterfaces(actual_bridges, config_os);
  }
  if (status.Ok()) {
    status = SetAllBridgesUp(config_os);
  }

  return status;

}

Status BridgeConfigurator::SetInterfaceDown(const Interface &interface) const {

  return bridge_controller_.SetInterfaceDown(interface);
}

Status BridgeConfigurator::SetBridgeUp(const Bridge &bridge) const {

  Status status = bridge_controller_.SetInterfaceUp(bridge);

  return status;
}

Status BridgeConfigurator::SetBridgeDown(const Bridge &bridge) const {

  Status status = bridge_controller_.SetInterfaceDown(bridge);

  return status;
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
