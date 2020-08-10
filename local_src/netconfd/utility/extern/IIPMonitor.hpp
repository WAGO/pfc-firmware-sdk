// SPDX-License-Identifier: GPL-2.0-or-later
#pragma once

#include <cstdint>
#include <string>
#include "IIPEvent.hpp"

namespace netconf {

class IIPMonitor {
 public:
  virtual ~IIPMonitor() = default;

  virtual ::std::string GetIPAddress(::std::uint32_t if_index)= 0;
  virtual ::std::string GetNetmask(::std::uint32_t if_index) = 0;

  virtual void RegisterEventHandler(IIPEvent& event_handler) = 0;
  virtual void UnregisterEventHandler(IIPEvent& event_handler) = 0;
};

}
