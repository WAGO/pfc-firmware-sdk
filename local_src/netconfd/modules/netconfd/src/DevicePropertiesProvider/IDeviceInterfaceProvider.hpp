// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include <string>
#include <stdint.h>

#include "Types.hpp"

namespace netconf {

class IDeviceInterfaceProvider {
 public:

  IDeviceInterfaceProvider() = default;
  virtual ~IDeviceInterfaceProvider() = default;
  IDeviceInterfaceProvider(const IDeviceInterfaceProvider&) = delete;
  IDeviceInterfaceProvider& operator=(const IDeviceInterfaceProvider&) = delete;
  IDeviceInterfaceProvider(IDeviceInterfaceProvider&&) = delete;
  IDeviceInterfaceProvider& operator=(IDeviceInterfaceProvider&&) = delete;

  virtual Interfaces GetOSPortNames() const = 0;
  virtual Interfaces GetOSInterfaceNames() const = 0;

};

}
