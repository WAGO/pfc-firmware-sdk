// SPDX-License-Identifier: GPL-2.0-or-later

#include <BridgeConfigTransformator.hpp>
#include <algorithm>

namespace netconfd {


void BridgeConfigTransformator::ProductToOS(Interfaces& interfaces) const{

  for (uint32_t i = 0; i < interfaces.size(); i++) {  // NOLINT(modernize-loop-convert) need index here
    auto it = interface_name_mapping_.find(interfaces[i]);
    if (it != interface_name_mapping_.cend()) {
      const Interface os_itf_name = it->second;
      interfaces[i] = os_itf_name;
    }
  }
}

void BridgeConfigTransformator::OSToProduct(Interfaces& interfaces) const{

  for (uint32_t i = 0; i < interfaces.size(); i++) {  // NOLINT(modernize-loop-convert) need index here

    ::std::string interface = interfaces[i];
    auto it = std::find_if(
        interface_name_mapping_.begin(), interface_name_mapping_.end(),
        [interface](const auto& itf) {return itf.second == interface;});

    if (it != interface_name_mapping_.end()) {
      Interface product_itf_name = it->first;
      interfaces[i] = product_itf_name;
    }
  }
}

BridgeConfigTransformator::BridgeConfigTransformator(
    const IDevicePropertiesProvider& properties_provider)
    : interface_name_mapping_ { properties_provider.GetInterfacesNameMapping() } {
}

BridgeConfig BridgeConfigTransformator::ConvertProductToOS(
    const BridgeConfig& config) const {
  BridgeConfig config_os = config;

  for (auto& bridge_pair : config_os) {
    auto& interfaces = bridge_pair.second;
    ProductToOS(interfaces);
  }

  return config_os;
}

BridgeConfig BridgeConfigTransformator::ConvertOSToProduct(
    const BridgeConfig& config) const {
  BridgeConfig config_product = config;

  for (auto& bridge_pair : config_product) {
    auto& interfaces = bridge_pair.second;
    OSToProduct(interfaces);
  }

  return config_product;
}

Interfaces BridgeConfigTransformator::ConvertProductToOS(
    Interfaces const& interfaces) const {

  Interfaces os_interfaces = interfaces;
  ProductToOS(os_interfaces);

  return os_interfaces;
}

Interfaces BridgeConfigTransformator::ConvertOSToProduct(
    Interfaces const& interfaces) const {

  Interfaces product_interfaces = interfaces;
  OSToProduct(product_interfaces);

  return product_interfaces;
}

} /* namespace netconfd */
