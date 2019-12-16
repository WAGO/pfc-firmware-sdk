#ifndef NETCONFD_TEST_EXTERN_MOCKS_MOCKICOMMANDEXECUTOR_HPP_
#define NETCONFD_TEST_EXTERN_MOCKS_MOCKICOMMANDEXECUTOR_HPP_

#include <gmock/gmock.h>
#include "CommandExecutor.hpp"

namespace netconfd {

class MockCommandExecutor : public CommandExecutor {
 public:
  MOCK_CONST_METHOD1(Execute, Status(::std::string const&) );
  MOCK_CONST_METHOD2(Execute, Status(::std::string const&, ::std::string &) );
};

}
  // namespace netconfd

#endif 
