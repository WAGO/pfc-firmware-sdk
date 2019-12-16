// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include "Status.hpp"
#include "Types.hpp"

#include "IIPController.hpp"

namespace netconfd {

class IPValidator {
 public:
  IPValidator(const IIPController& ip_controller);
  virtual ~IPValidator() = default;

  IPValidator(const IPValidator&) = delete;
  IPValidator& operator=(const IPValidator&) = delete;
  IPValidator(const IPValidator&&) = delete;
  IPValidator& operator=(const IPValidator&&) = delete;

  Status ValidateIPConfigs(const IPConfigs& ip_configs,
                           const bool interference_has_to_be_checked) const;

  bool IsInterfaceApplicableToSystem(const IPConfigs& ip_configs,
                            const Interfaces& system_interface,
                            const Interfaces& not_assignable_interface) const;

  Status ValidateIPConfigIsApplicableToSystem(
      const IPConfigs& ip_configs, const Interfaces& system_interface) const;

 private:
  const IIPController& ip_controller_;
}
;

} /* namespace netconfd */
