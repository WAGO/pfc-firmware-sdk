#ifndef NETCONFD_TEST_EXTERN_MOCKS_MOCKIINTERFACEINFORMATION_HPP_
#define NETCONFD_TEST_EXTERN_MOCKS_MOCKIINTERFACEINFORMATION_HPP_

#include <gmock/gmock.h>

#include "IInterfaceInformation.hpp"

namespace netconfd {

class MockIInterfaceManagerInformation : public IInterfaceInformation {
 public:

  MOCK_CONST_METHOD0(GetBridgeAssignedInterfaces, Interfaces() );
  MOCK_CONST_METHOD1(GetBridgeOfInterface, Bridge (const Interface&) );
  MOCK_CONST_METHOD1(IsInterfaceUp, bool(const Interface&) );
};

}
  // namespace netconfd
#endif
