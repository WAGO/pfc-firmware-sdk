// SPDX-License-Identifier: LGPL-3.0-or-later

#include <boost/asio/ip/address_v4.hpp>
#include <boost/asio/ip/network_v4.hpp>

#include <arpa/inet.h>
#include "NetworkHelper.hpp"

#include "Types.hpp"


namespace netconf {

using boost::asio::ip::address_v4;
using boost::asio::ip::network_v4;

::std::string GetPrefix(const ::std::string& netmask) {
  std::uint32_t mask = 0;
  auto status = inet_pton(AF_INET, netmask.c_str(), &mask);
  auto cidr = 0;

  if (status == 1 && mask > 0) {
    cidr = __builtin_popcount(mask);
  }

  return ::std::to_string(cidr);
}

::std::string GetNetmask(const uint32_t& prefix) {

  address_v4 address;
  auto network = network_v4(address, static_cast<uint16_t>(prefix));

  return network.netmask().to_string();

}

::std::string GetBroadcast(const ::std::string& address, const ::std::string& netmask){

  ::std::string broadcast = ZeroIP;
  if (address.empty() || netmask.empty()) {
    return ::std::string{};
  }
  if(address == ZeroIP || netmask == ZeroIP){
    return broadcast;
  }

  try{
    address_v4 addr = address_v4::from_string(address);
    address_v4 mask = address_v4::from_string(netmask);
    address_v4 bc = address_v4::broadcast(addr,mask);
    broadcast = bc.to_string();
  } catch(...){}

  return broadcast;
}


}
