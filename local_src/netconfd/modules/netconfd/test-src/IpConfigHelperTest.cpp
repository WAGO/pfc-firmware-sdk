// SPDX-License-Identifier: GPL-2.0-or-later
//------------------------------------------------------------------------------

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "IpConfigHelper.hpp"

namespace netconf {

using namespace ::testing;

class IpConfigHelperTest : public ::testing::Test {
 public:

  NetDevs netdevs_;
  Interfaces unassignable_interfaces_;

  void SetUp() override {

    netdevs_.insert(netdevs_.begin(), { ::std::make_shared<NetDev>(0, "ethX1", "X1", DeviceType::Ethernet),
                        ::std::make_shared<NetDev>(0, "ethX2", "X2", DeviceType::Ethernet),
                        ::std::make_shared<NetDev>(0, "br0", "br0", DeviceType::Bridge),
                        ::std::make_shared<NetDev>(0, "br2", "br0", DeviceType::Bridge), });

    unassignable_interfaces_.assign( { "ethX1" });

  }

};

TEST_F(IpConfigHelperTest, UnknownInterfaceInConfig) {

  IPConfig known { "br0", IPSource::DHCP, "0.0.0.0", "0.0.0.0" };
  IPConfig unknown { "br42", IPSource::DHCP, "0.0.0.0", "0.0.0.0" };
  IPConfig known2 { "br2", IPSource::DHCP, "0.0.0.0", "0.0.0.0"};

  IPConfigs config_to_be_set { known, unknown, known2 };

  CleanWithRespectToSystem(config_to_be_set, netdevs_, unassignable_interfaces_);

  EXPECT_EQ(2u, config_to_be_set.size());
  EXPECT_EQ(known, config_to_be_set.at(0));
  EXPECT_EQ(known2, config_to_be_set.at(1));
}

TEST_F(IpConfigHelperTest, InterfaceFiltered) {

  IPConfig known { "br0", IPSource::DHCP, "0.0.0.0", "0.0.0.0" };
  IPConfig not_assignable { "ethX1", IPSource::DHCP, "0.0.0.0", "0.0.0.0" };
  IPConfigs config_to_be_set { known, not_assignable };

  CleanWithRespectToSystem(config_to_be_set, netdevs_, unassignable_interfaces_);

  EXPECT_EQ(1u, config_to_be_set.size());
  EXPECT_EQ(known, config_to_be_set.at(0));
}

TEST_F(IpConfigHelperTest, RemoveConflictsFromIpConfig){
  IPConfig testee{ "br0", IPSource::STATIC, "192.168.0.1", "255.255.255.0" };
  IPConfigs config_withconflicts {testee, { "br1", IPSource::STATIC, "192.168.0.100", "255.255.255.0" }};
  IPConfigs config_withoutconflicts {testee, { "br1", IPSource::STATIC, "192.168.2.100", "255.255.255.0" }};
  IPConfigs config_withboth {testee, { "br1", IPSource::STATIC, "192.168.2.100", "255.255.255.0" }, { "br2", IPSource::STATIC, "192.168.0.100", "255.255.255.0" }};
  IPConfigs config_withdifferentsubnets {testee, { "br1", IPSource::STATIC, "192.168.2.100", "255.255.255.0" }, { "br2", IPSource::STATIC, "192.168.3.100", "255.255.0.0" }};

  RemoveIpConfigsWithConflicsTo(testee, config_withconflicts);
  RemoveIpConfigsWithConflicsTo(testee, config_withoutconflicts);
  RemoveIpConfigsWithConflicsTo(testee, config_withboth);
  RemoveIpConfigsWithConflicsTo(testee, config_withdifferentsubnets);

  EXPECT_THAT(config_withconflicts, ElementsAre(testee));
  EXPECT_THAT(config_withoutconflicts, ElementsAre(testee, IPConfig{ "br1", IPSource::STATIC, "192.168.2.100", "255.255.255.0" }));
  EXPECT_THAT(config_withboth, ElementsAre(testee, IPConfig{ "br1", IPSource::STATIC, "192.168.2.100", "255.255.255.0" }));
  EXPECT_THAT(config_withdifferentsubnets, ElementsAre(testee, IPConfig{ "br1", IPSource::STATIC, "192.168.2.100", "255.255.255.0" }));

}

}
