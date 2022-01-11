// SPDX-License-Identifier: LGPL-3.0-or-later
#include "DeviceType.hpp"
#include <sstream>
#include <tuple>
#include <array>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>

namespace netconf {

using KindConversion = ::std::tuple<const char*, DeviceType>;

constexpr ::std::array<KindConversion,7> conversions {{
    KindConversion{"bridge", DeviceType::Bridge},
    KindConversion{"wwan", DeviceType::Wwan},
    KindConversion{"virtual", DeviceType::Virtual},
    KindConversion{"port", DeviceType::Port},
    KindConversion{"loopback", DeviceType::Loopback},
    KindConversion{"ethernet", DeviceType::Ethernet},
    KindConversion{"other", DeviceType::Other}
}};



DeviceType DeviceTypeFromString(const ::std::string& str)
{
  DeviceType t = static_cast<DeviceType>(0); // Make empty at first;
  ::std::vector<::std::string> parts;
  boost::algorithm::split(parts, str, boost::is_any_of("|"), boost::algorithm::token_compress_on );

  for(const auto& p : parts){
    auto it = std::find_if(std::begin(conversions),std::end(conversions), [&](const auto& conv){return std::get<0>(conv) == p;});
    if(it != std::end(conversions)){
      t |= std::get<1>(*it);
    }
  }

  return t;
}

::std::string ToString(DeviceType kind) {

  ::std::stringstream ss;

  for(const auto& conv: conversions){
    const auto& type = ::std::get<1>(conv);
    const auto& text = ::std::get<0>(conv);
    if(kind && type){
      kind -= type;
      if (ss.rdbuf()->in_avail() > 0) {
        ss << "|";
      }
      ss << text;
    }
  }
  return ss.str();
}

}  // namespace netconf
