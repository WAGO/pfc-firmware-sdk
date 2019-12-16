// SPDX-License-Identifier: GPL-2.0-or-later

#include "EventManager.hpp"

#include <fcntl.h>
#include <unistd.h>
#include <boost/filesystem.hpp>

#include "CommandExecutor.hpp"

namespace netconfd {

namespace bfs = boost::filesystem;
using namespace std::literals;
#define IPV4_CHANGE_DIR "/var/run/ipv4"
#define MODEM_DEVICE "wwan0"

EventManager::EventManager(
    IDevicePropertiesProvider& device_properties_provider) {

  bool dir_exists = bfs::exists(IPV4_CHANGE_DIR) || bfs::create_directory(IPV4_CHANGE_DIR);
  if (dir_exists) {
    bfs::permissions(
        IPV4_CHANGE_DIR,
        bfs::owner_all | bfs::group_read | bfs::group_exe | bfs::others_read
            | bfs::others_exe);

    // Get count of ethX interfaces.
    const size_t bridgeCountMax = device_properties_provider
        .GetProductInterfaces().size();
    for (size_t i = 0; i < bridgeCountMax; i++) {
      NotifyIpChange("br"s.append(std::to_string(i)));
    }
  }

  if (device_properties_provider.HasInterface(MODEM_DEVICE)) {
    NotifyIpChange(MODEM_DEVICE);
  }
}

void EventManager::NotifyNetworkChanges() {
  CommandExecutor executor;
  executor.Execute(
      "/usr/bin/run-parts -a config /etc/config-tools/events/networking");
}

void EventManager::NotifyIpChange(const Interface & interface) {

  if (!interface.empty()) {
    ::std::string file = IPV4_CHANGE_DIR"/ipconchg-" + interface;

    int fd = open(file.c_str(), O_CREAT | O_WRONLY,
    S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);

    if (fd >= 0) {
      write(fd, "\n", 1);
      close(fd);
    }
  }

}

} /* namespace netconfd */
