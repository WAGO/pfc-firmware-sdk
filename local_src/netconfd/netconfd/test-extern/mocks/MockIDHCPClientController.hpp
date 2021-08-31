// SPDX-License-Identifier: GPL-2.0-or-later
#pragma once

#include <gmock/gmock.h>
#include "IDHCPClientController.hpp"

namespace netconf {

class MockIDHCPClientController : public IDHCPClientController {
 public:

  MOCK_CONST_METHOD1(StartClient, Status(const Bridge& bridge) );
  MOCK_CONST_METHOD1(StopClient, void(const Bridge& bridge) );
  MOCK_CONST_METHOD1(GetStatus, DHCPClientStatus(const Bridge& bridge) );

};

}
  // namespace netconf
