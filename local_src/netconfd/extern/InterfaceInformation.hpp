// SPDX-License-Identifier: GPL-2.0-or-later
#pragma once

#include <string>
#include <vector>
#include "DeviceType.hpp"

namespace netconf {


class InterfaceInformation {
 public:

  InterfaceInformation() = default;
  InterfaceInformation(const ::std::string& interface_name, const ::std::string& interface_label, DeviceType type, bool ip_configuration_readonly = false);

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

 private:
  ::std::string interface_name_;
  ::std::string interface_label_;
  DeviceType type_;
  bool ip_configuration_readonly_;
};

using InterfaceInformations = ::std::vector<InterfaceInformation>;


}  // namespace netconf

//---- End of header file ------------------------------------------------------

