// SPDX-License-Identifier: GPL-2.0-or-later
#pragma once

#include <gmock/gmock.h>
#include "IBackupRestore.hpp"

namespace netconf {

class MockIBackupRestore : public IBackupRestore {
 public:

  MOCK_CONST_METHOD0(GetBackupParameterCount, ::std::uint32_t() );
  MOCK_CONST_METHOD4(Backup, Status(const ::std::string&,const ::std::string&,const ::std::string&, uint32_t version) );
  MOCK_CONST_METHOD4(Restore, Status(const ::std::string&, ::std::string&, ::std::string&, uint32_t& version) );

};

} // namespace netconf
