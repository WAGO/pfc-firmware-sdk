// SPDX-License-Identifier: GPL-2.0-or-later

#include "MacAddressHandler.hpp"

#include <iostream>
#include <string>
#include <exception>

#include "Utils.hpp"
#include "InterfaceConfig.hpp"

namespace po = boost::program_options;

namespace network_config
{

  MacAddressHandler::MacAddressHandler(const po::variables_map &vm) :
          vm_{vm}
  {
    (void)vm_;
    execute_ = [this]()
    { this->GetMacAddress();};
  }
  ;

  static void AddSystemInterfacePrefixWhenLabelStartsWithX(::std::string &name)
  {
    const ::std::string first_interface_letter = "X";

    auto pos = name.find(first_interface_letter);
    if(pos == 0)
    {
      name.insert(0, "eth");
    }

  }

  void MacAddressHandler::GetMacAddress()
  {
    auto value = GetDevice(vm_);
    if(value.empty())
    {
      throw ::std::runtime_error("Failed to get mac address. Missing interface name. (e.g. --device=X1)");
    }

    AddSystemInterfacePrefixWhenLabelStartsWithX(value);

    auto mac_address = netconf::GetMacAddress(value);
    for(size_t i = 0; i < netconf::mac_addr_length; i++)
    {
      printf("%02X", mac_address.addr_[i]);
      if(i < 5)
      {
        printf(":");
      }
    }
  }

  void MacAddressHandler::Execute()
  {
    execute_();
  }

} /* namespace network_config */
