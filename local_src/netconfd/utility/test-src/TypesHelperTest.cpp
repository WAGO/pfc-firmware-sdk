// SPDX-License-Identifier: GPL-2.0-or-later
//------------------------------------------------------------------------------
///  \file
///
///  \brief    <short description of the file contents>
///
///  \author   <author> : WAGO Kontakttechnik GmbH & Co. KG
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// include files
//------------------------------------------------------------------------------
#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "TypesHelper.hpp"
#include <boost/asio/ip/network_v4.hpp>

using namespace netconfd;
using namespace testing;
using namespace std::literals;

TEST(TypesHelper, CheckBridgeConfigEquality) {

  auto bc1 = BridgeConfig { { "br0", { "X1", "X2" } }, { "br1", { "X11" } }, { "br2", { "X12" } } };
  auto bc_value = BridgeConfig { { "br0", { "X2", "X1" } }, { "br1", { "X11" } }, { "br2", { "X12" } } };
  auto bc_key = BridgeConfig { { "br0", { "X1", "X2" } }, { "br2", { "X12" } }, { "br1", { "X11" } }, };
  auto bc_diff_key = BridgeConfig { { "br3", { "X1", "X2" } }, { "br2", { "X12" } }, { "br1", { "X11" } }, };
  auto bc_diff_value = BridgeConfig { { "br0", { "X11", "X2" } }, { "br1", { "X11" } }, { "br2", { "X12" } } };
  auto bc_less_keys = BridgeConfig { { "br0", { "X1", "X2" } }, { "br2", { "X12" } } };
  auto bc_less_values = BridgeConfig { { "br0", { "X1" } }, { "br1", { "X11" } }, { "br2", { "X12" } } };

  ASSERT_TRUE(IsEqual(bc1, bc1));
  ASSERT_TRUE(IsEqual(bc1, bc_value));
  ASSERT_TRUE(IsEqual(bc1, bc_key));
  ASSERT_FALSE(IsEqual(bc1, bc_diff_key));
  ASSERT_FALSE(IsEqual(bc1, bc_diff_value));
  ASSERT_FALSE(IsEqual(bc1, bc_less_keys));
  ASSERT_FALSE(IsEqual(bc1, bc_less_values));

}

TEST(TypesHelper, CheckIPConfigEquality) {

  IPConfigs ip_config_sorted = { { "br0", IPSource::STATIC, "ip0", "net0", "bc0" }, { "br1", IPSource::STATIC, "ip1",
      "net1", "bc1" }, { "br2", IPSource::STATIC, "ip2", "net2", "bc2" } };

  IPConfigs ip_config_reverseorder = { { "br2", IPSource::STATIC, "ip2", "net2", "bc2" }, { "br1", IPSource::STATIC,
      "ip1", "net1", "bc1" }, { "br0", IPSource::STATIC, "ip0", "net0", "bc0" } };

  IPConfigs ip_config_unsorted = { { "br2", IPSource::STATIC, "ip2", "net2", "bc2" }, { "br0", IPSource::STATIC, "ip0",
      "net0", "bc0" }, { "br1", IPSource::STATIC, "ip1", "net1", "bc1" } };

  IPConfigs ip_config_differentsize = { { "br0", IPSource::STATIC, "ip0", "net0", "bc0" }, { "br1", IPSource::STATIC,
      "ip1", "net1", "bc1" } };

  IPConfigs ip_config_differentbridge = { { "br2", IPSource::STATIC, "ip2", "net2", "bc2" }, { "br0", IPSource::STATIC,
      "ip0", "net0", "bc0" }, { "br3", IPSource::STATIC, "ip1", "net1", "bc1" } };

  IPConfigs ip_config_differentipconfig = { { "br2", IPSource::STATIC, "ip2", "net2", "bc2" }, { "br0",
      IPSource::STATIC, "ip0", "net0", "bc0" }, { "br1", IPSource::NONE, "ip1", "net1", "bc1" } };

  ASSERT_TRUE(IsEqual(ip_config_sorted, ip_config_reverseorder));
  ASSERT_TRUE(IsEqual(ip_config_sorted, ip_config_unsorted));
  ASSERT_TRUE(IsEqual(ip_config_reverseorder, ip_config_unsorted));

  ASSERT_FALSE(IsEqual(ip_config_sorted, ip_config_differentsize));
  ASSERT_FALSE(IsEqual(ip_config_unsorted, ip_config_differentbridge));
  ASSERT_FALSE(IsEqual(ip_config_unsorted, ip_config_differentipconfig));

}

