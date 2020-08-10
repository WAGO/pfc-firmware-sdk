// SPDX-License-Identifier: GPL-2.0-or-later
//------------------------------------------------------------------------------
///  \file     BootpController.cpp
///
///  \brief    <short description of the file contents>
///
///  \author   <author> : WAGO Kontakttechnik GmbH & Co. KG
//------------------------------------------------------------------------------
#include "BootpClientController.hpp"

#include <boost/filesystem.hpp>
#include <boost/range/iterator_range.hpp>
#include <iostream>
#include <unistd.h>
#include <sys/wait.h>
#include <glib.h>

#include "Logger.hpp"
#include "Helper.hpp"

//using namespace boost::filesystem;
using namespace std::string_literals;
using boost::filesystem::path;

namespace netconf {

static ::std::string GetFileContent(const path& path) noexcept {
  try {
    boost::filesystem::ifstream stream { path };
    std::string file_content;
    stream >> file_content;
    stream.close();
    return file_content;
  } catch (...) {
    return ::std::string { };
  }
}

static bool IsRegularFile(const path& path) {
  boost::system::error_code ec;
  return boost::filesystem::is_regular_file(path, ec) && not ec;
}

static pid_t GetPid(const ::std::string &process_name_pattern) {
  if (boost::filesystem::is_directory("/proc")) {
    for (auto &entry : boost::make_iterator_range(boost::filesystem::directory_iterator("/proc"))) {

      path cmdline_path("/proc");
      cmdline_path += entry;
      cmdline_path /= "cmdline";

      boost::system::error_code ec;
      if (IsRegularFile(cmdline_path)) {
        auto file_content = GetFileContent(cmdline_path);

        auto found = file_content.find(process_name_pattern) != ::std::string::npos;
        if (found) {
          ::std::string pid_string = entry.path().filename().string();
          try {
            pid_t pid = ::std::stoi(pid_string);
            return pid;
          } catch (...) {
            return 0;
          }
        }
      }
    }
  }

  return 0;
}

Status BootpClientController::StartClient(const Bridge &bridge) const {
  Status status;

  if (BootpClientStatus::RUNNING == GetStatus(bridge)) {
    LogDebug("Bootp client for bridge " + bridge + " is already running");
    return status;
  }
  LogDebug("Run Bootp client for bridge " + bridge);

  auto argv_array = make_array(BOOTP_SCRIPT_PATH.c_str(), bridge.c_str());
  GPid pid;
  GError *error;
  auto spawned = g_spawn_async(nullptr, const_cast<gchar**>(argv_array.data()), nullptr, G_SPAWN_DEFAULT, nullptr,  // NOLINT(cppcoreguidelines-pro-type-const-cast)
                               nullptr, &pid, &error);

  if (spawned == TRUE) {
    LogDebug("BootpClientController: spawned bootpc pid#" + ::std::to_string(pid));
  } else {
    status.Prepend(StatusCode::ERROR, "Failed to start bootp client create child process error: "s + error->message);
  }

  return status;
}

Status BootpClientController::StopClient(const Bridge &bridge) const {

  Status status;

  ::std::string pattern = "bootpc-startup\0"s + bridge;
  pid_t pid = GetPid(pattern);

  if (pid > 0) {
    if (0 == kill(pid, SIGKILL)) {
      LogDebug("Stopped Bootp Client for bridge " + bridge);
    } else {
      status.Prepend(StatusCode::ERROR, "Failed to stop Bootp client for bridge " + bridge);
    }
  }

  return status;
}

BootpClientStatus BootpClientController::GetStatus(const Bridge &bridge) const {

  ::std::string pattern = "bootpc-startup\0"s + bridge;
  pid_t pid = GetPid(pattern);

  if (pid > 0) {
    return BootpClientStatus::RUNNING;
  }

  return BootpClientStatus::STOPPED;
}

} /* namespace netconf */
