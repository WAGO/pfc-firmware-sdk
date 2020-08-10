// SPDX-License-Identifier: GPL-2.0-or-later
//------------------------------------------------------------------------------
///  \file     CommandExecutor.cpp
///
///  \brief    <short description of the file contents>
///
///  \author   <author> : WAGO Kontakttechnik GmbH & Co. KG
//------------------------------------------------------------------------------

#include "CommandExecutor.hpp"
#include <cstdio>
#include <array>
#include <unistd.h>
#include <string>

#include <boost/process/child.hpp>
#include <boost/process/exception.hpp>

using namespace std::string_literals;

namespace netconf {

Status CommandExecutor::Execute(const ::std::string& command) const {
  ::std::string tmp;
  return Execute(command, tmp);
}

Status CommandExecutor::Execute(const ::std::string& command,
                                ::std::string & result) const {
  FILE* file;
  file = popen(command.c_str(), "r");  //NOLINT(cert-env33-c) TODO(PND): kann evtl. anders gelöst werden.

  if (file == nullptr) {
    return Status(StatusCode::ERROR);
  }

  std::array<char, 100> buffer { };

  while (fgets(buffer.data(), buffer.size(), file) != nullptr) {
    result += ::std::string(buffer.data());
  }

  int st = pclose(file);
  if (WIFEXITED(st)) {
    if (0 != WEXITSTATUS(st)) {
      return Status(StatusCode::ERROR);
    }
  }

  return Status();
}

Status CommandExecutor::ForkExecute(const ::std::string& command,
                                    const uint32_t timeout_ms) const {

  std::chrono::milliseconds duration(timeout_ms);
  boost::process::child child(command);
  std::error_code error;

  if (!child.wait_for(duration, error)) {
    return Status(
        StatusCode::ERROR, "Reached timeout: "
            "Execute command '"s + command + "' failed.");
  }

  if (child.exit_code() == 0) {
    return Status();
  }

  return Status(
      StatusCode::ERROR,
      "Execute command '"s + command + "' failed with exit code "
          + ::std::to_string(child.exit_code()) + "; " + error.message());
}

}  // namespace netconf
//---- End of source file ------------------------------------------------------