TEST(TypesHelper, AddNewIPConfigToConfigs) {

  IPConfig config0 = { "br0", IPSource::STATIC, "ip0", "net0", "bc0" };
  IPConfig config1 = { "br1", IPSource::STATIC, "ip1", "net1", "bc1" };

  IPConfigs configs;
  configs.push_back(config0);
  configs.push_back(config1);

  IPConfig config2 = { "br2", IPSource::STATIC, "ip2", "net2", "bc2" };

  AddIPConfig(config2, configs);

  EXPECT_EQ(configs.size(), 3);
  EXPECT_EQ(configs[0], config0);
  EXPECT_EQ(configs[1], config1);
  EXPECT_EQ(configs[2], config2);

}

TEST(TypesHelper, UpdateOneIPConfigOfConfigs) {

  IPConfig config0 = { "br0", IPSource::STATIC, "ip0", "net0", "bc0" };
  IPConfig config1 = { "br1", IPSource::STATIC, "ip1", "net1", "bc1" };

  IPConfigs configs;
  configs.push_back(config0);
  configs.push_back(config1);

  IPConfig config0_modifyed = { "br0", IPSource::NONE, "ipX0", "netX0", "bcX0" };

  AddIPConfig(config0_modifyed, configs);

  EXPECT_EQ(configs.size(), 2);
  EXPECT_EQ(configs[0], config0_modifyed);
  EXPECT_EQ(configs[1], config1);

  IPConfig config1_modifyed = { "br1", IPSource::NONE, "ipX1", "netX1", "bcX1" };

  AddIPConfig(config1_modifyed, configs);

  EXPECT_EQ(configs.size(), 2);
  EXPECT_EQ(configs[0], config0_modifyed);
  EXPECT_EQ(configs[1], config1_modifyed);
}

TEST(TypesHelper, NoIPConfigsToRemove) {

  IPConfig ip_config_1 { "br0", IPSource::STATIC, "ip0", "net0", "bc0" };
  IPConfig ip_config_2 { "br1", IPSource::STATIC, "ip1", "net1", "bc1" };
  IPConfig ip_config_3 { "br2", IPSource::STATIC, "ip2", "net2", "bc2" };

  IPConfigs ip_configs = { ip_config_1, ip_config_2, ip_config_3 };
  Bridges bridges = { "br0"s, "br1"s, "br2"s };

  IpConfigsIntersection(ip_configs, bridges);

  EXPECT_THAT(ip_configs, ElementsAre(ip_config_1, ip_config_2, ip_config_3));
}

TEST(TypesHelper, OneIpConfigToRemove) {

  IPConfig ip_config_1 { "br0", IPSource::STATIC, "ip0", "net0", "bc0" };
  IPConfig ip_config_2 { "br1", IPSource::STATIC, "ip1", "net1", "bc1" };
  IPConfig ip_config_3 { "br2", IPSource::STATIC, "ip2", "net2", "bc2" };

  IPConfigs ip_configs = { ip_config_1, ip_config_2, ip_config_3 };
  Bridges bridges = { "br0"s, "br1"s };

  IpConfigsIntersection(ip_configs, bridges);

  EXPECT_THAT(ip_configs, ElementsAre(ip_config_1, ip_config_2));
}

