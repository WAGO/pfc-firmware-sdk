// SPDX-License-Identifier: GPL-2.0-or-later
//------------------------------------------------------------------------------
///  \file     DeviceSettings.cpp
///
///  \brief    <short description of the file contents>
///
///  \author   <author> : WAGO Kontakttechnik GmbH & Co. KG
//------------------------------------------------------------------------------
#include <DeviceInterfaceProvider.hpp>
#include <fstream>
#include <sstream>
#include <iostream>
#include <algorithm>

namespace netconf {

DeviceInterfaceProvider::DeviceInterfaceProvider(IBridgeController& bridge_controller)
    : bridge_controller_ { bridge_controller } {

  GenerateInterfaceNameMapping();

}

Interfaces DeviceInterfaceProvider::GetProductPortNames() const {
  return product_port_interfaces_;
}

Interfaces DeviceInterfaceProvider::GetOSPortNames() const {
  return os_port_interfaces_;
}

Interfaces DeviceInterfaceProvider::GetOSInterfaceNames() const {
  return os_interfaces_;
}

InterfaceNameMapping DeviceInterfaceProvider::GetInterfacesNameMapping() const {
  return interface_name_mapping_;
}

void DeviceInterfaceProvider::GenerateInterfaceNameMapping() {

  const ::std::string interface_matcher = "ethX";
  const ::std::string os_interface_prefix = "eth";

  os_interfaces_ = bridge_controller_.GetInterfaces();

  for (auto& os_interface : os_interfaces_) {

    size_t pos = os_interface.find(interface_matcher);

    if (pos == 0) {

      std::string product_interface = os_interface;
      product_interface.erase(0, os_interface_prefix.size());

      os_port_interfaces_.push_back(os_interface);
      product_port_interfaces_.push_back(product_interface);
      InterfaceNamesPair pair(product_interface, os_interface);
      interface_name_mapping_.insert(::std::move(pair));
    }
  }
}

bool DeviceInterfaceProvider::HasInterface(const ::std::string& interface_name) const {

  if (not interface_name.empty()) {
    Interfaces interfaces = bridge_controller_.GetInterfaces();
    if (std::find(interfaces.begin(), interfaces.end(), interface_name) != interfaces.end()) {
      return true;
    }
  }
  return false;
}


void DeviceInterfaceProvider::ConvertProductToOSInterfaces(Interfaces& interfaces) const{

  for (uint32_t i = 0; i < interfaces.size(); i++) {  // NOLINT(modernize-loop-convert) need index here
    auto it = interface_name_mapping_.find(interfaces[i]);
    if (it != interface_name_mapping_.cend()) {
      const Interface os_itf_name = it->second;
      interfaces[i] = os_itf_name;
    }
  }
}

void DeviceInterfaceProvider::ConvertOSToProductInterfaces(Interfaces& interfaces) const{

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


} /* namespace netconf */

