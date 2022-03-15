// SPDX-License-Identifier: GPL-2.0-or-later

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "IPLink.hpp"
#include "MockIEventManager.hpp"
#include "MockIIPConfigure.hpp"
#include "MockIGratuitousArp.hpp"
#include "MockIIPLinks.hpp"
#include "NetDev.hpp"

using namespace testing;

namespace netconf {

TEST(IpLinkTest, GetAttributes) {

  int if_index = 1;
  ::std::string if_name = "XXX";
  ::std::string address = "1.1.1.1";
  ::std::string netmask = "255.255.255.0";
  eth::InterfaceLinkState state = eth::InterfaceLinkState::Up;

  IPLink link(if_index, if_name, address, netmask, state);

  EXPECT_EQ(link.GetName(), if_name);
  EXPECT_EQ(link.GetIfIndex(), if_index);
  EXPECT_EQ(link.GetAddress(), address);
  EXPECT_EQ(link.GetIPConfig().interface_, if_name);
  EXPECT_EQ(link.GetLinkState(), state);
}

TEST(IpLinkTest, SetGetIpConfig) {

  ::std::string if_name = "br0";
  IPLink link(1, if_name, "", "", eth::InterfaceLinkState::Up);

  IPSource source = IPSource::STATIC;
  ::std::string address = "1.1.1.1";
  ::std::string netmask = "255.255.255.0";
  IPConfig new_ip_config(if_name,source,address,netmask);
  link.SetIPConfig(new_ip_config);

  EXPECT_EQ(link.GetIPConfig(), new_ip_config);
}

TEST(IpLinkTest, SetGetIP) {

  IPLink link(1, "br0", "1.1.1.1", "255.0.0.0", eth::InterfaceLinkState::Up);
  auto expected_address = "3.3.3.3";
  auto expected_netmask = "255.255.255.0";
  link.SetAddress(expected_address, expected_netmask);

  EXPECT_EQ(link.GetCurrentIPConfig().address_, expected_address);
  EXPECT_EQ(link.GetCurrentIPConfig().netmask_, expected_netmask);
}


}
