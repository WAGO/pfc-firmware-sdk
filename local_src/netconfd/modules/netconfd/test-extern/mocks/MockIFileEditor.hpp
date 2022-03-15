// SPDX-License-Identifier: GPL-2.0-or-later
#pragma once

#include <gmock/gmock.h>
#include "IFileEditor.hpp"

namespace netconf {

class MockIFileEditor : public IFileEditor {
 public:

  MOCK_CONST_METHOD2(Read, Status(const ::std::string&, ::std::string&) );
  MOCK_CONST_METHOD2(Write, Status(const ::std::string&,const ::std::string&) );
  MOCK_CONST_METHOD2(WriteAndReplace, Status(const ::std::string&,const ::std::string&) );
  MOCK_CONST_METHOD2(Append, Status(const ::std::string&,const ::std::string&) );

};

} // namespace netconf
