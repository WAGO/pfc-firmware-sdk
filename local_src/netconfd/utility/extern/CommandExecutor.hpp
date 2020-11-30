// SPDX-License-Identifier: LGPL-3.0-or-later
#pragma once

#include "ICommandExecutor.hpp"

namespace netconf {

class CommandExecutor : public ICommandExecutor {

 public:
  CommandExecutor() = default;
  virtual ~CommandExecutor() = default;

  CommandExecutor(const CommandExecutor&) = default;
  CommandExecutor& operator=(const CommandExecutor&) = default;
  CommandExecutor(CommandExecutor&&) = default;
  CommandExecutor& operator=(CommandExecutor&&) = default;

  Error Execute(const ::std::string& command) const override;
  Error Execute(const ::std::string& command, ::std::string & result) const override;

};

}  // namespace netconf

//---- End of source file ------------------------------------------------------

