// SPDX-License-Identifier: GPL-2.0-or-later

#include "DipSwitchHandler.hpp"

#include <iostream>

#include "DipSwitchConfig.hpp"

#include "Utils.hpp"

namespace network_config
{

  namespace po = boost::program_options;

  DipSwitchHandler::DipSwitchHandler(const ::boost::program_options::variables_map &vm) :
          vm_{vm}
  {
  }

  bool DipSwitchHandler::Is(const ::std::string &command)
  {
    return Contains(vm_, command);
  }

  ::std::string DipSwitchHandler::Get(const ::std::string &command)
  {
    return GetValueOf(vm_, command);
  }

  void DipSwitchHandler::SetDipSwitchConfig(const ::std::string &config)
  {
    auto dip_switch_ip_config = netconf::api::MakeDipSwitchIpConfig(config);
    auto dsc = netconf::api::DipSwitchConfig{dip_switch_ip_config};

    auto status = netconf::api::SetDipSwitchConfig(dsc);
    if(status != netconf::api::Status::OK)
    {
      throw ::std::runtime_error("Failed to set dip switch configuration.");
    }
  }

  void DipSwitchHandler::ParseConfig(netconf::api::DipSwitchConfig &config)
  {
    auto dip_switch_config = config.GetDipSwitchConfig();
    auto format = GetFormat(vm_);
    ::std::stringstream output;
    if(format == "text")
    {
      output << ToString(config);
    }
    else if(format == "json")
    {
      output << ToJson(config);
    }

    ::std::cout << output.str() << ::std::flush;
  }

  void DipSwitchHandler::GetDipSwitchConfig()
  {
    auto config = netconf::api::GetDipSwitchConfig();
    ParseConfig(config);
  }

  void DipSwitchHandler::Execute()
  {
    if(Is("get"))
    {
      GetDipSwitchConfig();
    }
    else if(Is("set"))
    {
      SetDipSwitchConfig(Get("set"));
    }
  }

}  // namespace network_config
