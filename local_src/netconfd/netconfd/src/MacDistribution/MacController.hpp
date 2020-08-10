// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include "IMacController.hpp"
#include "MacAddress.hpp"

namespace netconf {

class MacController : public IMacController {
 public:
  MacController() = default;
  virtual ~MacController() = default;

  void SetMac(const MacAddress &mac, ::std::string const &interface) override;

};

}
