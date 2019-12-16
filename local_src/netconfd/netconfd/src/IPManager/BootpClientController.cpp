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

#include "Logger.hpp"

//using namespace boost::filesystem;
using namespace std::string_literals;
using boost::filesystem::path;

namespace netconfd {

static pid_t GetPid(const ::std::string& process_name_pattern) {

  if (boost::filesystem::is_directory("/proc")) {

    path p1 = boost::filesystem::current_path();

    for (auto& entry : boost::make_iterator_range(
        boost::filesystem::directory_iterator("/proc"))) {

      path cmdline_path("/proc");
      cmdline_path += entry;
      cmdline_path /= "cmdline";

      if (boost::filesystem::is_regular_file(cmdline_path)) {
        boost::filesystem::ifstream stream { cmdline_path };
        std::string file_content;
        stream >> file_content;
        stream.close();

        ::std::size_t found = file_content.find(process_name_pattern);
        if (::std::string::npos != found) {

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

Status BootpClientController::StartClient(const Bridge& bridge) const {
  Status status;

  if (BootpClientStatus::RUNNING == GetStatus(bridge)) {
    LogDebug("Bootp client for bridge " + bridge + " is already running");
    return status;
  }
  LogDebug("Run Bootp client for bridge " + bridge);

  auto pid = fork();
  if (0 == pid) {
    // child process B
    pid = fork();
    if (pid > 0) {
      exit(0);
    } else if (0 == pid) {
      // child process C

      auto exec_status = execl(BOOTP_SCRIPT_PATH.c_str(), BOOTP_SCRIPT_PATH.c_str(), bridge.c_str(),
            nullptr);

      if (exec_status != 0) {
        LogError("Failed to execute bootp client");
        abort();
      }
    } else {
      LogError("Failed to start Bootp client for bridge " + bridge
              + ", create child process error.");
      abort();
    }
  } else if (pid < 0) {
    status.Prepend(
        StatusCode::ERROR,
        "Failed start Bootp client for bridge " + bridge + "create child process error.");
  }

  return status;
}

Status BootpClientController::StopClient(const Bridge& bridge) const {

  Status status;

  ::std::string pattern = "bootpc-startup\0"s + bridge;
  pid_t pid = GetPid(pattern);

  if (pid > 0) {
    if (0 == kill(pid, SIGKILL)) {
      LogDebug("Stopped Bootp Client for bridge " + bridge);
    } else {
      status.Prepend(StatusCode::ERROR,
                       "Failed to stop Bootp client for bridge " + bridge);
    }
  }

  return status;
}

BootpClientStatus BootpClientController::GetStatus(const Bridge& bridge) const {

  ::std::string pattern = "bootpc-startup\0"s + bridge;
  pid_t pid = GetPid(pattern);

  if (pid > 0) {
    return BootpClientStatus::RUNNING;
  }

  return BootpClientStatus::STOPPED;
}

} /* namespace netconfd */
