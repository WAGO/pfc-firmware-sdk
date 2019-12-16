// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include "IDipSwitch.hpp"
#include <limits>
#include <string>

namespace netconfd {

class DipSwitch : public IDipSwitch {
 public:
  explicit DipSwitch(const std::string& dip_switch_device_value_file);
  ~DipSwitch() = default;

  std::uint8_t GetValue() const override { return value_;}
  bool HasReachedMaxValue() const override { return value_ == std::numeric_limits<uint8_t>::max();}
 private:

  std::uint8_t value_;
};

} /* namespace netconfd */
