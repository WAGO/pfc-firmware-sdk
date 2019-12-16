#ifndef NETCONFD_TEST_EXTERN_MOCKS_MOCKIMACDISTRIBUTOR_HPP_
#define NETCONFD_TEST_EXTERN_MOCKS_MOCKIMACDISTRIBUTOR_HPP_

#include <gmock/gmock.h>
#include "IMacDistributor.hpp"

namespace netconfd {

class MockIMacDistributor : public IMacDistributor {
 public:
  MOCK_METHOD1(SetMac, Status(Bridge const& interface) );
};

}
  // namespace netconfd
#endif 
