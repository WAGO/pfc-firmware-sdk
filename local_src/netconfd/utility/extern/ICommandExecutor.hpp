// SPDX-License-Identifier: GPL-2.0-or-later
//------------------------------------------------------------------------------
///  \file     ICommandExecutor.hpp
///
///  \brief    <short description of the file contents>
///
///  \author   <author> : WAGO Kontakttechnik GmbH & Co. KG
//------------------------------------------------------------------------------
#ifndef NETCONFD_EXTERN_IEXECUTECOMMAND_HPP_
#define NETCONFD_EXTERN_IEXECUTECOMMAND_HPP_

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
  virtual Status ForkExecute(const ::std::string& command, const uint32_t timeout_ms) const = 0;

};

}  // namespace netconf

#endif
//---- End of source file ------------------------------------------------------

