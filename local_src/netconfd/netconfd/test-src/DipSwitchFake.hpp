// SPDX-License-Identifier: GPL-2.0-or-later
#pragma once

#include "IDipSwitch.hpp"

namespace netconf
{

  class DipSwitchFake: public IDipSwitch
  {
    public:
      DipSwitchFake() = default;
      virtual ~DipSwitchFake() = default;

      netconf::DipSwitchMode GetMode() const override
      {
        return DipSwitchMode::HW_NOT_AVAILABLE;
      }

      bool HasReachedMaxValue() const override {
        return false;
      }

      uint8_t GetValue() const override {
        return 0;
      }
  };

} /* namespace netconf */
