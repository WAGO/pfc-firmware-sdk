// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include "Status.hpp"
#include "Types.hpp"

namespace netconf {

class IPValidator {
 public:
  IPValidator() = default;
  virtual ~IPValidator() = default;

  IPValidator(const IPValidator&) = delete;
  IPValidator& operator=(const IPValidator&) = delete;
  IPValidator(const IPValidator&&) = delete;
  IPValidator& operator=(const IPValidator&&) = delete;

  static Status ValidateIPConfigs(const IPConfigs &ip_configs);
  static Status ValidateCombinabilityOfIPConfigs(const IPConfigs &lhs_ip_configs, const IPConfigs &rhs_ip_configs);
  static Status ValidateExistenceAndAccess(const IPConfigs &ip_configs, const InterfaceInformations &interface_information);
  static Status Validate(IPConfigs candidate, IPConfigs current, const InterfaceInformations& interface_information);
  static IPConfigs FilterValidStaticIPConfigs(const IPConfigs &ip_configs);

  static bool IsSameSubnet(const IPConfig& lhs, const IPConfig& rhs);



};

}
