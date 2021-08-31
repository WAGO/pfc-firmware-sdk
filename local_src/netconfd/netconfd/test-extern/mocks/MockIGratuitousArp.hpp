// SPDX-License-Identifier: GPL-2.0-or-later
#pragma once

#include <gmock/gmock.h>
#include "IGratuitousArp.hpp"

namespace netconf {

class MockIGratuitousArp : public IGratuitousArp {
 public:
  MOCK_METHOD3(Send, Status(::std::string, const NetDevPtr, const NetDevPtr) );
};

}
