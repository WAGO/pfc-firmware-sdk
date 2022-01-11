// SPDX-License-Identifier: GPL-2.0-or-later
#pragma once

#include <gmock/gmock.h>

#include "IBootpClientController.hpp"

namespace netconf {

class MockIBootpController : public IBootpClientController {
 public:

  MOCK_CONST_METHOD1(StartClient, Status(const Bridge& bridge) );
  MOCK_CONST_METHOD1(StopClient, void(const Bridge& bridge) );
  MOCK_CONST_METHOD1(GetStatus, BootpClientStatus(const Bridge& bridge) );

};

} // namespace netconf
