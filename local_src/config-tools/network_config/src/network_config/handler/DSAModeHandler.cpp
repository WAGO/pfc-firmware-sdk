// SPDX-License-Identifier: GPL-2.0-or-later

#include "DSAModeHandler.hpp"

#include <string>
#include <iostream>
#include <exception>

#include "BridgeConfig.hpp"
#include "Utils.hpp"
#include "NetconfStatus.hpp"

namespace network_config {

namespace napi = ::netconf::api;

DSAModeHandler::DSAModeHandler(const ::boost::program_options::variables_map &vm)
    : vm_ { vm } {

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
  napi::BridgeConfig bridge_config;
  auto error = napi::GetBridgeConfig(bridge_config);
  if (error.IsNotOk()) {
    throw NetconfStatus(error);
  }

  auto bridgeOfX1 = bridge_config.GetBridgeOfInterface("X1");
  auto bridgeOfX2 = bridge_config.GetBridgeOfInterface("X2");

  if (bridgeOfX1 == "br0" && bridgeOfX2 == "br0") {
    ::std::cout << "0";
  } else if (bridgeOfX1 == "br0" && bridgeOfX2 == "br1") {
    ::std::cout << "1";
  } else {
    netconf::Status e { netconf::StatusCode::GENERIC_ERROR,
        "There is no valid dsa mode because interface X1 and/or X2 are not assigned to bridge br0 or br1." };
    throw NetconfStatus { e };
  }
}

void DSAModeHandler::SetConfig() {
  auto dsa_mode = GetValueOfSet(vm_);
  napi::BridgeConfig bridge_config;
  auto error = ::netconf::api::GetBridgeConfig(bridge_config);
  if (error.IsNotOk()) {
    throw NetconfStatus(error);
  }

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
    netconf::Status e { netconf::StatusCode::GENERIC_ERROR, "Can not set the dsa mode because '" + dsa_mode
        + "' is no valid value. Your options are '0' for switched and '1' for separated mode." };
  }

  error = napi::SetBridgeConfig(bridge_config);
  if (error.IsNotOk()) {
    throw NetconfStatus { error };
  }
}

} /* namespace network_config */
