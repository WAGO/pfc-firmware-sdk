// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include "Types.hpp"
#include <string>
#include "IBridgeController.hpp"
#include "IDeviceInterfaceProvider.hpp"

namespace netconf {

class DeviceInterfaceProvider : public IDeviceInterfaceProvider {
 public:
  explicit DeviceInterfaceProvider(IBridgeController &bridge_controller);
  virtual ~DeviceInterfaceProvider() = default;

  DeviceInterfaceProvider(const DeviceInterfaceProvider&) = delete;
  DeviceInterfaceProvider& operator=(const DeviceInterfaceProvider&) = delete;
  DeviceInterfaceProvider(DeviceInterfaceProvider&&) = delete;
  DeviceInterfaceProvider& operator=(DeviceInterfaceProvider&&) = delete;

  Interfaces GetProductPortNames() const;
  Interfaces GetOSPortNames() const override;
  Interfaces GetOSInterfaceNames() const override;
  InterfaceNameMapping GetInterfacesNameMapping() const;

  void ConvertProductToOSInterfaces(Interfaces &interfaces) const;
  void ConvertOSToProductInterfaces(Interfaces &interfaces) const;

  bool HasInterface(const ::std::string &interface_name) const;

 private:
  void GenerateInterfaceNameMapping();

  IBridgeController &bridge_controller_;

  Interfaces product_port_interfaces_;
  Interfaces os_port_interfaces_;
  Interfaces os_interfaces_;
  InterfaceNameMapping interface_name_mapping_;

};

}
