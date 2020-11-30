// SPDX-License-Identifier: LGPL-3.0-or-later

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "NetworkHelper.hpp"

#include "Types.hpp"

using namespace netconf;
using namespace testing;

TEST(NetworkHelper, GetPrefix) {

  ::std::string netmask = "255.255.255.128";

  ::std::string prefix = GetPrefix(netmask);

  EXPECT_EQ("25", prefix);
}


TEST(NetworkHelper, GetNetmask) {

  uint32_t prefix = 17;

  ::std::string netmask = GetNetmask(prefix);

  EXPECT_EQ("255.255.128.0", netmask);
}

TEST(NetworkHelper, GetBroadcast) {

  ::std::string address = "192.168.131.17";
  ::std::string netmask = "255.255.128.0";

  ::std::string broadcast = GetBroadcast(address, netmask);

  EXPECT_EQ("192.168.255.255", broadcast);
}

TEST(NetworkHelper, GetBroadcastOfZeroAddress) {

  ::std::string address = ZeroIP;
  ::std::string netmask = "255.255.255.0";

  ::std::string broadcast = GetBroadcast(address, netmask);

  EXPECT_EQ(ZeroIP, broadcast);
}

TEST(NetworkHelper, GetBroadcastOfZeroNetmask) {

  ::std::string address = "192.168.2.1";
  ::std::string netmask = ZeroIP;

  ::std::string broadcast = GetBroadcast(address, netmask);

  EXPECT_EQ(ZeroIP, broadcast);
}
