// SPDX-License-Identifier: GPL-2.0-or-later

#include "DipSwitchHandler.hpp"

#include <iostream>

#include "DipSwitchConfig.hpp"

#include "Utils.hpp"
#include "NetconfError.hpp"

namespace network_config
{

  namespace po = boost::program_options;
  namespace napi = ::netconf::api;

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
    netconf::DipSwitchIpConfig dip_switch_ip_config;
    auto error = napi::MakeDipSwitchIpConfig(config, dip_switch_ip_config);
    if(error.IsNotOk())
    {
      throw NetconfError{error};
    }

    auto dsc = netconf::DipSwitchConfig{dip_switch_ip_config};
    error = napi::SetDipSwitchConfig(dsc);
    if(error.IsNotOk())
    {
      throw NetconfError{error};
    }
  }

  void DipSwitchHandler::ParseConfig(netconf::DipSwitchConfig &config)
  {
    auto format = GetFormat(vm_);
    ::std::stringstream output;
    if(format == "text")
    {
      output << napi::ToString(config);
    }
    else if(format == "json")
    {
      output << napi::ToJson(config);
    }

    ::std::cout << output.str() << ::std::flush;
  }

  void DipSwitchHandler::GetDipSwitchConfig()
  {
    netconf::DipSwitchConfig config;
    auto error = napi::GetDipSwitchConfig(config);
    if(error.IsNotOk())
    {
      throw NetconfError{error};
    }
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
