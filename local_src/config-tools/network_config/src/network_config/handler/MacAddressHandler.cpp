// SPDX-License-Identifier: GPL-2.0-or-later

#include "MacAddressHandler.hpp"

#include <iostream>
#include <string>
#include <exception>

#include "Utils.hpp"
#include "InterfaceConfig.hpp"
#include "Error.hpp"
#include "NetconfError.hpp"

namespace po = boost::program_options;

namespace network_config
{
  using namespace netconf;

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
    try{
    auto value = GetDevice(vm_);

    AddSystemInterfacePrefixWhenLabelStartsWithX(value);

    ::std::cout << netconf::api::GetMacAddress(value).ToString() << ::std::flush;
    }
    catch (std::exception& e) {
      throw NetconfError{Error{ErrorCode::SYSTEM_CALL, e.what()}};
    }
  }

  void MacAddressHandler::Execute()
  {
    this->GetMacAddress();
  }

} /* namespace network_config */
