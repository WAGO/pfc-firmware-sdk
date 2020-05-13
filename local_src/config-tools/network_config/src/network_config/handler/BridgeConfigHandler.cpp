// SPDX-License-Identifier: GPL-2.0-or-later

#include "BridgeConfigHandler.hpp"

#include <string>
#include <iostream>
#include <exception>

#include "BridgeConfig.hpp"
#include "Utils.hpp"

namespace po = boost::program_options;

namespace network_config
{

  BridgeConfigHandler::BridgeConfigHandler(const po::variables_map &vm) :
          vm_{vm}
  {

    if(vm_.count("get") > 0)
    {
      execute_ = [this]()
      { this->GetConfig();};
    }
    else if(vm_.count("set") > 0)
    {
      execute_ = [this]()
      { this->SetConfig();};
    }
  }

  void BridgeConfigHandler::Execute()
  {
    execute_();
  }

  void BridgeConfigHandler::GetConfig()
  {
    auto bridge_config = netconf::GetBridgeConfig();
    auto format = GetFormat(vm_);

    ::std::string device;
    if(Contains(vm_, "device"))
    {
      device = GetDevice(vm_);
      ::std::cout << bridge_config.GetBridgeInterfaces(device);
    }
    else if(format == "text")
    {
      ::std::cout << bridge_config.ToString();
    }
    else if(format == "json")
    {
      ::std::cout << bridge_config.ToJson();
    }
  }

  void BridgeConfigHandler::SetConfig()
  {
    auto bridge_config = netconf::BridgeConfig(GetValueOfSet(vm_));
    auto status = netconf::SetBridgeConfig(bridge_config);

    if(netconf::Status::OK != status)
    {
      throw ::std::runtime_error("Failed to set bridge configuration.");
    }
  }

} /* namespace network_config */
