/*
 * StatusPrettyPrint.cpp
 *
 *  Created on: 16.08.2019
 *      Author: u014487
 */

#include "StatusPrettyPrint.hpp"

namespace netconfd {

void PrintTo(const Status& bar, std::ostream* os);
void PrintTo(const StatusCode& bar, std::ostream* os);

void PrintTo(const Status& bar, std::ostream* os) {
  *os << static_cast<uint32_t>(bar.Get()) << ": " << bar.GetMessage();  // whatever needed to print bar to os
}

void PrintTo(const StatusCode& bar, std::ostream* os) {
  *os << static_cast<uint32_t>(bar);  // whatever needed to print bar to os
}

}  // namespace netconfd
