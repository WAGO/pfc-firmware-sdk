#pragma once

#include <gmock/gmock.h>
#include "IDipSwitch.hpp"

namespace netconfd {

class MockIDipSwitch : public IDipSwitch {
 public:

  MOCK_CONST_METHOD0(GetValue, std::uint8_t() );
  MOCK_CONST_METHOD0(HasReachedMaxValue, bool() );
  MOCK_CONST_METHOD0(GetMode, DipSwitchMode() );
};

}
  // namespace netconfd
