// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include "IDeviceTypeLabelProvider.hpp"
#include "CommandExecutor.hpp"

namespace netconfd {

class DeviceTypeLabelProvider : public IDeviceTypeLabelProvider {

 public:
  DeviceTypeLabelProvider(CommandExecutor& executor);
  virtual ~DeviceTypeLabelProvider() = default;

  DeviceTypeLabelProvider(const DeviceTypeLabelProvider&) = delete;
  DeviceTypeLabelProvider& operator=(const DeviceTypeLabelProvider&) = delete;
  DeviceTypeLabelProvider(DeviceTypeLabelProvider&&) = delete;
  DeviceTypeLabelProvider& operator=(DeviceTypeLabelProvider&&) = delete;

  ::std::string GetOrderNumber() const override;
  ::std::string GetMac() const override;
  ::std::string GetIncrementedMac(uint32_t inc) const override;

 private:
  void GetMacFallback();
  void GetOrderFallback();

  CommandExecutor& executor_;

  ::std::string order_number_;
  ::std::string mac_;
  uint32_t mac_increment_;

};

} /* namespace netconfd */
