// SPDX-License-Identifier: GPL-2.0-or-later

#include "BridgeConfigTransformator.hpp"

#include <algorithm>

namespace netconf {

BridgeConfigTransformator::BridgeConfigTransformator(const IDeviceProperties& properties_provider)
    : device_properties_{properties_provider} {
}

BridgeConfig BridgeConfigTransformator::ConvertProductToOS(const BridgeConfig& config) const {
  BridgeConfig config_os = config;

  for (auto& bridge_pair : config_os) {
    auto& interfaces = bridge_pair.second;
    device_properties_.ConvertProductToOSInterfaces(interfaces);
  }

  return config_os;
}

BridgeConfig BridgeConfigTransformator::ConvertOSToProduct(const BridgeConfig& config) const {
  BridgeConfig config_product = config;

  for (auto& bridge_pair : config_product) {
    auto& interfaces = bridge_pair.second;
    device_properties_.ConvertOSToProductInterfaces(interfaces);
  }

  return config_product;
}

Interfaces BridgeConfigTransformator::ConvertProductToOS(Interfaces const& interfaces) const {
  Interfaces os_interfaces = interfaces;
  device_properties_.ConvertProductToOSInterfaces(os_interfaces);

  return os_interfaces;
}

Interfaces BridgeConfigTransformator::ConvertOSToProduct(Interfaces const& interfaces) const {
  Interfaces product_interfaces = interfaces;
  device_properties_.ConvertOSToProductInterfaces(product_interfaces);

  return product_interfaces;
}

} /* namespace netconf */
