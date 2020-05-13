// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include "IBridgeController.hpp"
#include "DeviceInterfaceProvider.hpp"
#include "DeviceTypeLabelProvider.hpp"
#include "IBridgeConfigTransformator.hpp"
#include "IDeviceProperties.hpp"
#include "SystemPropertiesProvider.hpp"

namespace netconfd {

class DeviceProperties : public IDeviceProperties {
 public:
  DeviceProperties(IBridgeController &bridge_controller);
  virtual ~DeviceProperties() = default;

  DeviceProperties(const DeviceProperties&) = delete;
  DeviceProperties& operator=(const DeviceProperties&) = delete;
  DeviceProperties(DeviceProperties&&) = delete;
  DeviceProperties& operator=(DeviceProperties&&) = delete;

  Interfaces GetProductInterfaces() const override;
  Interfaces GetOSInterfaces() const override;
  InterfaceNameMapping GetInterfacesNameMapping() const override;
  void ConvertProductToOSInterfaces(Interfaces &interfaces) const override;
  void ConvertOSToProductInterfaces(Interfaces &interfaces) const override;
  bool HasInterface(const ::std::string &ifName) const override;
  ::std::string GetOrderNumber() const override;
  ::std::string GetMac() const override;
  ::std::string GetIncrementedMac(uint32_t inc) const override;
  ::std::string GetHostname() const override;

 private:

  IBridgeController &bridge_controller_;
  CommandExecutor command_executer_;
  DeviceTypeLabelProvider type_label_provider_;
  DeviceInterfaceProvider interface_provider_;
  SystemPropertiesProvider system_properties_provider_;
};

} /* namespace netconfd */
