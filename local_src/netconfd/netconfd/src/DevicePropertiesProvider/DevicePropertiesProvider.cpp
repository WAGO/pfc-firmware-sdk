// SPDX-License-Identifier: GPL-2.0-or-later

#include "DevicePropertiesProvider.hpp"

namespace netconfd {

DevicePropertiesProvider::DevicePropertiesProvider(
    IBridgeController& bridge_controller)
    : bridge_controller_ { bridge_controller },
      type_label_provider_ { command_executer_ },
      interface_provider_ { bridge_controller_ } {

}

Interfaces DevicePropertiesProvider::GetProductInterfaces() const {
  return interface_provider_.GetProductInterfaces();
}

Interfaces DevicePropertiesProvider::GetOSInterfaces() const {
  return interface_provider_.GetOSInterfaces();
}

InterfaceNameMapping DevicePropertiesProvider::GetInterfacesNameMapping() const {
  return interface_provider_.GetInterfacesNameMapping();
}

bool DevicePropertiesProvider::HasInterface(const ::std::string& ifName) const {
  return interface_provider_.HasInterface(ifName);
}

::std::string DevicePropertiesProvider::GetMac() const {
  return type_label_provider_.GetMac();
}
::std::string DevicePropertiesProvider::GetIncrementedMac(uint32_t inc) const {
  return type_label_provider_.GetIncrementedMac(inc);
}

::std::string DevicePropertiesProvider::GetOrderNumber() const {
  return type_label_provider_.GetOrderNumber();
}

::std::string DevicePropertiesProvider::GetHostname() const {
  ::std::string hostname;
  system_properties_provider_.GetHostname(hostname);
  return hostname;
}

} /* namespace netconfd */
