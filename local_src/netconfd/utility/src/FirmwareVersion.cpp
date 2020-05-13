// SPDX-License-Identifier: GPL-2.0-or-later

#include "FirmwareVersion.hpp"
#include <utility>
#include <vector>
#include <boost/algorithm/string.hpp>

namespace netconfd
{

  static ::std::vector<::std::string> Split(::std::string text,
                                            const ::std::string& delimiters)
  {
    std::vector<std::string> v;
    boost::algorithm::split(v, text, boost::is_any_of(delimiters));
    return v;
  }

  FirmwareVersion::FirmwareVersion(::std::string complete_version) :
          complete_version_{std::move(complete_version)},
          major_{ },
          minor_{ },
          bugfix_{ },
          index_{ }
  {

    try
    {

      auto parts = Split(complete_version_, ".()");

      if(parts.size() >= 4)
      {
        major_ = ::std::stoi(parts[0]);
        minor_ = ::std::stoi(parts[1]);
        bugfix_ = ::std::stoi(parts[2]);
        index_ = ::std::stoi(parts[3]);
        is_valid_ = true;
      }

    }
    catch (...)
    {
      /* nothing to do here */
    }
  }

  int FirmwareVersion::GetFirmwareIndex() const
  {
    return index_;
  }

} /* namespace netconfd */
