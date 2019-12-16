// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include "IBridgeController.hpp"
#include "IDevicePropertiesProvider.hpp"
#include "DeviceInterfaceProvider.hpp"
#include "DeviceTypeLabelProvider.hpp"
#include "IBridgeConfigTransformator.hpp"
#include "SystemPropertiesProvider.hpp"

namespace netconfd {

class DevicePropertiesProvider : public IDevicePropertiesProvider{
 public:
  DevicePropertiesProvider(IBridgeController& bridge_controller);
  virtual ~DevicePropertiesProvider() = default;

  DevicePropertiesProvider(const DevicePropertiesProvider&) = delete;
  DevicePropertiesProvider& operator=(const DevicePropertiesProvider&) = delete;
  DevicePropertiesProvider(DevicePropertiesProvider&&) = delete;
  DevicePropertiesProvider& operator=(DevicePropertiesProvider&&) = delete;

  Interfaces GetProductInterfaces() const override;
  Interfaces GetOSInterfaces() const override;
  InterfaceNameMapping GetInterfacesNameMapping() const override;
  bool HasInterface(const ::std::string& ifName) const override;
  ::std::string GetOrderNumber() const override;
  ::std::string GetMac() const override;
  ::std::string GetIncrementedMac(uint32_t inc) const override;
  ::std::string GetHostname() const override;

 private:

  IBridgeController& bridge_controller_;
  CommandExecutor command_executer_;
  DeviceTypeLabelProvider type_label_provider_;
  DeviceInterfaceProvider interface_provider_;
  SystemPropertiesProvider system_properties_provider_;
};

} /* namespace netconfd */
