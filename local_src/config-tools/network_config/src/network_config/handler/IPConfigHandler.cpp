// SPDX-License-Identifier: GPL-2.0-or-later

#include "IPConfigHandler.hpp"

#include <string>
#include <iostream>
#include <exception>
#include <utility>
#include <boost/optional/optional.hpp>

#include "Utils.hpp"
#include "OptionStrings.hpp"
#include "NetconfError.hpp"

namespace po = boost::program_options;

namespace network_config
{

  namespace napi = ::netconf::api;

  IPConfigHandler::IPConfigHandler(const OptionParser &parser) :
          vm_{parser.GetVariableMap()},
          parser_{parser}
  {
    const auto &options = GetOptions();

    TypeFilter type_filter_opt;
    if(Contains(vm_, options.filter))
    {
      type_filter_opt = netconf::DeviceTypeFromString(Get(vm_, options.filter));
    }

    if(vm_.count("get") > 0)
    {
      execute_ = [this,
                  filter=type_filter_opt]()
      {
        this->GetConfig(filter);
      };
    }
    else if(vm_.count("get-current") > 0)
    {
      execute_ = [this,
                  filter=type_filter_opt]()
      {
        this->GetCurrentConfig(filter);
      };
    }
    else if(vm_.count("set") > 0)
    {
      execute_ = [this]()
      {
        this->SetConfig();
      };
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

  void IPConfigHandler::ParseConfig(netconf::api::IPConfigs &ip_configs)
  {
    auto format = GetFormat(vm_);
    const auto &fields = GetFields();

    ::std::stringstream output;
    boost::optional<netconf::DeviceType> type_filter_opt;

    if(format == "text")
    {
      for(auto &ip_config : ip_configs.GetConfig())
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
            output << " bcast=" << netconf::api::CalculateBroadcast(ip_config);
            output << " ";
          }
        }
        else
        {
          output << "device=" << ip_config.interface_;
          output << " source=" << IPSourceToString(ip_config.source_);
          output << " ipaddr=" << ip_config.address_;
          output << " netmask=" << ip_config.netmask_;
          output << " bcast=" << netconf::api::CalculateBroadcast(ip_config);
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
        if(ip_config_optional)
        {
          output << napi::ToJson(*ip_config_optional);
        }
      }
      else
      {
        output << napi::ToJson(ip_configs);
      }
    }

    ::std::cout << output.str();
    ::std::cout.flush();
  }

  void IPConfigHandler::GetCurrentConfig(TypeFilter filter)
  {

    napi::IPConfigs ip_configs;
    if(filter)
    {
      auto error = napi::GetCurrentIPConfigs(*filter, ip_configs);
      if(error.IsNotOk())
      {
        throw NetconfError{error};
      }
      ParseConfig(ip_configs);
    }
    else
    {
      auto error = ::netconf::api::GetCurrentIPConfigs(ip_configs);
      if(error.IsNotOk())
      {
        throw NetconfError{error};
      }
      ParseConfig(ip_configs);
    }
  }

  void IPConfigHandler::GetConfig(TypeFilter filter)
  {

    napi::IPConfigs ip_configs;
    if(filter)
    {
      auto error = napi::GetIPConfigs(*filter, ip_configs);
      if(error.IsNotOk()){
        throw NetconfError{error};
      }
      ParseConfig(ip_configs);
    }
    else
    {
      auto error = napi::GetIPConfigs(ip_configs);
      if(error.IsNotOk()){
        throw NetconfError{error};
      }
      ParseConfig(ip_configs);
    }

  }

  void IPConfigHandler::SetConfig()
  {
    auto ip_configs = CreateIPConfigs();
    auto error = SetIPConfigs(ip_configs);

    if(error.IsNotOk())
    {
      throw NetconfError{error};
    }
  }

  ::netconf::api::IPConfigs IPConfigHandler::CreateIPConfigs()
  {

    auto value = GetValueOfSet(vm_);
    if(!value.empty())
    {
      napi::IPConfigs ip_configs;
      auto error = napi::MakeIPConfigs(value, ip_configs);
      return error.IsOk() ? ip_configs : throw NetconfError{error};
    }

    auto ip_config = ::netconf::IPConfig();

    if(vm_.count("device") > 0)
    {
      ip_config.interface_ = vm_["device"].as<::std::string>();
    }

    if(vm_.count("ipaddr") > 0)
    {
      ip_config.address_ = vm_["ipaddr"].as<::std::string>();
    }

    if(vm_.count("netmask") > 0)
    {
      ip_config.netmask_ = vm_["netmask"].as<::std::string>();
    }

    if(vm_.count("prefix") > 0)
    {
      throw ::std::logic_error("Option prefix is not implemented yet");
    }

    auto ip_configs = napi::IPConfigs{ };
    ip_configs.AddIPConfig(ip_config);

    return ip_configs;
  }

  IPConfigHandler::IPConfigHandler(IPConfigHandler &&other) :
          execute_{std::move(other.execute_)},
          vm_{std::move(other.vm_)},
          parser_{other.parser_}
  {

  }

} /* namespace network_config */
