#ifndef NETCONFD_TEST_EXTERN_MOCKS_MOCKIBOOTPCLIENTCONTROLLER_HPP_
#define NETCONFD_TEST_EXTERN_MOCKS_MOCKIBOOTPCLIENTCONTROLLER_HPP_

#include <gmock/gmock.h>

#include "IBootpClientController.hpp"

namespace netconfd {

class MockIBootpController : public IBootpClientController {
 public:

  MOCK_CONST_METHOD1(StartClient, Status(const Bridge& bridge) );
  MOCK_CONST_METHOD1(StopClient, Status(const Bridge& bridge) );
  MOCK_CONST_METHOD1(GetStatus, BootpClientStatus(const Bridge& bridge) );

};

}
  // namespace netconfd
#endif
