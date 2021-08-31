// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * StatusPrettyPrint.cpp
 *
 *  Created on: 16.08.2019
 *      Author: u014487
 */

#include "StatusPrettyPrint.hpp"

namespace netconf {

void PrintTo(const Status& bar, std::ostream* os);
void PrintTo(const StatusCode& bar, std::ostream* os);

void PrintTo(const Status& bar, std::ostream* os) {
  *os << static_cast<uint32_t>(bar.GetStatusCode()) << ": " << bar.ToString();  // whatever needed to print bar to os
}

void PrintTo(const StatusCode& bar, std::ostream* os) {
  *os << static_cast<uint32_t>(bar);  // whatever needed to print bar to os
}

}  // namespace netconf
