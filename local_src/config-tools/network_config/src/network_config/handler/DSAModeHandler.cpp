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

  auto bridge_config = ::netconf::GetBridgeConfig();

  if ("br0" == bridge_config.GetBridgeOfInterface("X1") && "br0" == bridge_config.GetBridgeOfInterface("X2")) {
    ::std::cout << "0";
  } else if ("br0" == bridge_config.GetBridgeOfInterface("X1") && "br1" == bridge_config.GetBridgeOfInterface("X2")) {
    ::std::cout << "1";
  } else {
    throw ::std::runtime_error("Failed to get dsa-mode, interface X1 and X2 are not assigned to bridge br0 or br1.");
  }

}

void DSAModeHandler::SetConfig() {
  auto dsa_mode = GetValueOfSet(vm_);
  auto bridge_config = ::netconf::GetBridgeConfig();

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
    throw ::std::runtime_error("Failed to set dsa-mode, invalid parameter.");
  }

  auto status = SetBridgeConfig(bridge_config);
  if (netconf::Status::OK != status) {
    throw ::std::runtime_error("Failed to set dsa-mode, invalid bridge configuration");
  }
}

} /* namespace network_config */
