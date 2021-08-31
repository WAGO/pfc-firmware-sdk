// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include <string>

namespace network_config {

class IHandler {
 public:
  IHandler() = default;
  virtual ~IHandler() = default;

  IHandler(const IHandler&) = delete;
  IHandler& operator=(const IHandler&) = delete;
  IHandler(const IHandler&&) = delete;
  IHandler& operator=(const IHandler&&) = delete;

  virtual void Execute() = 0;
};

}
