// SPDX-License-Identifier: GPL-2.0-or-later
#pragma once

#include <exception>
#include <string>
#include "Error.hpp"

namespace network_config {

class NetconfError: public ::std::exception
{
  public:
    NetconfError(netconf::Error&& e): error_{std::forward<netconf::Error>(e)}, description_{error_.ToString()}
    {}
    NetconfError(const netconf::Error& e): error_{e}, description_{error_.ToString()}
    {}

    const char* what() const noexcept override{
      return description_.c_str();
    }

    netconf::Error Get(){
      return error_;
    }

  private:
    netconf::Error error_;
    ::std::string description_;
};



}  // network_config
