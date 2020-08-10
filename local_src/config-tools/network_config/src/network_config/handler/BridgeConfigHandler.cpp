// SPDX-License-Identifier: GPL-2.0-or-later

#include "BridgeConfigHandler.hpp"

#include <string>
#include <iostream>
#include <exception>
#include <BridgeConfig.hpp>
#include <boost/algorithm/string.hpp>

#include "Utils.hpp"

namespace po = boost::program_options;

namespace network_config
{
  namespace napi = ::netconf::api;

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
    auto bridge_config = napi::GetBridgeConfig();
    auto format = GetFormat(vm_);

    ::std::string device;
    if(Contains(vm_, "device"))
    {
      device = GetDevice(vm_);
      auto bridge_interfaces = bridge_config.GetBridgeInterfaces(device);
      ::std::cout << boost::algorithm::join(bridge_interfaces,",");

    }
    else if(format == "text")
    {
      ::std::cout << napi::ToString(bridge_config);
    }
    else if(format == "json")
    {
      ::std::cout << napi::ToJson(bridge_config);
    }
  }

  void BridgeConfigHandler::SetConfig()
  {
    auto bridge_config = napi::MakeBridgeConfig(GetValueOfSet(vm_));
    auto status = napi::SetBridgeConfig(bridge_config);

    if(netconf::api::Status::OK != status)
    {
      throw ::std::runtime_error("Failed to set bridge configuration.");
    }
  }

} /* namespace network_config */
