// SPDX-License-Identifier: GPL-2.0-or-later
#pragma once

#include "Types.hpp"
#include "Error.hpp"
#include <ostream>
namespace netconf {

std::ostream& operator<<(std::ostream& os, const ::netconf::InterfaceConfig& pc) ;

std::ostream& operator<<(std::ostream& os, const ::netconf::IPConfig& pc) ;

std::ostream& operator<<(std::ostream& os, const ::netconf::Error& e);

std::ostream& operator<<(std::ostream &os, const ::netconf::ErrorCode &e);

}  // namespace netconf



