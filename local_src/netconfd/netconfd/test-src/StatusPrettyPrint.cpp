// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * StatusPrettyPrint.cpp
 *
 *  Created on: 16.08.2019
 *      Author: u014487
 */

#include "StatusPrettyPrint.hpp"

namespace netconf {

void PrintTo(const Error& bar, std::ostream* os);
void PrintTo(const ErrorCode& bar, std::ostream* os);

void PrintTo(const Error& bar, std::ostream* os) {
  *os << static_cast<uint32_t>(bar.GetErrorCode()) << ": " << bar.ToString();  // whatever needed to print bar to os
}

void PrintTo(const ErrorCode& bar, std::ostream* os) {
  *os << static_cast<uint32_t>(bar);  // whatever needed to print bar to os
}

}  // namespace netconf
