// SPDX-License-Identifier: GPL-2.0-or-later
#include "DHCPClientController.hpp"

#include <csignal>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <cstdlib>
#include <glib.h>

#include "Logger.hpp"
#include "Helper.hpp"

using namespace std::string_literals;

namespace netconfd {

DHCPClientController::DHCPClientController(const IDeviceProperties &properties_provider, const IFileEditor &file_editor)
    :
    properties_provider_ { properties_provider },
    file_editor_ { file_editor } {
}

Status DHCPClientController::StartClient(const Bridge &bridge) const {
  Status status;

  if (DHCPClientStatus::RUNNING == GetStatus(bridge)) {
    LogDebug("DHCP Client for bridge " + bridge + " is already started");
    return status;
  }
  LogDebug("Start DHCP Client for bridge " + bridge);

  ::std::string hostname = properties_provider_.GetHostname();
  ::std::string pid_file_path = "/var/run/udhcpc_" + bridge + ".pid";

  auto argv_array = make_array(DHCP_CLIENT_PATH.c_str(), "--interface", bridge.c_str(),
                               "--vendorclass", properties_provider_.GetOrderNumber().c_str(), "--pidfile",
                               pid_file_path.c_str(), "-x", "hostname:", hostname.c_str(), "--syslog", nullptr);

  GPid pid;
  GError *error;
  auto spawned = g_spawn_async(nullptr, const_cast<gchar**>(argv_array.data()), nullptr, G_SPAWN_DEFAULT, nullptr, // NOLINT(cppcoreguidelines-pro-type-const-cast)
                               nullptr, &pid, &error);

  if (spawned == TRUE) {
    LogDebug("DHCPClientController: spawned udhcp pid#" + ::std::to_string(pid));
  } else {
    status.Prepend(StatusCode::ERROR, "Failed to start DHCP client create child process error");
  }

  return status;
}

Status DHCPClientController::StopClient(const Bridge &bridge) const {

  Status status(StatusCode::OK);

  if (DHCPClientStatus::STOPPED == GetStatus(bridge)) {
    return status;
  }

  ::std::string pid_file_path = "/var/run/udhcpc_" + bridge + ".pid";

  ::std::string pid_str;
  status = file_editor_.Read(pid_file_path, pid_str);
  if (status.NotOk()) {
    status.Prepend("Failed to stop DHCP client. ");
  }

  if (status.Ok()) {
    pid_t pid = std::stoi(pid_str);

    if (0 == kill(pid, SIGKILL)) {
      LogDebug("Stopped DHCP Client for bridge " + bridge);

      // At this point, we know that the pidfile exists.
      bool remove_pid_file_successfully = (std::remove(pid_file_path.c_str()) == 0);
      if (not remove_pid_file_successfully) {
        status.Append("Failed to remove pid file: " + pid_file_path);
      }
    } else {
      status.Prepend(StatusCode::ERROR, "Failed to kill DHCP client. ");
    }
  }

  return status;
}

DHCPClientStatus DHCPClientController::GetStatus(const Bridge &bridge) const {

  ::std::string pid_file_path = "/var/run/udhcpc_" + bridge + ".pid";

  ::std::string pid_str;
  Status status = file_editor_.Read(pid_file_path, pid_str);

  pid_t pid = 0;
  if (status.Ok()) {
    try {
      pid = std::stoi(pid_str);
      if (pid == 0) {
        status.Append(StatusCode::ERROR, "Failed to get DHCP status.");
      }
    } catch (...) {
      status.Append(StatusCode::ERROR, "Failed to get DHCP status (unexpected exception).");
    }
  }

  if (status.Ok()) {
    int state = kill(pid, 0);
    if (state == 0 || (state == -1 && errno == EPERM)) {
      return DHCPClientStatus::RUNNING;
    }
  }

  return DHCPClientStatus::STOPPED;

}

} /* namespace netconfd */
