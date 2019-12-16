#ifndef NETCONFD_TEST_EXTERN_MOCKS_MOCKIEVENTMANAGER_HPP_
#define NETCONFD_TEST_EXTERN_MOCKS_MOCKIEVENTMANAGER_HPP_

#include <gmock/gmock.h>
#include "IEventManager.hpp"

namespace netconfd {

class MockIEventManager : public IEventManager {
 public:
  MOCK_METHOD0(NotifyNetworkChanges, void() );
};

}
  // namespace netconfd
#endif 
