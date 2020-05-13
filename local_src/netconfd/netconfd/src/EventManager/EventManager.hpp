// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include "IDeviceProperties.hpp"
#include "IEventManager.hpp"
#include <set>
namespace netconfd {

class EventManager : public IEventManager {
 public:
  EventManager(IDeviceProperties& device_properties_provider);
  virtual ~EventManager() = default;

  EventManager(const EventManager&) = delete;
  EventManager& operator=(const EventManager&) = delete;
  EventManager(const EventManager&&) = delete;
  EventManager& operator=(const EventManager&&) = delete;

  void NotifyNetworkChanges(const EventType event_type, const EventLayer event_layer, Interface interface = "") override;

  void ProcessEvents() override;

 private:

  bool trigger_event_folder_;
  ::std::set<::std::string> ip_interface_update_pending_;

  friend void NotifyNetworkChangesFriend(EventManager&);
  void CommitNetworkChangesDone();
  void SpawnProcess();
  void UpdateIpChangeFiles();
};

} /* namespace netconfd */
