// SPDX-License-Identifier: GPL-2.0-or-later

#include "DeviceInfoHandler.hpp"

#include <iostream>
#include <string>
#include <exception>

#include "InterfaceConfig.hpp"

namespace po = boost::program_options;

namespace network_config
{

  DeviceInfoHandler::DeviceInfoHandler(const po::variables_map &vm) :
          vm_{vm}
  {
    (void)vm_;
    execute_ = []()
    {
      auto device_info = netconf::GetInterfaceInfo();
      ::std::cout << device_info.ToJson();
    };
  }

  void DeviceInfoHandler::Execute()
  {
    execute_();
  }

} /* namespace network_config */
