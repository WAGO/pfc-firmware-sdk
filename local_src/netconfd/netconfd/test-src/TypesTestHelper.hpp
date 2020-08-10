// SPDX-License-Identifier: GPL-2.0-or-later
#pragma once

#include "Types.hpp"
#include <ostream>

namespace netconf {

std::ostream& operator<<(std::ostream& os, const InterfaceConfig& pc) ;

std::ostream& operator<<(std::ostream& os, const IPConfig& pc) ;



}  // namespace netconf


