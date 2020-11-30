// SPDX-License-Identifier: LGPL-3.0-or-later
#pragma once

#include "IJsonConvert.hpp"
#include "Types.hpp"
#include "Error.hpp"

namespace netconf {

class JsonConverter : public IJsonConvert<IPConfigs>,
	public IJsonConvert<BridgeConfig>,
  public IJsonConvert<InterfaceConfigs>,
  public IJsonConvert<netconf::DipSwitchConfig>,
  public IJsonConvert<netconf::Error>,
  public IJsonConvert<netconf::DipSwitchIpConfig>,
  public IJsonConvert<netconf::IPConfig>,
  public IJsonConvert<netconf::InterfaceConfig>,
  public IJsonConvert<netconf::InterfaceInformation>,
  public IJsonConvert<netconf::InterfaceInformations> {
 public:

  ::std::string ToJsonString(const netconf::DipSwitchConfig &obj, JsonFormat format = JsonFormat::COMPACT) const noexcept override;
  Error FromJsonString(const ::std::string &str, netconf::DipSwitchConfig &out_obj) const noexcept override;

  ::std::string ToJsonString(const netconf::DipSwitchIpConfig &obj, JsonFormat format = JsonFormat::COMPACT) const
      noexcept override;
  Error FromJsonString(const ::std::string &str, netconf::DipSwitchIpConfig &out_obj) const noexcept override;

  ::std::string ToJsonString(const BridgeConfig &obj, JsonFormat format = JsonFormat::COMPACT) const noexcept override;
  Error FromJsonString(const ::std::string &str, BridgeConfig &out_obj) const noexcept override;

  ::std::string ToJsonString(const IPConfigs &obj, JsonFormat format = JsonFormat::COMPACT) const noexcept override;
  Error FromJsonString(const ::std::string &str, IPConfigs &out_obj) const noexcept override;

  ::std::string ToJsonString(const InterfaceConfigs &obj, JsonFormat format = JsonFormat::COMPACT) const noexcept override;
  Error FromJsonString(const ::std::string &str, InterfaceConfigs &out_obj) const noexcept override;

  ::std::string ToJsonString(const IPConfig &obj, JsonFormat format = JsonFormat::COMPACT) const noexcept override;
  Error FromJsonString(const ::std::string &str, IPConfig &out_obj) const noexcept override;

  ::std::string ToJsonString(const InterfaceConfig &obj, JsonFormat format = JsonFormat::COMPACT) const noexcept override;
  Error FromJsonString(const ::std::string &str, InterfaceConfig &out_obj) const noexcept override;

  ::std::string ToJsonString(const InterfaceInformation& obj, JsonFormat format = JsonFormat::COMPACT)const noexcept override;
  Error FromJsonString(const ::std::string& str, InterfaceInformation& out_obj)const noexcept override;

  ::std::string ToJsonString(const InterfaceInformations& obj, JsonFormat format = JsonFormat::COMPACT)const noexcept override;
  Error FromJsonString(const ::std::string& str, InterfaceInformations& out_obj)const noexcept override;

  ::std::string ToJsonString(const Error& obj, JsonFormat format = JsonFormat::COMPACT)const noexcept override;
  Error FromJsonString(const ::std::string& str, Error& out_obj)const noexcept override;

};

}  // namespace netconf
