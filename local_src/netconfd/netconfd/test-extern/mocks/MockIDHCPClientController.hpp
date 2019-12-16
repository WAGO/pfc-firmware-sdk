#ifndef NETCONFD_TEST_EXTERN_MOCKS_MOCKIDHCPCLIENTCONTROLLER_HPP_
#define NETCONFD_TEST_EXTERN_MOCKS_MOCKIDHCPCLIENTCONTROLLER_HPP_

#include <gmock/gmock.h>
#include "IDHCPClientController.hpp"

namespace netconfd {

class MockIDHCPClientController : public IDHCPClientController {
 public:

  MOCK_CONST_METHOD1(StartClient, Status(const Bridge& bridge) );
  MOCK_CONST_METHOD1(StopClient, Status(const Bridge& bridge) );
  MOCK_CONST_METHOD1(GetStatus, DHCPClientStatus(const Bridge& bridge) );

};

}
  // namespace netconfd
#endif 
