// SPDX-License-Identifier: LGPL-3.0-or-later
#pragma once

#include <string>
#include <ostream>
#include <vector>

#include "BaseTypes.hpp"

namespace netconf {

struct LinkMode {

  LinkMode(){};
  LinkMode(int speed, Duplex duplex): speed_{speed}, duplex_{duplex}{};
  virtual ~LinkMode() = default;
  LinkMode(const LinkMode&) = default;

  LinkMode& operator=(const LinkMode&) = delete;
  LinkMode(LinkMode&&) = delete;
  LinkMode& operator=(LinkMode&&) = delete;

  int speed_ = 0;
  Duplex duplex_ = Duplex::UNKNOWN;

  friend bool operator==(const LinkMode &rhs, const LinkMode &lhs) {
    return ((rhs.speed_ == lhs.speed_) && (rhs.duplex_ == lhs.duplex_));
  }

  explicit operator bool() const {
    return speed_ != 0 && duplex_ != Duplex::UNKNOWN;
  }

};

using LinkModes = ::std::vector<LinkMode>;

}  // namespace netconf

//---- End of header file ------------------------------------------------------

