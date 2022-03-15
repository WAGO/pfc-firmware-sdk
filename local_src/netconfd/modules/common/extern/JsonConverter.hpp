// SPDX-License-Identifier: LGPL-3.0-or-later
#pragma once

#include "Types.hpp"
#include "Status.hpp"
#include "DynamicIPEventAction.hpp"

namespace netconf {

enum class JsonFormat {
  COMPACT,
  PRETTY
};

class JsonConverter {
 public:

  ::std::string ToJsonString(const netconf::DipSwitchConfig &obj, JsonFormat format = JsonFormat::COMPACT) const;
  Status FromJsonString(const ::std::string &str, netconf::DipSwitchConfig &out_obj) const;

  ::std::string ToJsonString(const netconf::DipSwitchIpConfig &obj, JsonFormat format = JsonFormat::COMPACT) const;
  Status FromJsonString(const ::std::string &str, netconf::DipSwitchIpConfig &out_obj) const;

  ::std::string ToJsonString(const BridgeConfig &obj, JsonFormat format = JsonFormat::COMPACT) const;
  Status FromJsonString(const ::std::string &str, BridgeConfig &out_obj) const;

  ::std::string ToJsonString(const IPConfigs &obj, JsonFormat format = JsonFormat::COMPACT) const;
  Status FromJsonString(const ::std::string &str, IPConfigs &out_obj) const;

  ::std::string ToJsonString(const InterfaceConfigs &obj, JsonFormat format = JsonFormat::COMPACT) const;
  Status FromJsonString(const ::std::string &str, InterfaceConfigs &out_obj) const;

  ::std::string ToJsonString(const InterfaceStatuses &obj, JsonFormat format = JsonFormat::COMPACT) const;
  Status FromJsonString(const ::std::string &str, InterfaceStatuses &out_obj) const;

  ::std::string ToJsonString(const IPConfig &obj, JsonFormat format = JsonFormat::COMPACT) const;
  Status FromJsonString(const ::std::string &str, IPConfig &out_obj) const;

  ::std::string ToJsonString(const InterfaceConfig &obj, JsonFormat format = JsonFormat::COMPACT) const;
  Status FromJsonString(const ::std::string &str, InterfaceConfig &out_obj) const;

  ::std::string ToJsonString(const InterfaceInformation &obj, JsonFormat format = JsonFormat::COMPACT) const;
  Status FromJsonString(const ::std::string &str, InterfaceInformation &out_obj) const;

  ::std::string ToJsonString(const InterfaceInformations &obj, JsonFormat format = JsonFormat::COMPACT) const;
  Status FromJsonString(const ::std::string &str, InterfaceInformations &out_obj) const;

  ::std::string ToJsonString(const Status &obj, JsonFormat format = JsonFormat::COMPACT) const;
  Status FromJsonString(const ::std::string &str, Status &out_obj) const;

  ::std::string ToJsonString(const ::std::string &itf_name, DynamicIPEventAction action, JsonFormat format =
                                 JsonFormat::COMPACT) const;
  Status FromJsonString(const ::std::string &str, ::std::string &itf_name, DynamicIPEventAction &action) const;
};

}  // namespace netconf
