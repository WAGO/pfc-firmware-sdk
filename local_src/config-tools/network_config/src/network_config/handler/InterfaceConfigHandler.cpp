// SPDX-License-Identifier: GPL-2.0-or-later

#include "InterfaceConfigHandler.hpp"

#include <string>
#include <iostream>
#include <exception>

#include "Utils.hpp"


namespace network_config {
namespace po = ::boost::program_options;
namespace napi = ::netconf::api;

InterfaceConfigHandler::InterfaceConfigHandler(const po::variables_map &vm)
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

void InterfaceConfigHandler::Execute() {
  execute_();
}

void InterfaceConfigHandler::GetConfig() {
  auto interface_configs = napi::GetInterfaceConfigs();
  auto format = GetFormat(vm_);

  if (Contains(vm_, "device")) {
    auto device = GetDevice(vm_);
    auto interface_config = interface_configs.GetInterfaceConfig(device);
    if (interface_config) {
      if (format == "text") {
        ::std::cout << napi::ToString(*interface_config);
      } else if (format == "json") {
        ::std::cout << napi::ToJson(*interface_config);
      }
    }
    else
    {
      throw ::std::runtime_error("The device \"" + device + "\" is unknown or not given in the interface config.");
    }
  } else {
    if (format == "text") {
      ::std::cout << napi::ToString(interface_configs);
    } else if (format == "json") {
      ::std::cout << napi::ToJson(interface_configs);
    }
  }
}

void InterfaceConfigHandler::SetConfig() {
  auto interface_configs = CreateInterfaceConfigs();
  auto status = napi::SetInterfaceConfigs(interface_configs);

  if (napi::Status::OK != status) {
    throw ::std::runtime_error("Failed to set interface configuration.");
  }
}

napi::InterfaceConfigs InterfaceConfigHandler::CreateInterfaceConfigs() {

  auto value = GetValueOfSet(vm_);
  if (!value.empty()) {
    return napi::MakeInterfaceConfigs(value);
  }
  return napi::InterfaceConfigs();
}

} /* namespace network_config */
