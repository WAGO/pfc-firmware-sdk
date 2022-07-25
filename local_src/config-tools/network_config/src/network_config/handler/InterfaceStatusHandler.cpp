// SPDX-License-Identifier: GPL-2.0-or-later

#include "InterfaceStatusHandler.hpp"

#include <iostream>
#include <string>
#include <exception>
#include <boost/optional/optional.hpp>

#include "InterfaceConfig.hpp"
#include "Utils.hpp"

namespace po = boost::program_options;

namespace network_config {

InterfaceStatusHandler::InterfaceStatusHandler(const po::variables_map &vm)
    : vm_ { vm } {
}

void InterfaceStatusHandler::Execute() {

  netconf::InterfaceStatuses statuses;
  netconf::api::GetInterfaceStatuses(statuses);

  auto format = GetFormat(vm_);

  if (format == "text") {
      ::std::cout << netconf::api::ToString(statuses);
  } else if (format == "json") {
    ::std::cout << netconf::api::ToJson(statuses);
  } else if (format == "pretty-json") {
    ::std::cout << netconf::api::ToPrettyJson(statuses);
  }

}

} /* namespace network_config */
