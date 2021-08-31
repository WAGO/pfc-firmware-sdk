// SPDX-License-Identifier: GPL-2.0-or-later
#pragma once

#include <exception>
#include <string>
#include <Status.hpp>

namespace network_config {

class NetconfStatus : public ::std::exception {
 public:
  NetconfStatus(netconf::Status &&e)
      : error_ { std::forward < netconf::Status > (e) },
        description_ { error_.ToString() } {
  }
  NetconfStatus(const netconf::Status &e)
      : error_ { e },
        description_ { error_.ToString() } {
  }

  const char* what() const noexcept override {
    return description_.c_str();
  }

  netconf::Status Get() {
    return error_;
  }

 private:
  netconf::Status error_;
  ::std::string description_;
};

}  // network_config
