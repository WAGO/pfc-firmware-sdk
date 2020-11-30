// SPDX-License-Identifier: GPL-2.0-or-later

#include "EventManager.hpp"

#include <fcntl.h>
#include <unistd.h>
#include <boost/filesystem.hpp>
#include <glib.h>
#include <string>
#include "CommandExecutor.hpp"
#include "Logger.hpp"
#include "JsonConverter.hpp"
#include "FileOperations.hpp"

namespace netconf {

namespace bfs = boost::filesystem;
using namespace std::literals;
#define IPV4_CHANGE_DIR "/var/run/ipv4"

EventManager::EventManager(INetDevManager &netdev_manager)
    : trigger_event_folder_ { false } {

  bool dir_exists = bfs::exists(IPV4_CHANGE_DIR) || bfs::create_directory(IPV4_CHANGE_DIR);
  if (dir_exists) {
    bfs::permissions(
    IPV4_CHANGE_DIR,
                     bfs::owner_all | bfs::group_read | bfs::group_exe | bfs::others_read | bfs::others_exe);

    auto port_devs = netdev_manager.GetNetDevs(DeviceType::Port);
    for (size_t i = 0; i < port_devs.size(); i++) {
      NotifyNetworkChanges(EventType::USER, EventLayer::IP_CHANGE_FILES, "br"s.append(std::to_string(i)));
    }

    auto wwans = netdev_manager.GetNetDevs(DeviceType::Wwan);
    for(auto& wwan_dev : wwans){
      NotifyNetworkChanges(EventType::USER, EventLayer::IP_CHANGE_FILES, wwan_dev->GetName());
    }

  }


}

void EventManager::RegisterNetworkInformation(IBridgeInformation &bridge_information, IIPInformation &ip_information,
                                              IInterfaceInformation &interface_information) {
  bridge_information_ = &bridge_information;
  ip_information_ = &ip_information;
  interface_information_ = &interface_information;
}

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wwrite-strings"

::std::vector<char> EventManager::GetBridgeConfigAsJson() {
  BridgeConfig bridge_config;
  if (bridge_information_) {
    bridge_config = bridge_information_->GetBridgeConfig();
  }
  auto config = JsonConverter().ToJsonString(bridge_config);
  return ::std::vector<char>(config.c_str(), config.c_str() + config.length() + 1);
}

::std::vector<char> EventManager::GetIPConfigAsJson() {
  IPConfigs ip_config;
  if (ip_information_) {
    ip_config = ip_information_->GetCurrentIPConfigs();
  }
  auto config = JsonConverter().ToJsonString(ip_config);
  return ::std::vector<char>(config.c_str(), config.c_str() + config.length() + 1);
}

::std::vector<char> EventManager::GetInterfaceConfigAsJson() {
  InterfaceConfigs interface_config;
  if (interface_information_) {
    interface_config = interface_information_->GetPortConfigs();
  }
  auto config = JsonConverter().ToJsonString(interface_config);
  return ::std::vector<char>(config.c_str(), config.c_str() + config.length() + 1);
}

void EventManager::SpawnProcess() {

  auto bridge_config = GetBridgeConfigAsJson();
  auto ip_config = GetIPConfigAsJson();
  auto interface_config = GetInterfaceConfigAsJson();
  gchar *argv[] = { "/usr/bin/run-parts", "-a", "config", "/etc/config-tools/events/networking", nullptr };
  g_setenv("NETCONF_BRIDGE_CONFIG", &bridge_config[0], static_cast<gboolean>(true));
  g_setenv("NETCONF_IP_CONFIG", &ip_config[0], static_cast<gboolean>(true));
  g_setenv("NETCONF_INTERFACE_CONFIG", &interface_config[0], static_cast<gboolean>(true));
  gint exit_status = 0;
  GError *error = nullptr;
  if (g_spawn_sync(nullptr, static_cast<gchar**>(argv), nullptr, G_SPAWN_DEFAULT, nullptr, nullptr, nullptr, nullptr,
                   &exit_status, &error) != 0) {
    LogDebug("EventManager: called run-parts on /etc/config-tools/events/networking");
  } else {
    LogError("Failed to trigger /etc/config-tools/events/networking folder");
    g_error_free(error);
  }
}
#pragma GCC diagnostic pop

void EventManager::NotifyNetworkChanges(const EventType event_type, const EventLayer event_layer, Interface interface) {

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
  if (event_type == EventType::SYSTEM) {
    ProcessEvents();
  }
}

void EventManager::UpdateIpChangeFiles() {
  for (auto interface : ip_interface_update_pending_) {

    ::std::string file = IPV4_CHANGE_DIR"/ipconchg-" + interface;

    TouchFile(file);

  }
  ip_interface_update_pending_.clear();
}

void EventManager::ProcessEvents() {
  UpdateIpChangeFiles();

  if (trigger_event_folder_) {
    SpawnProcess();
  }

  trigger_event_folder_ = false;
}

}
