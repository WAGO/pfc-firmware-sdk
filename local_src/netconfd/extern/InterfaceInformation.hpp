// SPDX-License-Identifier: LGPL-3.0-or-later
#pragma once

#include <string>
#include <vector>
#include "DeviceType.hpp"
#include "LinkMode.hpp"

namespace netconf {

class InterfaceInformation {
 public:

  InterfaceInformation() = default;
  InterfaceInformation(const ::std::string &interface_name, const ::std::string &interface_label, DeviceType type,
                       bool ip_configuration_readonly = false, AutonegotiationSupported autoneg_supported = {
                           AutonegotiationSupported::UNKNOWN },
                       LinkModes link_modes = LinkModes { });

  ::std::string GetInterfaceLabel() const {
    return interface_label_;
  }

  ::std::string GetInterfaceName() const {
    return interface_name_;
  }

  bool IsIpConfigurationReadonly() const {
    return ip_configuration_readonly_;
  }

  DeviceType GetType() const {
    return type_;
  }

  AutonegotiationSupported GetAutonegSupported() const {
    return autoneg_supported_;
  }

  LinkModes GetSupportedlinkModes() const {
    return link_modes_;
  }

  bool operator==(const InterfaceInformation &ii) const {
    return ::std::tie(this->interface_name_, this->interface_label_, this->type_, this->ip_configuration_readonly_,
                      this->autoneg_supported_, this->link_modes_)
        == ::std::tie(ii.interface_name_, ii.interface_label_, ii.type_, ii.ip_configuration_readonly_,
                      ii.autoneg_supported_, ii.link_modes_);
  }

 private:
  ::std::string interface_name_;
  ::std::string interface_label_;
  DeviceType type_;
  bool ip_configuration_readonly_;
  AutonegotiationSupported autoneg_supported_;
  LinkModes link_modes_;
};

using InterfaceInformations = ::std::vector<InterfaceInformation>;

}  // namespace netconf

//---- End of header file ------------------------------------------------------

