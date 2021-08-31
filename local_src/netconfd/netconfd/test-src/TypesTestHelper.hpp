// SPDX-License-Identifier: GPL-2.0-or-later
#pragma once

#include "Types.hpp"
#include <ostream>

#include "Status.hpp"
namespace netconf {

std::ostream& operator<<(std::ostream& os, const ::netconf::InterfaceConfig& pc) ;

std::ostream& operator<<(std::ostream& os, const ::netconf::IPConfig& pc) ;

std::ostream& operator<<(std::ostream& os, const ::netconf::Status& e);

std::ostream& operator<<(std::ostream &os, const ::netconf::StatusCode &e);

}  // namespace netconf



