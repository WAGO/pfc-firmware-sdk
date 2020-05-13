// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include <string>
#include <tuple>

namespace netconfd
{

  class FirmwareVersion
  {
    public:
      explicit FirmwareVersion(::std::string complete_version);
      ~FirmwareVersion() = default;

      int GetFirmwareIndex() const;
      int GetMajor() const;
      int GetMinor() const;
      int GetBugfix() const;

      bool IsValid(){
        return is_valid_;
      }

      bool operator<(const FirmwareVersion &other) const
      {
        if(major_ < other.major_)
          return true;
        if((major_ == other.major_) and (minor_ < other.minor_))
          return true;
        if((major_ == other.major_) and (minor_ == other.minor_) and (bugfix_ < other.bugfix_))
          return true;
        return false;
      }

      bool operator==(const FirmwareVersion &other) const
      {
        return ((major_ == other.major_) and (minor_ == other.minor_) and (bugfix_ == other.bugfix_)
                and (index_ == other.index_));
      }

      bool operator>(const FirmwareVersion &other) const
      {
        return (not (*this == other) && not (*this < other));
      }

      bool operator!=(const FirmwareVersion &other) const
      {
        return not (*this == other);
      }

    private:
      ::std::string complete_version_;
      int major_;
      int minor_;
      int bugfix_;
      int index_;

      bool is_valid_ = false;
  };

} /* namespace netconfd */

