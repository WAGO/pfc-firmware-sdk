/*
 * StatusPrettyPrint.hpp
 *
 *  Created on: 16.08.2019
 *      Author: u014487
 */

#ifndef NETCONFD_TEST_SRC_STATUSPRETTYPRINT_HPP_
#define NETCONFD_TEST_SRC_STATUSPRETTYPRINT_HPP_

#include "Status.hpp"
#include <ostream>

namespace netconfd {

void PrintTo(const Status& bar, std::ostream* os);
void PrintTo(const StatusCode& bar, std::ostream* os);

}

#endif /* NETCONFD_TEST_SRC_STATUSPRETTYPRINT_HPP_ */
