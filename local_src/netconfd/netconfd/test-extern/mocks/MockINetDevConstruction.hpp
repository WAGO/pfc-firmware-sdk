// SPDX-License-Identifier: GPL-2.0-or-later
#pragma once

#include <gmock/gmock.h>

#include "../../src/IPManager/INetDevEvents.hpp"

namespace netconf {

class MockINetDevEvents : public INetDevEvents {
 public:

  MOCK_METHOD1(OnNetDevCreated,void(NetDevPtr netdev));
  MOCK_METHOD1(OnNetDevRemoved, void(NetDevPtr netdev));
  MOCK_METHOD1(OnNetDevChangeInterfaceRelations, void(NetDevPtr netdev));

};


}  // namespace netconf
