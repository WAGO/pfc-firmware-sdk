// SPDX-License-Identifier: GPL-2.0-or-later
#pragma once

#include "string"

#include "Error.hpp"
#include "InterprocessCondition.h"

namespace netconf {

class Daemonizer {
 public:
  Daemonizer(::std::string const& run_directory,
             ::std::string const& pid_file_name);
  virtual ~Daemonizer() = default;

  Error Daemonize(InterprocessCondition& condition);
  Error PreparePidDir() const;
  bool IsPidFileLocked() const;
  Error WritePidFile();
  void SetUnlinkPidOnExit();

 private:
  Error OpenAndLockPidFile();
  Error SetCloseOnExecFlag() const;

  ::std::string const& run_directory_;
  ::std::string const& pid_file_name_;
  int pid_file_handle_;

};

} // namespace netconf
