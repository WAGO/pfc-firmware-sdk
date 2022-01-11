// SPDX-License-Identifier: GPL-2.0-or-later
#pragma once

#include <ostream>

#include "Status.hpp"

namespace netconf {

void PrintTo(const Status& bar, std::ostream* os);
void PrintTo(const StatusCode& bar, std::ostream* os);

}
