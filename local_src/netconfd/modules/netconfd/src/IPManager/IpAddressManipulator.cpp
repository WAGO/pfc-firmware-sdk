// SPDX-License-Identifier: GPL-2.0-or-later

#include "IpAddressManipulator.hpp"

#include <boost/asio.hpp>

namespace netconf {

using ::boost::asio::ip::make_address_v4;
using ::boost::system::error_code;

IPConfig IpAddressManipulator::ChangeLastAddressOctet(IPConfig const& current, uint32_t new_octet) {
  IPConfig new_cfg{current};
  new_cfg.address_ = ChangeLastAddressOctet(current.address_, new_octet);
  return new_cfg;
}

Address IpAddressManipulator::ChangeLastAddressOctet(Address const& current, uint32_t new_octet) {
  error_code error;

  auto current_address = make_address_v4(current, error).to_uint();
  if (!error && current_address != 0) {
    uint32_t new_address = (current_address & 0xFFFFFF00) + (new_octet & 0x000000FF);

    return make_address_v4(new_address).to_string();
  }

  return current;
}

} /* namespace netconf */
