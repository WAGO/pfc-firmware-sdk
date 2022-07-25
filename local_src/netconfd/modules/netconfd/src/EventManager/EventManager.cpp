// SPDX-License-Identifier: GPL-2.0-or-later

#include "EventManager.hpp"

#include <fcntl.h>
#include <glib.h>
#include <unistd.h>

#include <boost/filesystem.hpp>
#include <chrono>
#include <string>
#include <thread>

#include "CommandExecutor.hpp"
#include "FileOperations.hpp"
#include "JsonConverter.hpp"
#include "Logger.hpp"

namespace netconf {

namespace bfs = boost::filesystem;
using namespace std::literals;
#define IPV4_CHANGE_DIR "/var/run/ipv4"

static constexpr int wait_for_events_timeout_millis = 500;

namespace {

::std::vector<char> StringToCharVector(::std::string s) {
  ::std::vector<char> c(s.begin(), s.end());
  c.push_back('\0');
  return c;
}

}  // namespace

EventManager::EventManager() : trigger_event_folder_{false} {
  static_assert(sizeof(guint) == sizeof(debounce_events_gtimeout_id_));
}

void EventManager::ProcessPendingEvents() {
  LOG_DEBUG("--------------> EventManager::ProcessEventsPendingEvents");
  /*
   * Pending events were previously queued into the gmain loop as a gtask.
   * We prefer this task in the processing to ensure that the event folder has been called before a dbus call returned.
   * g_main_context_iteration processes all tasks currently pending in the gmainloop
   *
   * Pending tasks (e.g. Netlink address change) are given priority in order to see
   * whether they lead to an event folder call.
   */

  // process pending netlink events, or wait in hope to receive any
  if (g_main_context_iteration(nullptr, FALSE) == 0 || debounce_events_gtimeout_id_ == 0) {
    ::std::this_thread::sleep_for(::std::chrono::milliseconds(50));
    g_main_context_iteration(nullptr, FALSE);
  }

  // at least one event received, now we wait for the debounce timeout to elapse
  while (debounce_events_gtimeout_id_ != 0) {
    g_main_context_iteration(nullptr, TRUE);
  }

  LOG_DEBUG("--------------> EventManager::ProcessEventsPendingEvents finished");
}

void EventManager::PublishNetworkChangesToSystem() {
  UpdateIpChangeFiles();
  if (trigger_event_folder_) {
    CallEventFolderSync();
  }
  trigger_event_folder_ = false;
}

int EventManager::ProcessEvents(void *user) {
  EventManager *em = reinterpret_cast<EventManager *>(user);  // NOLINT: Need reinterpret_cast to cast from void*.
  em->debounce_events_gtimeout_id_ = 0;
  em->PublishNetworkChangesToSystem();
  return 0;  // The function added with g_timeout_add is called repeatedly until it returns 0
}

void EventManager::DeferProcessEvents() {
  /*
   * Here we are implementing a debouncing mechanism.
   * The event folder call is not executed directly for every event, but only after the timeout has expired.
   * During this time, events are combined and timeout is reset.
   */
  if (debounce_events_gtimeout_id_ != 0) {
    g_source_remove(debounce_events_gtimeout_id_);
  }

  debounce_events_gtimeout_id_ =
      g_timeout_add_full(G_PRIORITY_HIGH, wait_for_events_timeout_millis, &EventManager::ProcessEvents, this, nullptr);
}

void EventManager::NotifyNetworkChanges(const EventLayer event_layer, Interface interface) {
  switch (event_layer) {
    case EventLayer::EVENT_FOLDER:
      trigger_event_folder_ = true;
      break;

    case EventLayer::IP_CHANGE_FILES:
      if (not interface.empty()) {
        ip_interface_update_pending_.emplace(::std::move(interface));
      }
      break;
  }
  DeferProcessEvents();
}

void EventManager::RegisterNetworkInformation(IBridgeInformation &bridge_information, IIPInformation &ip_information,
                                              IInterfaceInformation &interface_information,
                                              INetDevManager &netdev_manager) {
  bridge_information_    = &bridge_information;
  ip_information_        = &ip_information;
  interface_information_ = &interface_information;

  bool dir_exists = bfs::exists(IPV4_CHANGE_DIR) || bfs::create_directory(IPV4_CHANGE_DIR);
  if (dir_exists) {
    bfs::permissions(IPV4_CHANGE_DIR,
                     bfs::owner_all | bfs::group_read | bfs::group_exe | bfs::others_read | bfs::others_exe);

    auto port_devs = netdev_manager.GetNetDevs(DeviceType::Port);
    for (size_t i = 0; i < port_devs.size(); i++) {
      NotifyNetworkChanges(EventLayer::IP_CHANGE_FILES, "br"s.append(std::to_string(i)));
    }

    auto wwans = netdev_manager.GetNetDevs(DeviceType::Wwan);
    for (auto &wwan_dev : wwans) {
      NotifyNetworkChanges(EventLayer::IP_CHANGE_FILES, wwan_dev->GetName());
    }
  }
}

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wwrite-strings"

::std::vector<char> EventManager::GetBridgeConfigAsJson() {
  BridgeConfig bridge_config;
  if (bridge_information_ != nullptr) {
    bridge_config = bridge_information_->GetBridgeConfig();
  }
  return StringToCharVector(JsonConverter().ToJsonString(bridge_config));
}

::std::vector<char> EventManager::GetIPConfigAsJson() {
  IPConfigs ip_config;
  if (ip_information_ != nullptr) {
    ip_config = ip_information_->GetCurrentIPConfigs();
  }
  return StringToCharVector(JsonConverter().ToJsonString(ip_config));
}

::std::vector<char> EventManager::GetInterfaceConfigAsJson() {
  InterfaceConfigs interface_config;
  if (interface_information_ != nullptr) {
    interface_config = interface_information_->GetPortConfigs();
  }
  return StringToCharVector(JsonConverter().ToJsonString(interface_config));
}

void EventManager::CallEventFolderSync() {
  auto bridge_config    = GetBridgeConfigAsJson();
  auto ip_config        = GetIPConfigAsJson();
  auto interface_config = GetInterfaceConfigAsJson();
  gchar *argv[]         = {"/usr/bin/run-parts", "-a", "config", "/etc/config-tools/events/networking", nullptr};
  g_setenv("NETCONF_BRIDGE_CONFIG", &bridge_config[0], static_cast<gboolean>(true));
  g_setenv("NETCONF_IP_CONFIG", &ip_config[0], static_cast<gboolean>(true));
  g_setenv("NETCONF_INTERFACE_CONFIG", &interface_config[0], static_cast<gboolean>(true));
  gint exit_status = 0;
  GError *error    = nullptr;
  if (g_spawn_sync(nullptr, static_cast<gchar **>(argv), nullptr, G_SPAWN_DEFAULT, nullptr, nullptr, nullptr, nullptr,
                   &exit_status, &error) != 0) {
    LOG_DEBUG("EventManager: called run-parts on /etc/config-tools/events/networking");
  } else {
    LogError("Failed to trigger /etc/config-tools/events/networking folder");
    g_error_free(error);
  }
}
#pragma GCC diagnostic pop

void EventManager::UpdateIpChangeFiles() {
  for (auto &interface : ip_interface_update_pending_) {
    ::std::string file = IPV4_CHANGE_DIR "/ipconchg-" + interface;

    TouchFile(file);
  }
  ip_interface_update_pending_.clear();
}

}  // namespace netconf
