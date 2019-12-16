// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include "Status.hpp"
#include "Types.hpp"

namespace netconfd {

class IBridgeConfigurator {
 public:
  IBridgeConfigurator() = default;
  virtual ~IBridgeConfigurator() = default;

  IBridgeConfigurator(const IBridgeConfigurator&) = default;
  IBridgeConfigurator& operator=(const IBridgeConfigurator&) = default;
  IBridgeConfigurator(IBridgeConfigurator&&) = default;
  IBridgeConfigurator& operator=(IBridgeConfigurator&&) = default;

  virtual BridgeConfig GetConfiguration() const = 0;
  virtual Status Configure(BridgeConfig const& config) const = 0;
  virtual Status SetBridgeUp(const Bridge& bridge) const = 0;
  virtual Status SetBridgeDown(const Bridge& bridge) const = 0;

};

}  // namespace netconfd
