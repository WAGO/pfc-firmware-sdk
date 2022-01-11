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

Status DHCPClientController::StartClient(const Bridge &bridge) const {
  if (DHCPClientStatus::RUNNING == GetStatus(bridge)) {
    LogDebug("DHCP Client for bridge " + bridge + " is already started");
    return Status::Ok();
  }
  LogDebug("Start DHCP Client for bridge " + bridge);

  ::std::string hostname_option = "hostname:" + properties_provider_.GetHostname();
  ::std::string pid_file_path   = "/var/run/udhcpc_" + bridge + ".pid";
  auto vendorclass              = properties_provider_.GetOrderNumber();

  auto argv_array =
      make_array(DHCP_CLIENT_PATH.c_str(), "--foreground", "--interface", bridge.c_str(), "--vendorclass", vendorclass.c_str(),
                 "--pidfile", pid_file_path.c_str(), "-x", hostname_option.c_str(), "--syslog", nullptr);

  GPid pid;
  GError *g_error;
  auto spawned = g_spawn_async(nullptr, const_cast<gchar **>(argv_array.data()), nullptr, G_SPAWN_DEFAULT,
                               nullptr,  // NOLINT(cppcoreguidelines-pro-type-const-cast)
                               nullptr, &pid, &g_error);

  if (spawned == TRUE) {
    LogDebug("DHCPClientController: spawned udhcp pid#" + ::std::to_string(pid));
  } else {
    return Status{StatusCode::DHCP_CLIENT_START, bridge};
  }

  return Status::Ok();
}

pid_t DHCPClientController::ReadPidFromFile(const Bridge& bridge) const {
  ::std::string pid_file_path = "/var/run/udhcpc_" + bridge + ".pid";
  ::std::string pid_str;
  pid_t pid = 0;

  auto status = file_editor_.Read(pid_file_path, pid_str);
  if (status.IsOk()) {
    try {
      pid = std::stoi(pid_str);
    } catch (...) {
    }
  }
  return pid;
}

void DHCPClientController::StopClient(const Bridge &bridge) const {

  pid_t pid = ReadPidFromFile(bridge);

  if (DHCPClientStatus::STOPPED == GetStatus(pid)) {
    LogDebug("DHCP Client for " + bridge + ": Process with pid " + ::std::to_string(pid) + " is already stopped.");
    return;
  }

  LogDebug("DHCP Client for " + bridge + ": Send SIGKILL to process with pid " + ::std::to_string(pid));
  if (0 == kill(pid, SIGKILL)) {
    LogDebug("Stopped DHCP Client for bridge " + bridge);
  }

  WaitUntilClientIsStopped(bridge, pid);

}

void DHCPClientController::WaitUntilClientIsStopped(const Bridge &bridge, const pid_t pid) const {
  int max_retries = 100;
  int retries     = 0;
  auto status     = GetStatus(pid);
  while (status == DHCPClientStatus::RUNNING && retries++ < max_retries) {
    ::std::this_thread::sleep_for(::std::chrono::milliseconds{100});
    status = GetStatus(pid);
  }
  if (status == DHCPClientStatus::RUNNING) {
    LogError("DHCP Client for " + bridge + ":Failed to terminate process: " + ::std::to_string(pid));
    if(retries >= max_retries) {
      LogError("DHCP Client stop: Exceeded maximum number of retries.");
    }
  }
}

static DHCPClientStatus DetermineProcessStatus(pid_t pid) {
  int state = kill(pid, 0);
  if (state == 0) {
    return DHCPClientStatus::RUNNING;
  } else if(state == -1) {
    if(errno != ESRCH) {
      LogError("Failed to determine process state for pid " + ::std::to_string(pid) + "; errno: " +
               ::std::to_string(errno));
    }
    if(errno == EPERM){
      return DHCPClientStatus::RUNNING;
    }
  }
  return DHCPClientStatus::STOPPED;
}

DHCPClientStatus DHCPClientController::GetStatus(const Bridge &bridge) const {
  auto pid = ReadPidFromFile(bridge);
  return GetStatus(pid);
}

DHCPClientStatus DHCPClientController::GetStatus(const pid_t pid) const {
  DHCPClientStatus client_status = DHCPClientStatus::STOPPED;

  if (pid != 0) {
    client_status = DetermineProcessStatus(pid);
  }
  return client_status;
}

} /* namespace netconf */
