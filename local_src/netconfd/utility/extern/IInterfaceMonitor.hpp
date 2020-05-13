// SPDX-License-Identifier: GPL-2.0-or-later
#pragma once

#include <cstdint>
#include "IInterfaceEvent.hpp"

namespace netconfd {

class IInterfaceMonitor {
 public:
  virtual ~IInterfaceMonitor() = default;

  virtual void RegisterEventHandler(IInterfaceEvent& event_handler) = 0;
  virtual void UnregisterEventHandler(IInterfaceEvent& event_handler) = 0;

  virtual ::std::uint32_t GetIffFlags(::std::uint32_t if_index) = 0;
  virtual ::std::int32_t GetAddressFamily(::std::uint32_t if_index) = 0;

};


}
