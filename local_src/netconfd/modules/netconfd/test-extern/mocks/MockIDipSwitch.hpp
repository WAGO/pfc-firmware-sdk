// SPDX-License-Identifier: GPL-2.0-or-later
#pragma once

#include <gmock/gmock.h>
#include "IDipSwitch.hpp"

namespace netconf {

class MockIDipSwitch : public IDipSwitch {
 public:

  MOCK_CONST_METHOD0(GetValue, std::uint8_t() );
  MOCK_CONST_METHOD0(HasReachedMaxValue, bool() );
  MOCK_CONST_METHOD0(GetMode, DipSwitchMode() );
  MOCK_CONST_METHOD0(GetState, DipSwitchState());
};

}
  // namespace netconf
