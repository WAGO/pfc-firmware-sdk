// SPDX-License-Identifier: GPL-2.0-or-later
#pragma once

#include <gmock/gmock.h>
#include "IIPController.hpp"

namespace netconf {

class MockIIPController : public IIPController {
 public:

  MOCK_CONST_METHOD2(GetIPConfig, Error(const Interface&, IPConfig&) );
  MOCK_CONST_METHOD1(SetIPConfig, Error(const IPConfig&) );

};

} // namespace netconf
