// SPDX-License-Identifier: GPL-2.0-or-later
//------------------------------------------------------------------------------
///  \file     CommandExecutor.hpp
///
///  \brief    <short description of the file contents>
///
///  \author   <author> : WAGO Kontakttechnik GmbH & Co. KG
//------------------------------------------------------------------------------
#ifndef NETCONFD_EXTERN_EXECUTECOMMAND_HPP_
#define NETCONFD_EXTERN_EXECUTECOMMAND_HPP_

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

  Status Execute(const ::std::string& command) const override;
  Status Execute(const ::std::string& command, ::std::string & result) const override;
  Status ForkExecute(const ::std::string& command, const uint32_t timeout_ms) const override;

};

}  // namespace netconf

#endif
//---- End of source file ------------------------------------------------------

