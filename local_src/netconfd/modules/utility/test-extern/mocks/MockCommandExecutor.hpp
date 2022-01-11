// SPDX-License-Identifier: LGPL-3.0-or-later

#pragma once

#include <gmock/gmock.h>
#include "CommandExecutor.hpp"

namespace netconf {

class MockCommandExecutor : public CommandExecutor {
 public:
  MOCK_CONST_METHOD1(Execute, Status(::std::string const&) );
  MOCK_CONST_METHOD2(Execute, Status(::std::string const&, ::std::string &) );
};

} // namespace netconf
