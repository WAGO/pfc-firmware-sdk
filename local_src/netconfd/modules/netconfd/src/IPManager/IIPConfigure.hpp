// SPDX-License-Identifier: GPL-2.0-or-later
#pragma once

#include <string>
#include <memory>

namespace netconf {

class IIPConfigure {
 public:
  IIPConfigure() = default;
  virtual ~IIPConfigure() = default;

  IIPConfigure(const IIPConfigure&) = delete;
  IIPConfigure& operator=(const IIPConfigure&) = delete;
  IIPConfigure(IIPConfigure&&) = delete;
  IIPConfigure& operator=(IIPConfigure&&) = delete;

  virtual Status Configure(const IPConfig& config) const = 0;

};

}
