// SPDX-License-Identifier: GPL-2.0-or-later
#pragma once

#include <gmock/gmock.h>
#include "IFileEditor.hpp"

namespace netconf {

class MockIFileEditor : public IFileEditor {
 public:

  MOCK_CONST_METHOD2(Read, Error(const ::std::string&, ::std::string&) );
  MOCK_CONST_METHOD2(Write, Error(const ::std::string&,const ::std::string&) );
  MOCK_CONST_METHOD2(Append, Error(const ::std::string&,const ::std::string&) );

};

} // namespace netconf
