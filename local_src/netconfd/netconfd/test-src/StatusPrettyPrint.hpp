// SPDX-License-Identifier: GPL-2.0-or-later
#pragma once

#include <ostream>
#include "Error.hpp"

namespace netconf {

void PrintTo(const Error& bar, std::ostream* os);
void PrintTo(const ErrorCode& bar, std::ostream* os);

}
