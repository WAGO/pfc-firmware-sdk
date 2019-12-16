// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include "IEventManager.hpp"
#include "IDevicePropertiesProvider.hpp"

namespace netconfd {

class EventManager : public IEventManager {
 public:
  EventManager(IDevicePropertiesProvider& device_properties_provider);
  virtual ~EventManager() = default;

  EventManager(const EventManager&) = delete;
  EventManager& operator=(const EventManager&) = delete;
  EventManager(const EventManager&&) = delete;
  EventManager& operator=(const EventManager&&) = delete;

  void NotifyNetworkChanges() override;

  void NotifyIpChange(const Interface & interface) override;

};

} /* namespace netconfd */
