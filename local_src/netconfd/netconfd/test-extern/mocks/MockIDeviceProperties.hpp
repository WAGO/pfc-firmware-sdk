#pragma once

#include <gmock/gmock.h>

#include "IDeviceProperties.hpp"

namespace netconf {

class MockIDeviceProperties : public IDeviceProperties {
 public:
  MOCK_CONST_METHOD0(GetProductPortNames,
      Interfaces());
  MOCK_CONST_METHOD0(GetOSPortNames,
      Interfaces());
  MOCK_CONST_METHOD0(GetOSInterfaceNames,
      Interfaces());
  MOCK_CONST_METHOD0(GetInterfaceNameMapping,
      InterfaceNameMapping());
  MOCK_CONST_METHOD1(ConvertProductToOSInterfaces,
      void(Interfaces &interfaces));
  MOCK_CONST_METHOD1(ConvertOSToProductInterfaces,
      void(Interfaces &interfaces));
  MOCK_CONST_METHOD1(HasInterface,
      bool(const ::std::string& ifName));
  MOCK_CONST_METHOD0(GetOrderNumber,
      ::std::string());
  MOCK_CONST_METHOD0(GetMac,
      MacAddress());
  MOCK_CONST_METHOD0(GetMacCount,
                     uint32_t());
  MOCK_CONST_METHOD1(GetIncrementedMac,
      ::std::string(uint32_t inc));
  MOCK_CONST_METHOD0(GetHostname,
      ::std::string());
};


}
