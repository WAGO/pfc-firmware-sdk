//------------------------------------------------------------------------------
// Copyright (c) WAGO GmbH & Co. KG
//
// PROPRIETARY RIGHTS are involved in the subject matter of this material. All
// manufacturing, reproduction, use and sales rights pertaining to this
// subject matter are governed by the license agreement. The recipient of this
// software implicitly accepts the terms of the license.
//------------------------------------------------------------------------------

#include "Program.hpp"

#include <glib.h>
#include <poll.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>

namespace wago::util {

Program::Program(Program &&other) noexcept {
  this->operator =(::std::move(other));
}

Program& Program::operator=(Program &&other) noexcept {
  result_ = other.result_;

  other.result_ = -1;

  return *this;
}

Program Program::Execute(std::string &&cmdline) {
  return Execute(cmdline);
}

Program Program::Execute(std::string &cmdline) {
  gchar *stdout;
  gchar *stderr;
  GError *g_error;
  Program p;

  if (g_spawn_command_line_sync(cmdline.c_str(), &stdout, &stderr, &p.result_, &g_error) == 0) {
    g_error_free(g_error);
  } else {
    p.stdout_ = stdout;
    p.stderr_ = stderr;
    g_free(stdout);
    g_free(stderr);
  }

  return p;
}

::std::string Program::GetStdout() const {
  return stdout_;
}

::std::string Program::GetStderr() const {
  return stderr_;
}

int Program::GetResult() const {
  return result_;
}

}
