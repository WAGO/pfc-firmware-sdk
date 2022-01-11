// SPDX-License-Identifier: GPL-2.0-or-later

#include <NetconfStatus.hpp>
#include "InterfaceConfigHandler.hpp"

#include <string>
#include <iostream>
#include <exception>

#include "Utils.hpp"
#include "InterfaceInformationApi.hpp"

namespace network_config {
namespace po = ::boost::program_options;
namespace napi = ::netconf::api;

InterfaceConfigHandler::InterfaceConfigHandler(const po::variables_map &vm)
    : vm_ { vm } {
}

void InterfaceConfigHandler::Execute() {
  if (vm_.count("get") > 0) {
    GetConfig();
  } else if (vm_.count("set") > 0) {
    SetConfig();
  }
}

void InterfaceConfigHandler::GetConfig() {
  napi::InterfaceConfigs interface_configs;
  auto status = napi::GetInterfaceConfigs(interface_configs);
  if (status.IsNotOk()) {
    throw NetconfStatus { status };
  }
  auto format = GetFormat(vm_);

  if (Contains(vm_, "device")) {
    auto device = GetDevice(vm_);
    auto interface_config = interface_configs.GetInterfaceConfig(device);
    if (interface_config) {
      if (format == "text") {
        ::std::cout << napi::ToString(*interface_config);
      } else if (format == "json") {
        ::std::cout << napi::ToJson(*interface_config);
      } else if (format == "pretty-json") {
        ::std::cout << napi::ToPrettyJson(*interface_config);
      }
    } else {
      throw NetconfStatus { netconf::Status { netconf::StatusCode::INTERFACE_NOT_EXISTING, device } };
    }
  } else {
    if (format == "text") {
      ::std::cout << napi::ToString(interface_configs);
    } else if (format == "json") {
      ::std::cout << napi::ToJson(interface_configs);
    } else if (format == "pretty-json") {
      ::std::cout << napi::ToPrettyJson(interface_configs);
    }
  }
}

void InterfaceConfigHandler::SetConfig() {
  auto interface_configs = CreateInterfaceConfigs();

  netconf::Status status;
  auto &options = GetOptions();
  if (Contains(vm_, options.dryrun)) {
    status = napi::ValidateInterfaceConfigs(interface_configs, napi::GetInterfaceInformation());
  } else {
    status = napi::SetInterfaceConfigs(interface_configs);
  }

  if (status.IsNotOk()) {
    throw NetconfStatus { status };
  }
}

napi::InterfaceConfigs InterfaceConfigHandler::CreateInterfaceConfigs() {

  auto value = GetValueOfSet(vm_);
  if (!value.empty()) {
    napi::InterfaceConfigs config;
    auto error = napi::MakeInterfaceConfigs(value, config);
    if (error.IsNotOk()) {
      throw NetconfStatus { error };
    }
    return config;
  }
  return napi::InterfaceConfigs();
}

} /* namespace network_config */
