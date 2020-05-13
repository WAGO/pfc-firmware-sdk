#pragma once

#include <gmock/gmock.h>

#include "IIPConfigure.hpp"

namespace netconfd {

  class IPConfig;


class MockIIPConfigure : public IIPConfigure {
 public:

  MOCK_CONST_METHOD1(Configure, Status(const IPConfig& config) );

};

}
