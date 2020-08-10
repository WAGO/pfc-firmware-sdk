// SPDX-License-Identifier: GPL-2.0-or-later

#include <gtest/gtest.h>
#include "NetDev.hpp"
#include <sys/socket.h>
#include <linux/if.h>

using namespace testing;

namespace netconf {

class NetDevFlagChangeTest : public Test {
 public:
};

TEST_F(NetDevFlagChangeTest, ChangesWithoutCallback) {

  auto netdev = MakeNetDev(0, "abridge", DeviceType::Bridge);

  std::uint32_t newflags = IFF_LOWER_UP;

  netdev->SetIfFlags(newflags);
}

TEST_F(NetDevFlagChangeTest, CallbackCalledWhenLowerUpChanges) {

  int calls = 0;
  eth::InterfaceLinkState state = eth::InterfaceLinkState::Down;
  auto callback = [&](NetDev& /*dev*/, eth::InterfaceLinkState newState) {
    ++calls;
    state = newState;
  };

  auto netdev = MakeNetDev(0, "abridge", DeviceType::Bridge);
  netdev->SetLinkChangeHandler(callback);
  std::uint32_t newflags = IFF_LOWER_UP;

  netdev->SetIfFlags(newflags);
  newflags |= IFF_UP;
  netdev->SetIfFlags(newflags);

  EXPECT_EQ(1u, calls);
}

}  // namespace netconf
