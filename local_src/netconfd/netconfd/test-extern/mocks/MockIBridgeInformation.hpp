#ifndef NETCONFD_TEST_EXTERN_MOCKS_MOCKIBRIDGEINFORMATION_HPP_
#define NETCONFD_TEST_EXTERN_MOCKS_MOCKIBRIDGEINFORMATION_HPP_

#include <gmock/gmock.h>

#include "../../src/InterfaceManager/IBridgeInformation.hpp"

namespace netconf {

class MockIInterfaceManagerInformation : public IBridgeInformation {
 public:

  MOCK_CONST_METHOD0(GetBridgeAssignedInterfaces, Interfaces() );
  MOCK_CONST_METHOD0(GetBridgeConfig, BridgeConfig () );
};

}
  // namespace netconf
#endif
