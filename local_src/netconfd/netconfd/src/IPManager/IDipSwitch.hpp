// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include <cstdint>
#include "Types.hpp"

namespace netconf {

class IDipSwitch
{
 public:
  IDipSwitch() = default;
  virtual ~IDipSwitch() = default;

  virtual std::uint8_t GetValue() const = 0;
  virtual bool HasReachedMaxValue() const = 0;
  virtual DipSwitchMode GetMode() const = 0;
};

}  // namespace netconf
