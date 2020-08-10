// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include "Types.hpp"
#include "Status.hpp"

namespace netconf {

class IBridgeConfigValidator {
 public:
  IBridgeConfigValidator() = default;
  virtual ~IBridgeConfigValidator() = default;

  IBridgeConfigValidator(const IBridgeConfigValidator&) = delete;
  IBridgeConfigValidator& operator=(const IBridgeConfigValidator&) = delete;
  IBridgeConfigValidator(const IBridgeConfigValidator&&) = delete;
  IBridgeConfigValidator& operator=(const IBridgeConfigValidator&&) = delete;

  virtual Status Validate(BridgeConfig const& config) const = 0;

};

} /* namespace netconf */

