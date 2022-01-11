// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include "Types.hpp"

namespace netconf {

class IpAddressManipulator {
 public:

  static IPConfig ChangeLastAddressOctet(IPConfig const& current, uint32_t new_octet);
  static Address ChangeLastAddressOctet(Address const& current, uint32_t new_octet);
};

} /* namespace netconf */
