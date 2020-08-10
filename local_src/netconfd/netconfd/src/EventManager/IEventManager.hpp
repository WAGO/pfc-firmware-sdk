// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include "Types.hpp"

namespace netconf {

enum class EventType {
  SYSTEM,
  USER
};

enum class EventLayer {
  EVENT_FOLDER,
  IP_CHANGE_FILES
};

class IEventManager {
 public:
  IEventManager() = default;
  virtual ~IEventManager() = default;
  IEventManager(const IEventManager&) = delete;
  IEventManager& operator=(const IEventManager&) = delete;
  IEventManager(const IEventManager&&) = delete;
  IEventManager& operator=(const IEventManager&&) = delete;

  virtual void NotifyNetworkChanges(const EventType event_type, const EventLayer event_layer, Interface interface = Interface{}) = 0;
  virtual void ProcessEvents() = 0;

};

}
