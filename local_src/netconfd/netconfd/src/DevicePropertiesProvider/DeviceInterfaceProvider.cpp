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

namespace netconfd {

DeviceInterfaceProvider::DeviceInterfaceProvider(IBridgeController& bridge_controller)
    : bridge_controller_ { bridge_controller } {

  GenerateInterfaceNameMapping();

}

Interfaces DeviceInterfaceProvider::GetProductInterfaces() const {
  return product_interfaces_;
}

Interfaces DeviceInterfaceProvider::GetOSInterfaces() const {
  return eth_os_interfaces_;
}

InterfaceNameMapping DeviceInterfaceProvider::GetInterfacesNameMapping() const {
  return interface_name_mapping_;
}

void DeviceInterfaceProvider::GenerateInterfaceNameMapping() {

  const ::std::string interface_matcher = "ethX";
  const ::std::string os_interface_prefix = "eth";

  Interfaces all_os_interfaces = bridge_controller_.GetInterfaces();

  for (auto& os_interface : all_os_interfaces) {

    size_t pos = os_interface.find(interface_matcher);

    if (pos == 0) {

      std::string product_interface = os_interface;
      product_interface.erase(0, os_interface_prefix.size());

      eth_os_interfaces_.push_back(os_interface);
      product_interfaces_.push_back(product_interface);
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

} /* namespace netconfd */

