// SPDX-License-Identifier: GPL-2.0-or-later

#include "DipSwitchHandler.hpp"

#include <iostream>

#include "IPConfig.hpp"

#include "Utils.hpp"

namespace network_config {

namespace po = boost::program_options;

DipSwitchHandler::DipSwitchHandler(const ::boost::program_options::variables_map &vm) : vm_{vm} {
}

bool DipSwitchHandler::Is(const ::std::string &command) {
  return Contains(vm_, command);
}

::std::string DipSwitchHandler::Get(const ::std::string &command) {
  return GetValueOf<::std::string>(vm_, command);
}

static void SetDipSwitchConfig(const ::std::string &config) {
  auto status = netconf::SetDipSwitchConfig(config);
  if (status != netconf::Status::OK) {
    throw ::std::runtime_error("Failed to set dip switch configuration.");
  }
}

static void GetDipSwitchConfig() {
  auto config = netconf::GetDipSwitchConfig();
  ::std::cout << config << std::endl;
}

void DipSwitchHandler::Execute() {
  if (Is("get")) {
    GetDipSwitchConfig();
  } else if (Is("set")) {
    SetDipSwitchConfig(Get("set"));
  }
}

}  // namespace network_config
