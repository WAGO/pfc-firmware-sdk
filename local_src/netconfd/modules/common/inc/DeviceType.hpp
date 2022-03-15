// SPDX-License-Identifier: LGPL-3.0-or-later
#pragma once

#include <string>
#include <ostream>

namespace netconf {

/**
 * Device types used in the neconf environment to classify different types of interfaces.
 */
enum class DeviceType {
  Ethernet = 0x01,  //!< Ethernet basic master interfaces e.g. eth0
  Bridge = 0x02,    //!< Bridge bridge interfaces e.g. br0,br1,...
  Virtual = 0x04,   //!< Virtual vpn tunnel interface e.g. tun
  Wwan = 0x08,      //!< Wwan modem device e.g. wwan0
  Loopback = 0x10,  //!< Loopback loopback device e.g. lo
  Port = 0x20,      //!< Port ethernet port interfaces e.g. ethX1, ethX2, ethX11, ethX12
  Service = 0x40,   //!< Service WAGO USB Service interface e.g. usb0
  Other = 0x80,     //!< Other other interface types
};



constexpr DeviceType operator |(const DeviceType lhs, const DeviceType rhs) {
  using T = std::underlying_type_t <DeviceType>;
  return static_cast<DeviceType>(static_cast<T>(lhs) | static_cast<T>(rhs));
}

constexpr DeviceType& operator |=(DeviceType &lhs, const DeviceType rhs) {
  lhs = lhs | rhs;
  return lhs;
}

constexpr DeviceType& operator -=(DeviceType &lhs, DeviceType rhs) {
  using T = std::underlying_type_t <DeviceType>;
  lhs = static_cast<DeviceType>(static_cast<T>(lhs) & ~static_cast<T>(rhs));
  return lhs;
}

constexpr bool operator &&(const DeviceType lhs, const DeviceType rhs) {
  using T = std::underlying_type_t <DeviceType>;
  return (static_cast<T>(lhs) & static_cast<T>(rhs)) != 0;
}

::std::string ToString(const DeviceType kind);
DeviceType DeviceTypeFromString(const ::std::string& str);

inline std::ostream& operator <<(::std::ostream &os, const DeviceType type) {
  os << ToString(type);
  return os;
}

/**
 * The device types currently supported by the netconf API.
 */
constexpr DeviceType SupportedInterfaces = DeviceType::Bridge | DeviceType::Virtual | DeviceType::Wwan
    | DeviceType::Port | DeviceType::Service;

}  // namespace netconf

//---- End of header file ------------------------------------------------------

