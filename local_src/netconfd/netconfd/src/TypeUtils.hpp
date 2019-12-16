// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include "Types.hpp"
#include <boost/algorithm/string.hpp>

namespace netconfd {

static inline ::std::string DuplexToString(Duplex d) {
  if (d == Duplex::FULL)
    return "full";
  if (d == Duplex::HALF)
    return "half";
  return ::std::string { };
}

static inline Duplex DuplexFromString(::std::string s) {
  boost::algorithm::to_lower(s);
  if (s == "half")
    return Duplex::HALF;
  if (s == "full")
    return Duplex::FULL;
  return Duplex::UNKNOWN;
}

static inline InterfaceState InterfaceStateFromString(::std::string s) {
  boost::algorithm::to_lower(s);
  if (s == "up")
    return InterfaceState::UP;
  if (s == "down")
    return InterfaceState::DOWN;
  return InterfaceState::UNKNOWN;
}

static inline ::std::string InterfaceStateToString(InterfaceState d) {
  if (d == InterfaceState::UP)
    return "up";
  if (d == InterfaceState::DOWN)
    return "down";
  return ::std::string { };
}

static inline bool AutonegotiationToBool(Autonegotiation a) {
  return (a == Autonegotiation::ON);
}


}  // namespace netconfd
