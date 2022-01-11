// SPDX-License-Identifier: GPL-2.0-or-later

#include "BridgeConfigHandler.hpp"
#include "NetconfStatus.hpp"

#include <string>
#include <iostream>
#include <exception>
#include <BridgeConfig.hpp>
#include <boost/algorithm/string.hpp>

#include "Utils.hpp"
#include "InterfaceInformationApi.hpp"

namespace po = boost::program_options;

namespace network_config {
namespace napi = ::netconf::api;

BridgeConfigHandler::BridgeConfigHandler(const po::variables_map &vm)
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

void BridgeConfigHandler::Execute() {
  execute_();
}

void BridgeConfigHandler::GetConfig() {
  napi::BridgeConfig bridge_config;
  auto error = napi::GetBridgeConfig(bridge_config);
  if (error.IsNotOk()) {
    throw NetconfStatus { error };
  }

  auto format = GetFormat(vm_);

  ::std::string device;
  if (Contains(vm_, "device")) {
    device = GetDevice(vm_);
    auto bridge_interfaces = bridge_config.GetBridgeInterfaces(device);
    ::std::cout << boost::algorithm::join(bridge_interfaces, ",");

  } else if (format == "text") {
    ::std::cout << napi::ToString(bridge_config);
  } else if (format == "json") {
    ::std::cout << napi::ToJson(bridge_config);
  } else if (format == "pretty-json") {
    ::std::cout << napi::ToPrettyJson(bridge_config);
  }
}

void BridgeConfigHandler::SetConfig() {
  napi::BridgeConfig bridge_config;
  auto status = napi::MakeBridgeConfig(GetValueOfSet(vm_), bridge_config);

  const auto &options = GetOptions();
  if (status.IsOk()) {
    if (Contains(vm_, options.dryrun)) {
      status = napi::ValidateBridgeConfig(bridge_config, napi::GetInterfaceInformation());
    } else {
      status = napi::SetBridgeConfig(bridge_config);
    }
  }

  if (status.IsNotOk()) {
    throw NetconfStatus { status };
  }
}

} /* namespace network_config */
