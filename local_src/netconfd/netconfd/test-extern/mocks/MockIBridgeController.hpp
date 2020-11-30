// SPDX-License-Identifier: GPL-2.0-or-later
#pragma once

#include <gmock/gmock.h>
#include "IBridgeController.hpp"

namespace netconf {

class MockIBridgeController : public IBridgeController {
 public:
  MOCK_CONST_METHOD1(AddBridge, Error (const Bridge& bridge));
  MOCK_CONST_METHOD1(DeleteBridge, Error (const Bridge& bridge));
  MOCK_CONST_METHOD2(AddInterface, Error (const Bridge& bridge, const Interface& interface));
  MOCK_CONST_METHOD2(DeleteInterface, Error (const Bridge& bridge, const Interface& interface));

  MOCK_CONST_METHOD0(GetBridges, Bridges());
  MOCK_CONST_METHOD1(GetBridgeInterfaces, Interfaces (const Bridge& bridge));

  MOCK_CONST_METHOD0(GetInterfaces, Interfaces());

  MOCK_CONST_METHOD1(SetInterfaceUp, Error (const ::std::string& name));
  MOCK_CONST_METHOD1(SetInterfaceDown, Error (const ::std::string& name));

  MOCK_CONST_METHOD2(IsInterfaceUp, Error (const ::std::string& name, bool& is_up));
};

} // namespace netconf
