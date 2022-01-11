// SPDX-License-Identifier: GPL-2.0-or-later
#pragma once

#include <gmock/gmock.h>

#include "IMacDistributor.hpp"

namespace netconf {

class MockIMacDistributor : public IMacDistributor {
 public:
  MOCK_METHOD1(AssignMacs,
      void(NetDevs &net_devs));
};

}  // namespace netconf
