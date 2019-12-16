#ifndef NETCONFD_TEST_EXTERN_MOCKS_MOCKIBACKUPRESTORE_HPP_
#define NETCONFD_TEST_EXTERN_MOCKS_MOCKIBACKUPRESTORE_HPP_

#include <gmock/gmock.h>
#include "IBackupRestore.hpp"

namespace netconfd {

class MockIBackupRestore : public IBackupRestore {
 public:

  MOCK_CONST_METHOD0(GetBackupParameterCount, ::std::uint32_t() );
  MOCK_CONST_METHOD3(Backup, Status(const ::std::string&,const ::std::string&, uint32_t version) );
  MOCK_CONST_METHOD3(Restore, Status(const ::std::string&, ::std::string&, uint32_t& version) );

};

}
  // namespace netconfd
#endif 
