// SPDX-License-Identifier: GPL-2.0-or-later

#include "DSAModeHandler.hpp"

#include <string>
#include <iostream>
#include <exception>

#include "BridgeConfig.hpp"
#include "Utils.hpp"

namespace network_config {
DSAModeHandler::DSAModeHandler(const ::boost::program_options::variables_map &vm)
    :
    vm_ { vm } {

  if (vm_.count("get") > 0) {
    execute_ = [this]() {
      this->GetConfig();
    };
  } else if (vm_.count("set") > 0) {
    execute_ = [this]() {
      this->SetConfig();
    };
  }
}

void DSAModeHandler::Execute() {
  execute_();
}

void DSAModeHandler::GetConfig() {
  auto bridge_config = ::netconf::api::GetBridgeConfig();
  auto bridgeOfX1 = bridge_config.GetBridgeOfInterface("X1");
  auto bridgeOfX2 = bridge_config.GetBridgeOfInterface("X2");

  if (bridgeOfX1 == "br0" && bridgeOfX2 == "br0") {
    ::std::cout << "0";
  } else if (bridgeOfX1 == "br0" && bridgeOfX2 == "br1") {
    ::std::cout << "1";
  } else {
    throw ::std::runtime_error("There is no valid dsa mode because interface X1 and/or X2 are not assigned to bridge br0 or br1.\nYou should check the bridge configuration with 'network_config --get --bridge-config'.");
  }
}

void DSAModeHandler::SetConfig() {
  auto dsa_mode = GetValueOfSet(vm_);
  auto bridge_config = ::netconf::api::GetBridgeConfig();

  if (dsa_mode == "0") {
    bridge_config.AddBridge("br0");
    bridge_config.AssignInterfaceToBridge("X1", "br0");
    bridge_config.AssignInterfaceToBridge("X2", "br0");
    if (bridge_config.BridgeIsEmpty("br1")) {
      bridge_config.DeleteBridge("br1");
    }
  } else if (dsa_mode == "1") {
    bridge_config.AddBridge("br0");
    bridge_config.AssignInterfaceToBridge("X1", "br0");
    bridge_config.AddBridge("br1");
    bridge_config.AssignInterfaceToBridge("X2", "br1");
  } else {
    throw ::std::runtime_error("Can not set the dsa mode because '" + dsa_mode + "' is no valid value.\nYour options are '0' for switched and '1' for separated mode.");
  }

  auto status = SetBridgeConfig(bridge_config);
  if (netconf::api::Status::OK != status) {
    throw ::std::runtime_error("Failed to set the dsa mode because the corresponding bridge configuration could not be applied.");
  }
}

} /* namespace network_config */
