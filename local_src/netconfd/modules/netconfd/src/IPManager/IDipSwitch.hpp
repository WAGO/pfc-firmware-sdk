// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include <cstdint>
#include "Types.hpp"

namespace netconf {

constexpr auto DIP_SWITCH_BRIDGE = "br0";

struct DipSwitchState{
  DipSwitchMode mode = DipSwitchMode::HW_NOT_AVAILABLE;
  std::uint8_t value = 0;
};

class IDipSwitch
{
 public:
  IDipSwitch() = default;
  virtual ~IDipSwitch() = default;

  IDipSwitch(const IDipSwitch&) = delete;
  IDipSwitch& operator=(const IDipSwitch&) = delete;
  IDipSwitch(IDipSwitch&&) = delete;
  IDipSwitch& operator=(IDipSwitch&&) = delete;


  virtual std::uint8_t GetValue() const = 0;
  virtual bool HasReachedMaxValue() const = 0;
  virtual DipSwitchMode GetMode() const = 0;
  /**
   * Get the state of the DIP switch.
   * The state represents the mode and the value.
   */
  virtual DipSwitchState GetState() const = 0;
};

}  // namespace netconf
