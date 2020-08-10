// SPDX-License-Identifier: GPL-2.0-or-later
#pragma once

#include <gmock/gmock.h>

#include "IInterfaceMonitor.hpp"

namespace netconf {

class MockIInterfaceMonitor : public IInterfaceMonitor {
 public:

  MOCK_METHOD1(RegisterEventHandler,void(IInterfaceEvent& event_handler));
  MOCK_METHOD1(UnregisterEventHandler,void(IInterfaceEvent& event_handler));

  MOCK_METHOD1(GetIffFlags, ::std::uint32_t(::std::uint32_t if_index) );
  MOCK_METHOD1(GetAddressFamily, ::std::int32_t(::std::uint32_t if_index) );

};


}  // namespace netconf
