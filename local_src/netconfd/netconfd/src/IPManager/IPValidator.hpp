// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include "Status.hpp"
#include "Types.hpp"

#include "IIPController.hpp"
#include "INetDevManager.hpp"

namespace netconfd {

class IPValidator {
 public:
  IPValidator(const IIPController& ip_controller);
  virtual ~IPValidator() = default;

  IPValidator(const IPValidator&) = delete;
  IPValidator& operator=(const IPValidator&) = delete;
  IPValidator(const IPValidator&&) = delete;
  IPValidator& operator=(const IPValidator&&) = delete;

  Status ValidateIPConfigs(const IPConfigs& ip_configs, const bool interference_has_to_be_checked) const;

  bool IsInterfaceApplicableToSystem(const IPConfigs& ip_configs,
                                     const NetDevs& netdevs,
                                     const Interfaces& not_assignable_interface) const;

  Status ValidateIPConfigIsApplicableToSystem(const IPConfigs& ip_configs, const ::std::vector<::std::shared_ptr<NetDev>>& netdevs) const;

  bool IsSameSubnet(IPConfig lhs, IPConfig rhs) const;

 private:
  const IIPController& ip_controller_;
}
;

} /* namespace netconfd */
