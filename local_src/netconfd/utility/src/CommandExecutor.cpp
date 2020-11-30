// SPDX-License-Identifier: LGPL-3.0-or-later

#include "CommandExecutor.hpp"
#include <cstdio>
#include <array>
#include <unistd.h>
#include <string>

#include <boost/process/child.hpp>
#include <boost/process/exception.hpp>

using namespace std::string_literals;

namespace netconf {

Error CommandExecutor::Execute(const ::std::string& command) const {
  ::std::string tmp;
  return Execute(command, tmp);
}

Error CommandExecutor::Execute(const ::std::string& command,
                                ::std::string & result) const {
  FILE* file;
  file = popen(command.c_str(), "r");  //NOLINT(cert-env33-c) TODO(PND): kann evtl. anders gelöst werden.

  if (file == nullptr) {
    return Error{ErrorCode::SYSTEM_EXECUTE, ::std::to_string(errno)};
  }

  std::array<char, 100> buffer { };

  while (fgets(buffer.data(), buffer.size(), file) != nullptr) {
    result += ::std::string(buffer.data());
  }

  int st = pclose(file);
  if (WIFEXITED(st)) {
    if (0 != WEXITSTATUS(st)) {
      return Error{ErrorCode::SYSTEM_EXECUTE, ::std::to_string(WEXITSTATUS(st))};
    }
  }

  return Error::Ok();
}

}  // namespace netconf
//---- End of source file ------------------------------------------------------

