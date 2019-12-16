// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include "Types.hpp"
#include <string>
#include "IBridgeController.hpp"

namespace netconfd {

class DeviceInterfaceProvider {
 public:
  explicit DeviceInterfaceProvider(IBridgeController& bridge_controller);
  virtual ~DeviceInterfaceProvider() = default;

  DeviceInterfaceProvider(const DeviceInterfaceProvider&) = delete;
  DeviceInterfaceProvider& operator=(const DeviceInterfaceProvider&) = delete;
  DeviceInterfaceProvider(DeviceInterfaceProvider&&) = delete;
  DeviceInterfaceProvider& operator=(DeviceInterfaceProvider&&) = delete;

  Interfaces GetProductInterfaces() const;
  Interfaces GetOSInterfaces() const;
  InterfaceNameMapping GetInterfacesNameMapping() const;

  bool HasInterface(const ::std::string& interface_name) const;

 private:
  void GenerateInterfaceNameMapping();

  IBridgeController& bridge_controller_;

  Interfaces product_interfaces_;
  Interfaces eth_os_interfaces_;
  InterfaceNameMapping interface_name_mapping_;

};

} /* namespace netconfd */
