// SPDX-License-Identifier: LGPL-3.0-or-later

#pragma once

#include <string>
#include <tuple>

namespace netconf {

class FirmwareVersion {
 public:
  FirmwareVersion() = default;
  constexpr FirmwareVersion(int major, int minor, int bugfix, int index)
  :
          major_ { major },
          minor_ { minor },
          bugfix_ {bugfix },
          index_ { index },
          is_valid_ { true }
  {}

  explicit FirmwareVersion(::std::string complete_version);
  ~FirmwareVersion() = default;
  FirmwareVersion(FirmwareVersion&& other) noexcept;
  FirmwareVersion(const FirmwareVersion& other) = default;
  FirmwareVersion& operator=(FirmwareVersion&& other) noexcept;
  FirmwareVersion& operator=(const FirmwareVersion& other) = default;

  constexpr int GetFirmwareIndex() const {
    return index_;
  }
  constexpr int GetMajor() const {
    return major_;
  }
  constexpr int GetMinor() const {
    return minor_;
  }
  constexpr int GetBugfix() const {
    return bugfix_;
  }

  constexpr bool IsValid() const {
    return is_valid_;
  }

  constexpr bool operator<(const FirmwareVersion &other) const {
    if (major_ < other.major_)
      return true;
    if ((major_ == other.major_) and (minor_ < other.minor_))
      return true;
    if ((major_ == other.major_) and (minor_ == other.minor_) and (bugfix_ < other.bugfix_))
      return true;
    return false;
  }

  constexpr bool operator==(const FirmwareVersion &other) const {
    return ((major_ == other.major_) and (minor_ == other.minor_) and (bugfix_ == other.bugfix_)
        and (index_ == other.index_));
  }

  bool operator>(const FirmwareVersion &other) const {
    return (not (*this == other) && not (*this < other));
  }

  bool operator!=(const FirmwareVersion &other) const {
    return not (*this == other);
  }

 private:
  int major_ = 0;
  int minor_ = 0;
  int bugfix_ = 0;
  int index_ = 0;
  bool is_valid_ = false;
};

} /* namespace netconf */

