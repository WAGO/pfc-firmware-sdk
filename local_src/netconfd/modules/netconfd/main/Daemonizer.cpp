// SPDX-License-Identifier: GPL-2.0-or-later
//------------------------------------------------------------------------------
///  \file     Daemonizer.cpp
///
///  \brief    Create daemon and corresponding pid-file.
///
///  \author   WAGO Kontakttechnik GmbH & Co. KG
//------------------------------------------------------------------------------
#include "Daemonizer.hpp"

#include <boost/format.hpp>
#include <fcntl.h>
#include <unistd.h>
#include <cstdlib>
#include <csignal>
#include <sys/stat.h>
#include <cstring>

#include "Logger.hpp"
#include "CommandExecutor.hpp"
#include "Status.hpp"

#include <chrono>

namespace netconf {

using namespace ::std::chrono_literals;

Daemonizer::Daemonizer(::std::string const &run_directory, ::std::string const &pid_file_name)
    : run_directory_ { run_directory },
      pid_file_name_ { pid_file_name },
      pid_file_handle_ { -1 } {
}

static bool ExistsFile(char const *const szName) {
  struct stat buffer { };
  return (stat(szName, &buffer) == 0);
}

static void ExitDaemonCallback(int stat, void *arg) {
  (void) stat;
  unlink(static_cast<char const*>(arg));
}

// Lock file region using nonblocking F_SETLK
int static lockRegion(int fd, int16_t type, int16_t whence, int start, int16_t len) {
  struct flock lock = { };

  lock.l_type = type;
  lock.l_whence = whence;
  lock.l_start = start;
  lock.l_len = len;

  return fcntl(fd, F_SETLK, &lock);
}

Status Daemonizer::Daemonize(InterprocessCondition &condition) {

  Status status;
  pid_t pid;

  pid = fork();
  // Exit parent process.
  if (pid > 0) {
    // Wait for the start condition before exit
    condition.Wait(10s);
    LogInfo("Daemonizer: initial process exits");
    exit(EXIT_SUCCESS);
  } else if (pid < 0) {
    return MakeSystemCallError();
  }

  // Become session leader
  if (setsid() < 0) {
    return MakeSystemCallError();
  }

  signal(SIGHUP, SIG_IGN);  // NOLINT old style cast in system definition

  pid = fork();
  if (pid < 0) {
    return MakeSystemCallError();
  }

  // Exit first child process.
  if (pid != 0) {
    exit(EXIT_SUCCESS);
  }

  // Set working directory.
  umask(0);
  if (chdir("/") < 0) {
    return MakeSystemCallError();
  }

  // Close file descriptors
  for (int i = 0; i < sysconf(_SC_OPEN_MAX); ++i) {
    close(i);
  }

  // attach /dev/null to stin, stdout and stderr
  auto fd0 = open("/dev/null", O_RDWR);
  auto fd1 = dup(fd0);
  auto fd2 = dup(fd0);
  if (fd0 != 0 || fd1 != 1 || fd2 != 2) {
    return MakeSystemCallError();
  }
  return Status::Ok();
}

Status Daemonizer::PreparePidDir() const {
  Status status;

  ::std::string pidFilePath = run_directory_ + "/" + pid_file_name_;

  if (pidFilePath.length() > 256) {
    status.Set(StatusCode::GENERIC_ERROR, "Pid file path too long! Max 256!");
  }

  if (status.IsOk()) {
    // Setup dir
    if (!ExistsFile(run_directory_.c_str())) {
      if (0 != mkdir(run_directory_.c_str(), 0644)) {
        status = MakeSystemCallError();
      }
    }
  }

  return status;
}

bool Daemonizer::IsPidFileLocked() const {
  bool isLocked = false;
  ::std::string pidFilePath = run_directory_ + "/" + pid_file_name_;

  int pidFd = open(static_cast<const char*>(pidFilePath.c_str()),
  O_RDWR | O_CREAT,
                   0644);
  if (pidFd < 0) {
    return isLocked;
  }

  if (lockRegion(pidFd, F_WRLCK, SEEK_SET, 0, 0) == -1) {
    if (errno == EAGAIN || errno == EACCES) {
      isLocked = true;
    }
  }

  if (pidFd >= 0) {
    close(pidFd);
  }

  return isLocked;
}

Status Daemonizer::OpenAndLockPidFile() {
  ::std::string pidFilePath = run_directory_ + "/" + pid_file_name_;

  pid_file_handle_ = open(static_cast<const char*>(pidFilePath.c_str()),  O_RDWR | O_CREAT, 0644);
  if (pid_file_handle_ < 0) {
    return MakeSystemCallError();
  }

  if (lockRegion(pid_file_handle_, F_WRLCK, SEEK_SET, 0, 0) == -1) {
    if (errno == EAGAIN || errno == EACCES) {
      return MakeSystemCallError();
    }
  }
  return Status::Ok();
}

Status Daemonizer::WritePidFile() {

  ::std::string pidFilePath = run_directory_ + "/" + pid_file_name_;

  Status status = OpenAndLockPidFile();

  if (status.IsOk()) {
    if (ftruncate(pid_file_handle_, 0) == -1) {
      status = MakeSystemCallError();
    }
  }

  if (status.IsOk()) {
    int const pid = getpid();
    char szPid[sizeof("32768\n")];  // 32768 is max pid for 32 bit systems

    int const pidLength = sprintf(static_cast<char*>(szPid), "%d\n", pid);
    if (pidLength > 0) {
      ssize_t bytesWritten = write(pid_file_handle_, static_cast<char*>(szPid), static_cast<size_t>(pidLength));
      if (bytesWritten <= 0) {
        status = MakeSystemCallError();
      }
    }
  }

  if (status.IsOk()) {
    status = SetCloseOnExecFlag();
  }

  return status;
}

Status Daemonizer::SetCloseOnExecFlag() const {
  // Set close on exec file descriptor property.
  Status status;

  int flags = fcntl(pid_file_handle_, F_GETFD);
  if (flags == -1) {
    status = MakeSystemCallError();
  }

  if (status.IsOk()) {
    flags |= FD_CLOEXEC;

    if (fcntl(pid_file_handle_, F_SETFD, flags) == -1) {
      status = MakeSystemCallError();
    }
  }

  return status;
}

void Daemonizer::SetUnlinkPidOnExit() {
  ::std::string pidFilePath = run_directory_ + "/" + pid_file_name_;
  static ::std::array<char, 256 + 1> buffer;
  strncpy(buffer.data(), pidFilePath.c_str(), pidFilePath.length());

  on_exit(ExitDaemonCallback, buffer.data());
}

} /* namespace netconf */