TEST(TypesHelper, AllIpConfigsHaveToBeRemoved) {

  IPConfig ip_config_1 { "br0", IPSource::STATIC, "ip0", "net0", "bc0" };
  IPConfig ip_config_2 { "br1", IPSource::STATIC, "ip1", "net1", "bc1" };
  IPConfig ip_config_3 { "br2", IPSource::STATIC, "ip2", "net2", "bc2" };

  IPConfigs ip_configs = { ip_config_1, ip_config_2, ip_config_3 };
  Bridges bridges = { };

  IpConfigsIntersection(ip_configs, bridges);

  EXPECT_THAT(ip_configs, IsEmpty());
}

TEST(TypesHelper, BridgeConfigEquality) {
  auto bc1 = BridgeConfig { { "br0", { "X1", "X2" } }, { "br1", { "X11" } }, { "br2", { "X12" } } };
  auto bc2 = BridgeConfig { { "br0", { "X1" } }, { "br1", { "X11" } }, { "br2", { "X12" } } };
  auto bc3 = BridgeConfig { { "br0", { "X2", "X1" } }, { "br1", { "X11" } }, { "br2", { "X12" } } };
  auto bc4 = BridgeConfig { { "br0", { "X1", "X2" } }, { "br1", { "X11", "X12" } }, { "br2", { } } };

  auto bc_inital = BridgeConfig { { "br0", { "X1", "X2", "X11", "X12" } }, { "br1", { } } };
  auto bc_inital_wo_x2 = BridgeConfig { { "br0", { "X1", "X11", "X12" } }, { "br1", { } } };
  auto bc_separated = BridgeConfig { { "br0", { "X1", "X11", "X12" } }, { "br1", { "X2" } } };

  EXPECT_TRUE(IsEqual(bc1, bc3));
  EXPECT_TRUE(IsEqual(bc3, bc1));
  EXPECT_FALSE(IsEqual(bc1, bc2));
  EXPECT_FALSE(IsEqual(bc2, bc1));
  EXPECT_FALSE(IsEqual(bc1, bc4));
  EXPECT_FALSE(IsEqual(bc4, bc1));

  EXPECT_FALSE(IsEqual(bc_inital, bc_separated));
  EXPECT_FALSE(IsEqual(bc_separated, bc_inital));

  EXPECT_FALSE(IsEqual(bc_inital, bc_inital_wo_x2));
  EXPECT_FALSE(IsEqual(bc_inital_wo_x2, bc_inital));

  EXPECT_TRUE(IsEqual(bc_inital, bc_inital));
}

TEST(TypesHelper, ComplementNetmask)
{
  auto without_netmask = IPConfig::CreateDefault("X1337");
  without_netmask.address_ = "192.168.2.42";
  ComplementNetmask(without_netmask);

  EXPECT_EQ("255.255.255.0", without_netmask.netmask_);

  auto noaddressatall = IPConfig::CreateDefault("X1337");
  ComplementNetmask(noaddressatall);

  EXPECT_EQ(ZeroIP, noaddressatall.netmask_);

  auto invalid_ip = IPConfig::CreateDefault("X1337");
  invalid_ip.address_ = "6546";
  ComplementNetmask(invalid_ip);

  EXPECT_EQ(ZeroIP, invalid_ip.netmask_);

}

TEST(TypesHelper, ComplementNetmasks)
{
  IPConfigs configs_wo_netmasks {
    { "br0", IPSource::STATIC, "192.168.1.2", ZeroIP, "" },
    { "br1337", IPSource::STATIC, "172.5.1.2", ZeroIP, "" },
    { "br42", IPSource::STATIC, "10.1.1.2", ZeroIP, "" }
  };

  IPConfigs configs_complemented {
    { "br0", IPSource::STATIC, "192.168.1.2", "255.255.255.0", "" },
    { "br1337", IPSource::STATIC, "172.5.1.2", "255.255.0.0", "" },
    { "br42", IPSource::STATIC, "10.1.1.2", "255.0.0.0", "" }
  };

  ComplementNetmasks(configs_wo_netmasks);

  EXPECT_TRUE(IsEqual(configs_complemented, configs_wo_netmasks));
}
