// SPDX-License-Identifier: GPL-2.0-or-later
#pragma once

#include <gmock/gmock.h>

#include "INetDevConstruction.hpp"

namespace netconf {

class MockINetDevConstruction : public INetDevConstruction {
 public:

  MOCK_METHOD1(OnNetDevCreated,void(NetDevPtr netdev));
  MOCK_METHOD1(OnNetDevRemoved, void(NetDevPtr netdev));

};


}  // namespace netconf
