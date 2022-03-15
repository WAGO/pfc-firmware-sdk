// SPDX-License-Identifier: LGPL-3.0-or-later

#include "FirmwareVersion.hpp"
#include <utility>
#include <vector>
#include <boost/algorithm/string.hpp>

namespace netconf {

static ::std::vector<::std::string> Split(::std::string text, const ::std::string &delimiters) {
  std::vector<std::string> v;
  boost::algorithm::split(v, text, boost::is_any_of(delimiters));
  return v;
}

FirmwareVersion::FirmwareVersion(::std::string complete_version)
     {

  try {

    auto parts = Split(::std::move(complete_version), ".()");

    if (parts.size() >= 4) {
      major_ = ::std::stoi(parts[0]);
      minor_ = ::std::stoi(parts[1]);
      bugfix_ = ::std::stoi(parts[2]);
      index_ = ::std::stoi(parts[3]);
      is_valid_ = true;
    }

  } catch (...) {
    /* nothing to do here */
  }
}

FirmwareVersion::FirmwareVersion(FirmwareVersion &&other) noexcept
    :
    major_ { other.major_ },
    minor_ { other.minor_ },
    bugfix_ { other.bugfix_ },
    index_ { other.index_ },
    is_valid_ { other.is_valid_ } {

  other.major_ = 0;
  other.minor_ = 0;
  other.bugfix_ = 0;
  other.index_ = 0;
  other.is_valid_ = false;
}

FirmwareVersion& FirmwareVersion::operator=(FirmwareVersion &&other) noexcept {
  major_ = other.major_;
  minor_ = other.minor_;
  bugfix_ = other.bugfix_;
  index_ = other.index_;
  is_valid_ = other.is_valid_;

  other.major_ = 0;
  other.minor_ = 0;
  other.bugfix_ = 0;
  other.index_ = 0;
  other.is_valid_ = false;
  return *this;
}

} /* namespace netconf */
