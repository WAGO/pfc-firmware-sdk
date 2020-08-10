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
  }


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

    ::std::cout << netconf::api::GetMacAddress(value).ToString() << ::std::flush;
  }

  void MacAddressHandler::Execute()
  {
    this->GetMacAddress();
  }

} /* namespace network_config */
