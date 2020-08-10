// SPDX-License-Identifier: GPL-2.0-or-later
#pragma once

#include <string>
#include <cstdint>
#include <gsl/gsl>
#include <type_traits>

namespace netconf {

struct MacAddress {
  static constexpr size_t LENGTH = 6;
  ::std::uint8_t addr_[LENGTH] = {0};

  MacAddress(){}
  constexpr MacAddress(const ::std::uint8_t(& array)[LENGTH]): addr_{}{
    for (auto i = 0; i != LENGTH; ++i)
      addr_[i] = array[i];
  }

  template <typename T,
            typename ::std::enable_if< std::is_integral<T>::value && (sizeof(T) == sizeof(uint8_t)), T>::type* = nullptr>
  explicit MacAddress(::gsl::span<const T, LENGTH> mac): MacAddress(mac.data()) { }

  explicit MacAddress(const void* mac){
    ::std::memcpy(addr_, mac, LENGTH);
  }

  ::std::string ToString(char delim = ':') const;

  static MacAddress FromString(const ::std::string& macstr, char delim = ':');

  /**
   * Increment the vendor specific part of the MAC address by the given value.
   * The OUI part is not altered.
   *
   * @param inc The increment value.
   * @return Copy of this MAC address incremented by @ref inc
   */
  MacAddress Increment(uint32_t inc) const;

  bool operator ==(const MacAddress& other) const{
    return std::memcmp(addr_, other.addr_, LENGTH) == 0;
  }

  constexpr const uint8_t* data() const{
    return &addr_[0];
  }
};

}  // namespace netconf
//---- End of header file ------------------------------------------------------

