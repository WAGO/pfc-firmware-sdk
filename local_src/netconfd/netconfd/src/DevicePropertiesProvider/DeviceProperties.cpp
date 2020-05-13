// SPDX-License-Identifier: GPL-2.0-or-later

#include <DeviceProperties.hpp>

namespace netconfd {

DeviceProperties::DeviceProperties(
    IBridgeController& bridge_controller)
    : bridge_controller_ { bridge_controller },
      type_label_provider_ { command_executer_ },
      interface_provider_ { bridge_controller_ } {

}

Interfaces DeviceProperties::GetProductInterfaces() const {
  return interface_provider_.GetProductInterfaces();
}

Interfaces DeviceProperties::GetOSInterfaces() const {
  return interface_provider_.GetOSInterfaces();
}

InterfaceNameMapping DeviceProperties::GetInterfacesNameMapping() const {
  return interface_provider_.GetInterfacesNameMapping();
}

void DeviceProperties::ConvertProductToOSInterfaces(Interfaces &interfaces) const {
  interface_provider_.ConvertProductToOSInterfaces(interfaces);
}

void DeviceProperties::ConvertOSToProductInterfaces(Interfaces &interfaces) const {
  interface_provider_.ConvertOSToProductInterfaces(interfaces);
}

bool DeviceProperties::HasInterface(const ::std::string& ifName) const {
  return interface_provider_.HasInterface(ifName);
}

::std::string DeviceProperties::GetMac() const {
  return type_label_provider_.GetMac();
}
::std::string DeviceProperties::GetIncrementedMac(uint32_t inc) const {
  return type_label_provider_.GetIncrementedMac(inc);
}

::std::string DeviceProperties::GetOrderNumber() const {
  return type_label_provider_.GetOrderNumber();
}

::std::string DeviceProperties::GetHostname() const {
  ::std::string hostname;
  system_properties_provider_.GetHostname(hostname);
  return hostname;
}

} /* namespace netconfd */
