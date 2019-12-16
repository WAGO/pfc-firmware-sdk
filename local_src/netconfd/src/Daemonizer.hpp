// SPDX-License-Identifier: GPL-2.0-or-later
//------------------------------------------------------------------------------
///  \file     Daemonizer.hpp
///
///  \brief    <short description of the file contents>
///
///  \author   <author> : WAGO Kontakttechnik GmbH & Co. KG
//------------------------------------------------------------------------------
#ifndef NETCONFD_DAEMONIZER_HPP_
#define NETCONFD_DAEMONIZER_HPP_

#include "string"
#include "../extern/Status.hpp"

namespace netconfd {

class Daemonizer {
 public:
  Daemonizer(::std::string const& run_directory,
             ::std::string const& pid_file_name);
  virtual ~Daemonizer() = default;

  Status Daemonize();
  Status PreparePidDir() const;
  bool IsPidFileLocked() const;
  Status WritePidFile();
  void SetUnlinkPidOnExit();

 private:
  Status OpenAndLockPidFile();
  Status SetCloseOnExecFlag() const;

  ::std::string const& run_directory_;
  ::std::string const& pid_file_name_;
  int pid_file_handle_;

};

} /* namespace netconfd */

#endif /* NETCONFD_DAEMONIZER_HPP_ */
