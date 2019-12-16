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

namespace netconfd {


Status CommandExecutor::Execute(const ::std::string& command) const {
  ::std::string tmp;
  return Execute(command, tmp);
}


Status CommandExecutor::Execute(const ::std::string& command, ::std::string & result) const {
  FILE* file;
  file = popen(command.c_str(), "r"); //NOLINT(cert-env33-c) TODO(PND): kann evtl. anders gelöst werden.

  if (file == nullptr) {
    return Status(StatusCode::ERROR);
  }

  std::array<char, 100> buffer{};

  while(fgets(buffer.data(), buffer.size(), file) != nullptr) {
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


}  // namespace netconfd

//---- End of source file ------------------------------------------------------

