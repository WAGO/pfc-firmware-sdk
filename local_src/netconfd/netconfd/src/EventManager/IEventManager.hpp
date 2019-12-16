// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include "Types.hpp"

namespace netconfd {

class IEventManager {
 public:
  IEventManager() = default;
  virtual ~IEventManager() = default;
  IEventManager(const IEventManager&) = delete;
  IEventManager& operator=(const IEventManager&) = delete;
  IEventManager(const IEventManager&&) = delete;
  IEventManager& operator=(const IEventManager&&) = delete;

  virtual void NotifyNetworkChanges() = 0;

  virtual void NotifyIpChange(const Interface & interface) = 0;

 private:
};

}  // namespace netconfd
