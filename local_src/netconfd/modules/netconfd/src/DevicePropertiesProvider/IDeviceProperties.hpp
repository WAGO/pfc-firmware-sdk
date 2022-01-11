// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include <string>
#include <stdint.h>

#include "Types.hpp"

namespace netconf {

class IDeviceProperties {
 public:

  IDeviceProperties() = default;
  virtual ~IDeviceProperties() = default;
  IDeviceProperties(const IDeviceProperties&) = delete;
  IDeviceProperties& operator=(const IDeviceProperties&) = delete;
  IDeviceProperties(IDeviceProperties&&) = delete;
  IDeviceProperties& operator=(IDeviceProperties&&) = delete;

  virtual Interfaces GetProductPortNames() const = 0;
  virtual Interfaces GetOSPortNames() const = 0;
  virtual Interfaces GetOSInterfaceNames() const = 0;
  virtual InterfaceNameMapping GetInterfaceNameMapping() const = 0;
  virtual void ConvertProductToOSInterfaces(Interfaces &interfaces) const = 0;
  virtual void ConvertOSToProductInterfaces(Interfaces &interfaces) const = 0;
  virtual bool HasInterface(const ::std::string& ifName) const = 0;
  virtual ::std::string GetOrderNumber() const = 0;
  virtual MacAddress GetMac() const = 0;
  virtual uint32_t GetMacCount() const = 0;
  virtual ::std::string GetIncrementedMac(uint32_t inc) const = 0;
  virtual ::std::string GetHostname() const = 0;

};

}
