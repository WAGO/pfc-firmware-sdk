// SPDX-License-Identifier: LGPL-3.0-or-later
#pragma once

#include <cstdio>
#include <string>

#include "Status.hpp"

namespace netconf {

class ICommandExecutor {

 public:
  ICommandExecutor() = default;
  virtual ~ICommandExecutor() = default;

  ICommandExecutor(const ICommandExecutor&) = default;
  ICommandExecutor& operator=(const ICommandExecutor&) = default;
  ICommandExecutor(ICommandExecutor&&) = default;
  ICommandExecutor& operator=(ICommandExecutor&&) = default;

  virtual Status Execute(const ::std::string& command) const = 0;
  virtual Status Execute(const ::std::string& command, ::std::string & result) const = 0;

};

}  // namespace netconf

