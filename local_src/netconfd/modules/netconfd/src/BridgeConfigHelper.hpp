// SPDX-License-Identifier: GPL-2.0-or-later
#pragma once

#include <initializer_list>
#include "Types.hpp"

namespace netconf {

bool AreInSameBridge(const BridgeConfig& bc, const Interfaces& interfaces);

}  // namespace netonfd
