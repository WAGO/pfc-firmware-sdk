// SPDX-License-Identifier: GPL-2.0-or-later
#pragma once

#include "Types.hpp"
#include <ostream>

namespace netconfd {

bool operator==(const netconfd::InterfaceConfig& a, const netconfd::InterfaceConfig& b);
std::ostream& operator<<(std::ostream& os, const InterfaceConfig& pc) ;

std::ostream& operator<<(std::ostream& os, const IPConfig& pc) ;



}  // namespace netconfd


