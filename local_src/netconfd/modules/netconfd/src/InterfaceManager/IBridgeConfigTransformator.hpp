
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include "Types.hpp"

namespace netconf {

class IBridgeConfigTransformator {
 public:
  IBridgeConfigTransformator() = default;
  virtual ~IBridgeConfigTransformator() = default;
  IBridgeConfigTransformator(const IBridgeConfigTransformator&) = default;
  IBridgeConfigTransformator& operator=(const IBridgeConfigTransformator&) = default;
  IBridgeConfigTransformator(IBridgeConfigTransformator&&) = default;
  IBridgeConfigTransformator& operator=(IBridgeConfigTransformator&&) = default;

  /**
   * Convert the given product interface label names to the OS specific names representing these interfaces.
   */
  virtual BridgeConfig ConvertProductToOS(const BridgeConfig& config) const = 0;

  /**
   * Convert the OS interface names to the WAGO product interface labels.
   */
  virtual BridgeConfig ConvertOSToProduct(const BridgeConfig& config) const = 0;

  virtual Interfaces ConvertProductToOS(Interfaces const& interfaces) const = 0;

  virtual Interfaces ConvertOSToProduct(Interfaces const& interfaces) const = 0;

};

} /* namespace netconf */
