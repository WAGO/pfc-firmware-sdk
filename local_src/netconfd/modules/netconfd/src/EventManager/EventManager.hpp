// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include <set>
#include <vector>

#include "IBridgeInformation.hpp"
#include "IEventManager.hpp"
#include "IIPInformation.hpp"
#include "IInterfaceInformation.hpp"
#include "INetDevManager.hpp"

namespace netconf {

class EventManager : public IEventManager {
 public:
  EventManager();
  virtual ~EventManager() = default;

  EventManager(const EventManager &) = delete;
  EventManager &operator=(const EventManager &) = delete;
  EventManager(const EventManager &&)           = delete;
  EventManager &operator=(const EventManager &&) = delete;

  void NotifyNetworkChanges(const EventLayer event_layer, Interface interface) override;

  void ProcessPendingEvents() override;

  void RegisterNetworkInformation(IBridgeInformation &bridge_information, IIPInformation &ip_information,
                                  IInterfaceInformation &interface_information, INetDevManager &netdev_manager);

 private:
  IBridgeInformation *bridge_information_       = nullptr;
  IIPInformation *ip_information_               = nullptr;
  IInterfaceInformation *interface_information_ = nullptr;

  bool trigger_event_folder_;
  ::std::set<::std::string> ip_interface_update_pending_;

  uint32_t debounce_events_gtimeout_id_ = 0;

  void CallEventFolderSync();
  void UpdateIpChangeFiles();
  void DeferProcessEvents();
  void PublishNetworkChangesToSystem();

  static int ProcessEvents(void *user);

  ::std::vector<char> GetBridgeConfigAsJson();
  ::std::vector<char> GetIPConfigAsJson();
  ::std::vector<char> GetInterfaceConfigAsJson();
};

}  // namespace netconf
