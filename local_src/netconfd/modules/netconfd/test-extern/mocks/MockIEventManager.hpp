// SPDX-License-Identifier: GPL-2.0-or-later
#pragma once

#include <gmock/gmock.h>

#include "IEventManager.hpp"

namespace netconf {

class MockIEventManager : public IEventManager {
 public:
  MOCK_METHOD2(NotifyNetworkChanges, void(const EventLayer, Interface));
  MOCK_METHOD0(ProcessPendingEvents, void());
};

}  // namespace netconf
