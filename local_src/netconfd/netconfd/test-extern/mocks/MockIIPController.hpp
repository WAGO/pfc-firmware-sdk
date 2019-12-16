#ifndef NETCONFD_TEST_EXTERN_MOCKS_MOCKIIPCONTROLLER_HPP_
#define NETCONFD_TEST_EXTERN_MOCKS_MOCKIIPCONTROLLER_HPP_

#include <gmock/gmock.h>
#include "IIPController.hpp"

namespace netconfd {

class MockIIPController : public IIPController {
 public:

  MOCK_CONST_METHOD2(GetIPConfig, Status(const Interface&, IPConfig&) );
  MOCK_CONST_METHOD1(SetIPConfig, Status(const IPConfig&) );

};

}
  // namespace netconfd
#endif 
