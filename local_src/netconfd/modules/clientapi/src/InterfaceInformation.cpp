// SPDX-License-Identifier: LGPL-3.0-or-later

#include <algorithm>
#include <boost/algorithm/string/join.hpp>
#include <sstream>
#include <vector>

#include "InterfaceInformationApi.hpp"
#include "JsonConverter.hpp"
#include "NetconfdDbusClient.hpp"

namespace netconf {
namespace api {

::std::string ToString(const InterfaceInformation &info) noexcept {
  ::std::stringstream ss;
  ss << "device=" << info.GetInterfaceName();
  ss << " label=" << info.GetInterfaceLabel();
  ss << " type=" << ToString(info.GetType());
  ss << " ipreadonly=" << ::std::boolalpha << info.IsIpConfigurationReadonly();

  auto link_modes = info.GetSupportedlinkModes();
  if (!link_modes.empty()) {
    ss << " supportedlinkmodes=";
    for (auto &mode : link_modes) {
      ::std::string duplex = mode.duplex_ == Duplex::FULL ? ::std::string("full") : ::std::string("half");
      ss << mode.speed_ << "/" << duplex << ",";
    }
    ss.seekp(-1, ::std::ios_base::end);
  }

  auto autoneg_supported = info.GetAutonegSupported();
  if (autoneg_supported != AutonegotiationSupported::UNKNOWN) {
    auto autoneg = (autoneg_supported == AutonegotiationSupported::NO) ? "false" : "true";
    ss << " autonegsupported=" << autoneg;
  }

  return ss.str();
}

::std::string ToString(const InterfaceInformations &info) noexcept {
  ::std::vector<::std::string> text_infos;
  ::std::transform(info.cbegin(), info.cend(), ::std::back_inserter(text_infos),
                   [](const auto &i) { return ToString(i); });
  return ::boost::algorithm::join(text_infos, ",");
}

::std::string ToJson(const InterfaceInformation &info) noexcept {
  JsonConverter jc;
  return jc.ToJsonString(info);
}

::std::string ToJson(const InterfaceInformations &info) noexcept {
  JsonConverter jc;
  return jc.ToJsonString(info);
}

::std::string ToPrettyJson(const InterfaceInformation &info) noexcept {
  JsonConverter jc;
  return jc.ToJsonString(info, JsonFormat::PRETTY);
}

::std::string ToPrettyJson(const InterfaceInformations &info) noexcept {
  JsonConverter jc;
  return jc.ToJsonString(info, JsonFormat::PRETTY);
}

}  // namespace api
}  // namespace netconf
