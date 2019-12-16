// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include <string>
#include <stdint.h>

namespace netconfd {

  class IDeviceTypeLabelProvider {
   public:

    IDeviceTypeLabelProvider() = default;
    virtual ~IDeviceTypeLabelProvider() = default;
    IDeviceTypeLabelProvider(const IDeviceTypeLabelProvider&) = delete;
    IDeviceTypeLabelProvider& operator=(const IDeviceTypeLabelProvider&) = delete;
    IDeviceTypeLabelProvider(IDeviceTypeLabelProvider&&) = delete;
    IDeviceTypeLabelProvider& operator=(IDeviceTypeLabelProvider&&) = delete;


    virtual ::std::string GetOrderNumber() const = 0;
    virtual ::std::string GetMac() const = 0;
    virtual ::std::string GetIncrementedMac(uint32_t inc) const = 0;
  };


}  // namespace netconfd
