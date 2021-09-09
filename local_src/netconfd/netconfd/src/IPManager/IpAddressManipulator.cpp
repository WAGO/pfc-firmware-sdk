// SPDX-License-Identifier: GPL-2.0-or-later

#include <boost/asio.hpp>
#include <IpAddressManipulator.hpp>

namespace netconf {

using ::boost::asio::ip::make_address_v4;
using ::boost::system::error_code;

IPConfig IpAddressManipulator::ChangeLastAddressOctet(IPConfig const& current, uint32_t new_octet){
  IPConfig new_cfg{current};
  error_code error;

  auto current_address = make_address_v4(current.address_,error).to_uint();
  if(!error && current_address != 0){
    uint32_t new_address = (current_address & 0xFFFFFF00) + (new_octet & 0x000000FF);

    new_cfg.address_ = make_address_v4(new_address).to_string();
  }

  return new_cfg;
}

Address IpAddressManipulator::ChangeLastAddressOctet(Address const& current, uint32_t new_octet){
  error_code error;
  Address address;

  auto current_address = make_address_v4(current,error).to_uint();
  if(!error && current_address != 0){
    uint32_t new_address = (current_address & 0xFFFFFF00) + (new_octet & 0x000000FF);

    address = make_address_v4(new_address).to_string();
  }

  return address;
}

} /* namespace netconf */
