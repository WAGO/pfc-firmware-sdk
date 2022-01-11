// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include "Types.hpp"
#include "Status.hpp"

namespace netconf {

struct BridgeConfigValidator {
  static Status Validate(BridgeConfig const& config, const Interfaces& available_interfaces);
};

} /* namespace netconf */
