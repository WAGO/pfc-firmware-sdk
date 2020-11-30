// SPDX-License-Identifier: LGPL-3.0-or-later
#include "MacAddress.hpp"
#include <boost/algorithm/string/join.hpp>
#include <boost/algorithm/string.hpp>
#include <vector>
#include <sstream>
#include <exception>
#include <gsl/gsl>

namespace netconf {


::std::string MacAddress::ToString(char delim) const {
  ::std::vector<::std::string> hex_parts;

  std::transform(&addr_[0], &addr_[6], ::std::back_inserter(hex_parts), [](auto byte) {
    static constexpr char lookup[16] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F' };
    char hex_cstr[3] = "00";
    hex_cstr[0] = lookup[((byte >> 4) & 0x0F)];
    hex_cstr[1] = lookup[((byte) & 0x0F)];
    return ::std::string { static_cast<const char*>(hex_cstr) };
  });
  return ::boost::algorithm::join(hex_parts, ::std::string { delim });

}

MacAddress MacAddress::FromString(const ::std::string &macstr, char delim) {
  ::std::vector<::std::string> mac_bytes;
  boost::split(mac_bytes, macstr, boost::is_any_of(std::string { delim }));

  if (mac_bytes.size() != LENGTH) {
    throw std::invalid_argument("Convert (" + macstr + ") to binary mac failed.");
  }

  MacAddress mac { };
  for (auto i = 0u; i < LENGTH; i++) {
    std::stringstream ss(mac_bytes.at(i));
    uint32_t mac_byte;
    ss >> std::hex >> mac_byte;
    mac.addr_[i] = gsl::narrow_cast<uint8_t>(mac_byte);
  }

  return mac;
}

using ::gsl::span;

constexpr uint32_t PartToUint(span<const uint8_t, 3> part){
  return static_cast<uint32_t>(part[0] << 16 | part[1] << 8 | part[2]);
}
constexpr void UintToPart(span<uint8_t, 3> part, uint32_t number){
  part[0] = gsl::narrow_cast<uint8_t>(number >> 16);
  part[1] = gsl::narrow_cast<uint8_t>(number >> 8);
  part[2] = gsl::narrow_cast<uint8_t>(number);
}


MacAddress MacAddress::Increment(uint32_t inc) const{

  auto mac = *this;
  auto indipart = span<uint8_t,3>(&(mac.addr_[3]), 3);
  uint32_t individual_part_number = PartToUint(indipart);
  UintToPart(indipart, individual_part_number+inc);
  return mac;

}



}  // namespace netconf

