// SPDX-License-Identifier: GPL-2.0-or-later
#pragma once

#include <gmock/gmock.h>

#include "IBridgeController.hpp"

namespace netconf {

class MockIBridgeController : public IBridgeController {
 public:
  MOCK_CONST_METHOD1(AddBridge,
      Status(const Bridge& bridge));
  MOCK_CONST_METHOD1(DeleteBridge,
      Status(const Bridge& bridge));
  MOCK_CONST_METHOD2(AddInterface,
      Status(const Bridge& bridge, const Interface& interface));
  MOCK_CONST_METHOD2(DeleteInterface,
      Status(const Bridge& bridge, const Interface& interface));
  MOCK_CONST_METHOD0(GetBridges,
      Bridges());
  MOCK_CONST_METHOD1(GetBridgeInterfaces,
      Interfaces(const Bridge& bridge));
  MOCK_CONST_METHOD0(GetInterfaces,
      Interfaces());
  MOCK_CONST_METHOD1(SetInterfaceUp,
      Status(const ::std::string& name));
  MOCK_CONST_METHOD1(SetInterfaceDown,
      Status(const ::std::string& name));
  MOCK_CONST_METHOD2(IsInterfaceUp,
      Status(const ::std::string& name, bool& is_up));
  MOCK_CONST_METHOD2(SetAgetime,
      void(const Bridge& bridge, int seconds));
};

} // netconf
