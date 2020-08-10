// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include <string>
#include "MacAddress.hpp"

namespace netconf {

class IMacController {
 public:
  IMacController() = default;
  virtual ~IMacController() = default;

  virtual void SetMac(MacAddress const& mac, ::std::string const& interface) = 0;

};

}
