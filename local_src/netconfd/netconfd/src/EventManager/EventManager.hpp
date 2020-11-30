// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include "INetDevManager.hpp"
#include "IEventManager.hpp"
#include "IBridgeInformation.hpp"
#include "IIPInformation.hpp"
#include "IInterfaceInformation.hpp"
#include <set>
#include <vector>
namespace netconf {

class EventManager : public IEventManager {
 public:
  EventManager(INetDevManager &netdev_manager);
  virtual ~EventManager() = default;

  EventManager(const EventManager&) = delete;
  EventManager& operator=(const EventManager&) = delete;
  EventManager(const EventManager&&) = delete;
  EventManager& operator=(const EventManager&&) = delete;

  void NotifyNetworkChanges(const EventType event_type, const EventLayer event_layer, Interface interface = "")
      override;

  void ProcessEvents() override;

  void RegisterNetworkInformation(IBridgeInformation &bridge_information, IIPInformation &ip_information,
                                  IInterfaceInformation &interface_information);

 private:

  IBridgeInformation *bridge_information_ = nullptr;
  IIPInformation *ip_information_ = nullptr;
  IInterfaceInformation *interface_information_ = nullptr;

  bool trigger_event_folder_;
  ::std::set<::std::string> ip_interface_update_pending_;

  friend void NotifyNetworkChangesFriend(EventManager&);
  void CommitNetworkChangesDone();
  void SpawnProcess();
  void UpdateIpChangeFiles();

  ::std::vector<char> GetBridgeConfigAsJson();
  ::std::vector<char> GetIPConfigAsJson();
  ::std::vector<char> GetInterfaceConfigAsJson();
};

}
