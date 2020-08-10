// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include "CommandExecutor.hpp"

namespace netconf {

class DeviceTypeLabelProvider {

 public:
  DeviceTypeLabelProvider(CommandExecutor& executor);
  virtual ~DeviceTypeLabelProvider() = default;

  DeviceTypeLabelProvider(const DeviceTypeLabelProvider&) = delete;
  DeviceTypeLabelProvider& operator=(const DeviceTypeLabelProvider&) = delete;
  DeviceTypeLabelProvider(DeviceTypeLabelProvider&&) = delete;
  DeviceTypeLabelProvider& operator=(DeviceTypeLabelProvider&&) = delete;

  ::std::string GetOrderNumber() const;
  ::std::string GetMac() const;
  ::std::string GetIncrementedMac(uint32_t inc) const;
  uint32_t GetMacCount() const;


 private:
  void GetMacFallback();
  void GetOrderFallback();

  CommandExecutor& executor_;

  ::std::string order_number_;
  ::std::string mac_;
  uint32_t mac_count_;

};

}
