// SPDX-License-Identifier: GPL-2.0-or-later

#include "DeviceProperties.hpp"

namespace netconf {

DeviceProperties::DeviceProperties(IBridgeController& bridge_controller)
    : bridge_controller_{bridge_controller},
      type_label_provider_{command_executer_},
      interface_provider_{bridge_controller_} {
}

Interfaces DeviceProperties::GetProductPortNames() const {
  return interface_provider_.GetProductPortNames();
}

Interfaces DeviceProperties::GetOSPortNames() const {
  return interface_provider_.GetOSPortNames();
}

Interfaces DeviceProperties::GetOSInterfaceNames() const {
  return interface_provider_.GetOSInterfaceNames();
}

InterfaceNameMapping DeviceProperties::GetInterfaceNameMapping() const {
  return interface_provider_.GetInterfacesNameMapping();
}

void DeviceProperties::ConvertProductToOSInterfaces(Interfaces& interfaces) const {
  interface_provider_.ConvertProductToOSInterfaces(interfaces);
}

void DeviceProperties::ConvertOSToProductInterfaces(Interfaces& interfaces) const {
  interface_provider_.ConvertOSToProductInterfaces(interfaces);
}

bool DeviceProperties::HasInterface(const ::std::string& ifName) const {
  return interface_provider_.HasInterface(ifName);
}

MacAddress DeviceProperties::GetMac() const {
  return MacAddress::FromString(type_label_provider_.GetMac());
}
::std::string DeviceProperties::GetIncrementedMac(uint32_t inc) const {
  return type_label_provider_.GetIncrementedMac(inc);
}

::std::string DeviceProperties::GetOrderNumber() const {
  return type_label_provider_.GetOrderNumber();
}

::std::string DeviceProperties::GetHostname() const {
  return system_properties_provider_.GetHostname();
}

uint32_t DeviceProperties::GetMacCount() const {
  return type_label_provider_.GetMacCount();
}

}  // namespace netconf
