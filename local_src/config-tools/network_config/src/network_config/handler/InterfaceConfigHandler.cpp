// SPDX-License-Identifier: GPL-2.0-or-later

#include "InterfaceConfigHandler.hpp"

#include <string>
#include <iostream>
#include <exception>

#include "Utils.hpp"

namespace po = boost::program_options;

namespace network_config
{
  InterfaceConfigHandler::InterfaceConfigHandler(const po::variables_map &vm) :
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

  void InterfaceConfigHandler::Execute()
  {
    execute_();
  }

  void InterfaceConfigHandler::GetConfig()
  {
    auto interface_config = ::netconf::GetInterfaceConfigs();
    auto format = GetFormat(vm_);

    if(format == "text")
    {
      ::std::cout << interface_config.ToString();
    }
    else if(format == "json")
    {
      ::std::cout << interface_config.ToJson();
    }
  }

  void InterfaceConfigHandler::SetConfig()
  {
    auto interface_configs = CreateInterfaceConfigs();
    auto status = SetInterfaceConfigs(interface_configs);

    if(netconf::Status::OK != status)
    {
      throw ::std::runtime_error("Failed to set interface configuration.");
    }
  }

  ::netconf::InterfaceConfigs InterfaceConfigHandler::CreateInterfaceConfigs()
  {

    auto value = GetValueOfSet(vm_);
    if(!value.empty())
    {
      return ::netconf::InterfaceConfigs(value);
    }
    return ::netconf::InterfaceConfigs();
  }

} /* namespace network_config */
