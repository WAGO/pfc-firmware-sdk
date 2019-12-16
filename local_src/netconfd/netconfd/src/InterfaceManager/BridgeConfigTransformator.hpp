// SPDX-License-Identifier: GPL-2.0-or-later
#pragma once

#include "IBridgeConfigTransformator.hpp"
#include "IDevicePropertiesProvider.hpp"

namespace netconfd {

class BridgeConfigTransformator : public IBridgeConfigTransformator {
 public:
  BridgeConfigTransformator(const IDevicePropertiesProvider& properties_provider);

  BridgeConfig ConvertProductToOS(const BridgeConfig& config) const override;

  BridgeConfig ConvertOSToProduct(const BridgeConfig& config) const override;

  Interfaces ConvertProductToOS(Interfaces const& interfaces) const override;

  Interfaces ConvertOSToProduct(Interfaces const& interfaces) const override;

 private:

  void ProductToOS(Interfaces& interfaces) const;
  void OSToProduct(Interfaces& interfaces) const;

  const InterfaceNameMapping interface_name_mapping_;

};

} /* namespace netconfd */
