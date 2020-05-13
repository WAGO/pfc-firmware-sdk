// SPDX-License-Identifier: GPL-2.0-or-later

#include "IPConfigHandler.hpp"

#include <string>
#include <iostream>
#include <exception>

#include "Utils.hpp"
#include "OptionStrings.hpp"

namespace po = boost::program_options;

namespace network_config
{
  IPConfigHandler::IPConfigHandler(const OptionParser& parser) :
          vm_{parser.GetVariableMap()},
          parser_{parser}
  {
    if(vm_.count("get") > 0)
    {
      execute_ = [this]()
      { this->GetConfig();};
    }
    else if(vm_.count("get-current") > 0)
    {
      execute_ = [this]()
      { this->GetCurrentConfig();};
    }
    else if(vm_.count("set") > 0)
    {
      execute_ = [this]()
      { this->SetConfig();};
    }
  }

  void IPConfigHandler::Execute()
  {
    execute_();
  }

  static ::std::string IPSourceToString(::netconf::IPSource source)
  {
    ::std::string value = "unknown";
    if(source == ::netconf::IPSource::NONE)
    {
      value = "none";
    }
    else if(source == ::netconf::IPSource::STATIC)
    {
      value = "static";
    }
    else if(source == ::netconf::IPSource::DHCP)
    {
      value = "dhcp";
    }
    else if(source == ::netconf::IPSource::BOOTP)
    {
      value = "bootp";
    }
    else if(source == ::netconf::IPSource::EXTERNAL)
    {
      value = "external";
    }
    else if(source == ::netconf::IPSource::TEMPORARY)
    {
      value = "temporary";
    }
    return value;
  }



  void IPConfigHandler::ParseConfig(netconf::IPConfigs &ip_configs)
  {
    auto format = GetFormat(vm_);
    const auto& fields = GetFields();

    ::std::stringstream output;

    if(format == "text")
    {
      for(auto &ip_config : ip_configs.Get())
      {
        if(Contains(vm_, fields.device))
        {
          auto device = Get(vm_, fields.device);
          if(device == ip_config.interface_)
          {
            output << "device=" << ip_config.interface_;
            output << " source=" << IPSourceToString(ip_config.source_);
            output << " ipaddr=" << ip_config.address_;
            output << " netmask=" << ip_config.netmask_;
            output << " bcast=" << ip_config.broadcast_;
            output << " ";
          }
        }
        else
        {
          output << "device=" << ip_config.interface_;
          output << " source=" << IPSourceToString(ip_config.source_);
          output << " ipaddr=" << ip_config.address_;
          output << " netmask=" << ip_config.netmask_;
          output << " bcast=" << ip_config.broadcast_;
          output << " ";
        }
      }
    }
    else if(format == "json")
    {
      if(Contains(vm_, fields.device))
      {
        auto device = Get(vm_, fields.device);
        auto ip_config_optional = ip_configs.GetIPConfig(device);
        netconf::IPConfigs configs;
        if(ip_config_optional)
        {
          configs.AddIPConfig(ip_config_optional.get());
        }
        output << configs.ToJson();
      }
      else
      {
        output << ip_configs.ToJson();
      }
    }

    ::std::cout << output.str();
    ::std::cout.flush();
  }

  void IPConfigHandler::GetCurrentConfig()
  {
    auto ip_configs = ::netconf::GetCurrentIPConfigs();
    ParseConfig(ip_configs);
  }

  void IPConfigHandler::GetConfig()
  {
    auto ip_configs = ::netconf::GetIPConfigs();
    ParseConfig(ip_configs);
  }

  void IPConfigHandler::SetConfig()
  {
    auto ip_configs = CreateIPConfigs();
    auto status = SetIPConfigs(ip_configs);

    if(netconf::Status::OK != status)
    {
      throw ::std::runtime_error("Failed to set IP configuration.");
    }
  }

  ::netconf::IPConfigs IPConfigHandler::CreateIPConfigs()
  {

    auto value = GetValueOfSet(vm_);
    if(!value.empty())
    {
      return ::netconf::IPConfigs(value);
    }

    auto ip_config = ::netconf::IPConfig();

    if(vm_.count("device") > 0)
    {
      ip_config.interface_ = vm_["device"].as<::std::string>();
    }

    if(vm_.count("ipaddr") > 0)
    {
      ip_config.address_ = vm_["address"].as<::std::string>();
    }

    if(vm_.count("netmask") > 0)
    {
      ip_config.netmask_ = vm_["netmask"].as<::std::string>();
    }

    if(vm_.count("prefix") > 0)
    {
      throw ::std::logic_error("Option prefix is not implemented yet");
    }

    auto ip_configs = ::netconf::IPConfigs();
    ip_configs.AddIPConfig(ip_config);

    return ip_configs;
  }

} /* namespace network_config */
