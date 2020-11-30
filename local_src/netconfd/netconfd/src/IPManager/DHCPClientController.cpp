// SPDX-License-Identifier: GPL-2.0-or-later
#include "DHCPClientController.hpp"

#include <glib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include <csignal>
#include <cstdlib>
#include <thread>

#include "Helper.hpp"
#include "Logger.hpp"

using namespace std::string_literals;

namespace netconf {

DHCPClientController::DHCPClientController(const IDeviceProperties &properties_provider, const IFileEditor &file_editor)
    : properties_provider_{properties_provider}, file_editor_{file_editor} {
}

Error DHCPClientController::StartClient(const Bridge &bridge) const {
  if (DHCPClientStatus::RUNNING == GetStatus(bridge)) {
    LogDebug("DHCP Client for bridge " + bridge + " is already started");
    return Error::Ok();
  }
  LogDebug("Start DHCP Client for bridge " + bridge);

  ::std::string hostname_option = "hostname:" + properties_provider_.GetHostname();
  ::std::string pid_file_path   = "/var/run/udhcpc_" + bridge + ".pid";
  auto vendorclass              = properties_provider_.GetOrderNumber();

  auto argv_array =
      make_array(DHCP_CLIENT_PATH.c_str(), "--interface", bridge.c_str(), "--vendorclass", vendorclass.c_str(),
                 "--pidfile", pid_file_path.c_str(), "-x", hostname_option.c_str(), "--syslog", nullptr);

  GPid pid;
  GError *g_error;
  auto spawned = g_spawn_async(nullptr, const_cast<gchar **>(argv_array.data()), nullptr, G_SPAWN_DEFAULT,
                               nullptr,  // NOLINT(cppcoreguidelines-pro-type-const-cast)
                               nullptr, &pid, &g_error);

  if (spawned == TRUE) {
    LogDebug("DHCPClientController: spawned udhcp pid#" + ::std::to_string(pid));
  } else {
    return Error{ErrorCode::DHCP_CLIENT_START, bridge};
  }

  return Error::Ok();
}

void DHCPClientController::StopClient(const Bridge &bridge) const {
  if (DHCPClientStatus::STOPPED == GetStatus(bridge)) {
    return;
  }

  ::std::string pid_file_path = "/var/run/udhcpc_" + bridge + ".pid";

  ::std::string pid_str;
  auto error = file_editor_.Read(pid_file_path, pid_str);
  if (error.IsOk()) {
    pid_t pid = std::stoi(pid_str);

    if (0 == kill(pid, SIGKILL)) {
      LogDebug("Stopped DHCP Client for bridge " + bridge);
    }

    WaitUntilClientIsStopped(bridge);
  }
}

void DHCPClientController::WaitUntilClientIsStopped(const Bridge &bridge) const {
  int max_retries = 100;
  int retries     = 0;
  auto status     = GetStatus(bridge);
  while (status == DHCPClientStatus::RUNNING && retries++ < max_retries) {
    ::std::this_thread::sleep_for(::std::chrono::milliseconds{100});
    status = GetStatus(bridge);
  }
  if (status == DHCPClientStatus::RUNNING) {
    LogError("Failed to stop dhcp client for " + bridge);
  }
}

static DHCPClientStatus DetermineProcessStatus(pid_t pid) {
  int state = kill(pid, 0);
  if (state == 0 || (state == -1 && errno == EPERM)) {
    return DHCPClientStatus::RUNNING;
  }
  return DHCPClientStatus::STOPPED;
}

DHCPClientStatus DHCPClientController::GetStatus(const Bridge &bridge) const {
  ::std::string pid_file_path = "/var/run/udhcpc_" + bridge + ".pid";

  ::std::string pid_str;
  Error status = file_editor_.Read(pid_file_path, pid_str);
  LogStatus("read pid file", status);

  DHCPClientStatus client_status = DHCPClientStatus::STOPPED;
  if (status.IsOk()) {
    try {
      pid_t pid = std::stoi(pid_str);
      if (pid != 0) {
        client_status = DetermineProcessStatus(pid);
      }
    } catch (...) {
    }
  }

  return client_status;
}

} /* namespace netconf */
