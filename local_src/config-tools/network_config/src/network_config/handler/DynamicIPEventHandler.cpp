// SPDX-License-Identifier: GPL-2.0-or-later

#include "DynamicIPEventHandler.hpp"
#include "NetconfStatus.hpp"

#include <string>
#include <exception>
#include <Event.hpp>

#include "Utils.hpp"

namespace network_config {

namespace napi = ::netconf::api;

DynamicIPEventHandler::DynamicIPEventHandler(const ::boost::program_options::variables_map &vm)
    : vm_ { vm } {

  if (vm_.count("set") > 0) {
    execute_ = [this]() {
      this->SetConfig();
    };
  }

}

void DynamicIPEventHandler::Execute() {
  execute_();
}

void DynamicIPEventHandler::SetConfig() {

  ::std::string interface;
  netconf::DynamicIPEventAction action;

  auto status = napi::MakeDynamicIPAction(GetValueOfSet(vm_), interface, action);

  if (status.IsOk()) {
    status = napi::NotifyDynamicIPAction(interface, action);
  }

  if (status.IsNotOk()) {
    throw NetconfStatus { status };
  }
}

}
