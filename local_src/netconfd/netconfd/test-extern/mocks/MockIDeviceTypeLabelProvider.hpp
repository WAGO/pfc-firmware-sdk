#ifndef NETCONFD_TEST_EXTERN_MOCKS_MOCKIDEVICETYPELABELPROVIDER_HPP_
#define NETCONFD_TEST_EXTERN_MOCKS_MOCKIDEVICETYPELABELPROVIDER_HPP_

#include <gmock/gmock.h>

#include "IDeviceTypeLabelProvider.hpp"


namespace netconfd {

class MockIDeviceTypeLabelProvider : public IDeviceTypeLabelProvider {
 public:
  MOCK_CONST_METHOD0(GetOrderNumber, ::std::string() );
  MOCK_CONST_METHOD0(GetMac, ::std::string() );
  MOCK_CONST_METHOD1(GetIncrementedMac, ::std::string(uint32_t) );
};

}
  // namespace netconfd

#endif 
