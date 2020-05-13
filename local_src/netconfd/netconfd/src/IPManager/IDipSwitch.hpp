// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include <cstdint>
#include <nlohmann/json.hpp>
#include "Types.hpp"

namespace netconfd {

NLOHMANN_JSON_SERIALIZE_ENUM( DipSwitchMode, {
    {OFF, "off"},
    {STATIC, "static"},
    {DHCP, "dhcp"},
    {HW_NOT_AVAILABLE, "hw-not-available"}
});

class IDipSwitch
{
 public:
  IDipSwitch() = default;
  virtual ~IDipSwitch() = default;

  virtual std::uint8_t GetValue() const = 0;
  virtual bool HasReachedMaxValue() const = 0;
  virtual DipSwitchMode GetMode() const = 0;
};

}  // namespace netconfd
