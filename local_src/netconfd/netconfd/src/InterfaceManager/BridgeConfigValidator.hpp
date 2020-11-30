// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include "IBridgeConfigValidator.hpp"
#include "IBridgeController.hpp"


namespace netconf {

class BridgeConfigValidator : public IBridgeConfigValidator {
 public:
  BridgeConfigValidator(IBridgeController& bridge_controller);
  virtual ~BridgeConfigValidator() = default;

  BridgeConfigValidator(const BridgeConfigValidator&) = delete;
  BridgeConfigValidator& operator=(const BridgeConfigValidator&) = delete;
  BridgeConfigValidator(const BridgeConfigValidator&&) = delete;
  BridgeConfigValidator& operator=(const BridgeConfigValidator&&) = delete;

  Error Validate(BridgeConfig const& config) const override;

 private:
  IBridgeController& bridge_controller_;

};

} /* namespace netconf */
