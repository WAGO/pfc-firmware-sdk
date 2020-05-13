// SPDX-License-Identifier: GPL-2.0-or-later
#pragma once

#include "IDeviceProperties.hpp"
#include "IBridgeConfigTransformator.hpp"

namespace netconfd {

class BridgeConfigTransformator : public IBridgeConfigTransformator {
 public:
  BridgeConfigTransformator(const IDeviceProperties& properties_provider);

  BridgeConfig ConvertProductToOS(const BridgeConfig& config) const override;

  BridgeConfig ConvertOSToProduct(const BridgeConfig& config) const override;

  Interfaces ConvertProductToOS(Interfaces const& interfaces) const override;

  Interfaces ConvertOSToProduct(Interfaces const& interfaces) const override;

 private:

  void ProductToOS(Interfaces& interfaces) const;
  void OSToProduct(Interfaces& interfaces) const;

  const IDeviceProperties& device_properties_;

};

} /* namespace netconfd */
