#ifndef NETCONFD_TEST_EXTERN_MOCKS_MOCKIPERSISTENCEJSONCONFIGCONVERTER_HPP_
#define NETCONFD_TEST_EXTERN_MOCKS_MOCKIPERSISTENCEJSONCONFIGCONVERTER_HPP_

#include <gmock/gmock.h>
#include "IPersistenceJsonConfigConverter.hpp"

namespace netconfd {

class MockIPersistenceJsonConfigConverter : public IPersistenceJsonConfigConverter {
 public:

  MOCK_CONST_METHOD4(ToJson, Status(const BridgeConfig&, const IPConfigs&, bool, ::std::string&) );
  MOCK_CONST_METHOD3(ToConfigs, Status(const ::std::string&, BridgeConfig&, IPConfigs&) );

  MOCK_CONST_METHOD5(ToJson, Status(const BridgeConfig&, const IPConfigs&, const DipSwitchIpConfig&,
                                    bool, ::std::string&) );
  MOCK_CONST_METHOD4(ToConfigs, Status(const ::std::string&, BridgeConfig&, IPConfigs&, DipSwitchIpConfig&) );

};

}
  // namespace netconfd
#endif 
