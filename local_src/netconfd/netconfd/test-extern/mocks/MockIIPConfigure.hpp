// SPDX-License-Identifier: GPL-2.0-or-later
#pragma once

#include <gmock/gmock.h>

#include "IIPConfigure.hpp"

namespace netconf {
  class IPConfig;
}  // namespace netconf

namespace netconf {

class MockIIPConfigure : public IIPConfigure {
 public:

  MOCK_CONST_METHOD1(Configure, Error(const netconf::IPConfig& config) );

};

}
