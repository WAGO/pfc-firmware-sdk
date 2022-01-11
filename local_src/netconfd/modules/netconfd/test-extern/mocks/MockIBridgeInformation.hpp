// SPDX-License-Identifier: GPL-2.0-or-later
#pragma once

#include <gmock/gmock.h>

#include "IBridgeInformation.hpp"

namespace netconf {

class MockIInterfaceManagerInformation : public IBridgeInformation {
 public:

  MOCK_CONST_METHOD0(GetBridgeAssignedInterfaces, Interfaces() );
  MOCK_CONST_METHOD0(GetBridgeConfig, BridgeConfig () );
};

}
