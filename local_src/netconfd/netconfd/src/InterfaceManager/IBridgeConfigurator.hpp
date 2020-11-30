// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include "Error.hpp"
#include "Types.hpp"

namespace netconf {

class IBridgeConfigurator {
 public:
  IBridgeConfigurator() = default;
  virtual ~IBridgeConfigurator() = default;

  IBridgeConfigurator(const IBridgeConfigurator&) = default;
  IBridgeConfigurator& operator=(const IBridgeConfigurator&) = default;
  IBridgeConfigurator(IBridgeConfigurator&&) = default;
  IBridgeConfigurator& operator=(IBridgeConfigurator&&) = default;

  virtual BridgeConfig GetConfiguration() const = 0;
  virtual Error Configure(BridgeConfig const& config) const = 0;

};

}  // namespace netconf
