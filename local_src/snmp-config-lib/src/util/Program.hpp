#pragma once

#include <string>
#include <unistd.h>

namespace wago::util {

class Program {
 public:
  Program() = default;
  ~Program() = default;
  Program(const Program&) = delete;
  Program(Program &&other) noexcept;
  Program& operator=(const Program&) = delete;
  Program& operator=(Program &&other) noexcept;

  ::std::string GetStdout() const;
  ::std::string GetStderr() const;
  int GetResult() const;

  static Program Execute(std::string &&cmdline);
  static Program Execute(std::string &cmdline);

 private:
  int result_ = -1;
  ::std::string stdout_;
  ::std::string stderr_;
};

}  // namespace wago::util
