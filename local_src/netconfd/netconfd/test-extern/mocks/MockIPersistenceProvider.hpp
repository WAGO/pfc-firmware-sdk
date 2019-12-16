#ifndef NETCONFD_TEST_EXTERN_MOCKS_MOCKIPERSISITENCEPROVIDER_H_
#define NETCONFD_TEST_EXTERN_MOCKS_MOCKIPERSISITENCEPROVIDER_H_

#include <gmock/gmock.h>
#include "IPersistenceProvider.hpp"

namespace netconfd {

class MockIPersistenceProvider : public IPersistenceProvider {
 public:

  MOCK_CONST_METHOD1(Write, Status(const BridgeConfig&) );
  MOCK_CONST_METHOD1(Read, Status(const BridgeConfig&) );
  MOCK_CONST_METHOD1(Backup, Status(const ::std::string& ) );
  MOCK_CONST_METHOD3(Restore, Status(const ::std::string&, BridgeConfig&, InterfaceConfigs&) );
  MOCK_CONST_METHOD0(GetBackupParameterCount, uint32_t());

};

}
  // namespace netconfd
#endif 
