// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include <string>
#include <stdint.h>

#include "Types.hpp"

namespace netconfd {

class IDeviceProperties {
 public:

  IDeviceProperties() = default;
  virtual ~IDeviceProperties() = default;
  IDeviceProperties(const IDeviceProperties&) = delete;
  IDeviceProperties& operator=(const IDeviceProperties&) = delete;
  IDeviceProperties(IDeviceProperties&&) = delete;
  IDeviceProperties& operator=(IDeviceProperties&&) = delete;

  virtual Interfaces GetProductInterfaces() const = 0;
  virtual Interfaces GetOSInterfaces() const = 0;
  virtual InterfaceNameMapping GetInterfacesNameMapping() const = 0;
  virtual void ConvertProductToOSInterfaces(Interfaces &interfaces) const = 0;
  virtual void ConvertOSToProductInterfaces(Interfaces &interfaces) const = 0;
  virtual bool HasInterface(const ::std::string& ifName) const = 0;
  virtual ::std::string GetOrderNumber() const = 0;
  virtual ::std::string GetMac() const = 0;
  virtual ::std::string GetIncrementedMac(uint32_t inc) const = 0;
  virtual ::std::string GetHostname() const = 0;

};

}  // namespace netconfd
