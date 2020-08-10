#pragma once

#include <gmock/gmock.h>
#include "IEventManager.hpp"

namespace netconf {

class MockIEventManager : public IEventManager {
 public:
  MOCK_METHOD3(NotifyNetworkChanges, void(const EventType, const EventLayer, Interface) );
  MOCK_METHOD0(ProcessEvents, void());
};

}
