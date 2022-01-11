// SPDX-License-Identifier: GPL-2.0-or-later
#pragma once

#include <gmock/gmock.h>

#include "IIPMonitor.hpp"

namespace netconf {

class MockIIPMonitor : public IIPMonitor {
 public:

  MOCK_METHOD1(RegisterEventHandler,void(IIPEvent& event_handler));
  MOCK_METHOD1(UnregisterEventHandler,void(IIPEvent& event_handler));

  MOCK_METHOD1(GetIPAddress, ::std::string(::std::uint32_t));
  MOCK_METHOD1(GetNetmask, ::std::string(::std::uint32_t));

  MOCK_METHOD1(SetIP, void(::std::string address) );

};


}  // namespace netconf
