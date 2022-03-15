// SPDX-License-Identifier: GPL-2.0-or-later
#pragma once

#include <string>
#include <memory>
#include "IPLink.hpp"
#include "Status.hpp"

namespace netconf {

class IIPConfigure {
 public:
  IIPConfigure() = default;
  virtual ~IIPConfigure() = default;

  IIPConfigure(const IIPConfigure&) = delete;
  IIPConfigure& operator=(const IIPConfigure&) = delete;
  IIPConfigure(IIPConfigure&&) = delete;
  IIPConfigure& operator=(IIPConfigure&&) = delete;

  virtual Status Configure(const IPLinkPtr link) const = 0;
  virtual void Flush(const IPLinkPtr link) const = 0;

};

}
