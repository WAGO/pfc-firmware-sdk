// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include "Error.hpp"
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

  static Error ValidateIPConfigs(const IPConfigs &ip_configs);
  static Error ValidateCombinabilityOfIPConfigs(const IPConfigs &lhs_ip_configs, const IPConfigs &rhs_ip_configs);

  static IPConfigs FilterValidStaticAndTemporaryIPConfigs(const IPConfigs &ip_configs);
  static bool IsSameSubnet(IPConfig lhs, IPConfig rhs);

};

}
