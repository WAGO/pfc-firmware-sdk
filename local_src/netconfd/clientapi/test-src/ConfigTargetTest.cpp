// SPDX-License-Identifier: GPL-2.0-or-later

#include "BridgeConfig.hpp"
#include "IPConfig.hpp"
#include "InterfaceConfig.hpp"
#include "JsonConverter.hpp"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "Types.hpp"

using namespace testing;

namespace netconf {
namespace api {

class ConfigTest_Target : public Test {
 public:
  BridgeConfig seperated_;
  BridgeConfig switched_;

  ConfigTest_Target() {
    seperated_.AddBridge("br0");
    seperated_.AssignInterfaceToBridge("X1", "br0");
    seperated_.AddBridge("br1");
    seperated_.AssignInterfaceToBridge("X2", "br1");

    switched_.AddBridge("br0");
    switched_.AssignInterfaceToBridge("X1", "br0");
    switched_.AssignInterfaceToBridge("X2", "br0");
    switched_.AddBridge("br1");
  }

  void SetUp() override {
  }

  void TearDown() override {
  }
};

TEST_F(ConfigTest_Target, SetAndGetBridgeConfig) {
  Status s1                     = SetBridgeConfig(seperated_);
  BridgeConfig actual_seperated = GetBridgeConfig();
  EXPECT_EQ(Status::OK, s1);

  JsonConverter jc;
  EXPECT_EQ(R"({"br0":["X1"],"br1":["X2"]})", jc.ToJsonString(actual_seperated.GetConfig()));

  Status s2                    = SetBridgeConfig(switched_);
  BridgeConfig actual_switched = GetBridgeConfig();
  EXPECT_EQ(Status::OK, s2);
  EXPECT_EQ(R"({"br0":["X1","X2"]})", jc.ToJsonString(actual_switched.GetConfig()));
}

TEST_F(ConfigTest_Target, SetAndGetIpConfig) {
  SetBridgeConfig(seperated_);

  Status status;
  IPConfig ip_config1_br1_{"br1", IPSource::STATIC, "192.168.5.5", "255.255.255.0"};
  IPConfig ip_config2_br1_{"br1", IPSource::STATIC, "192.168.6.6", "255.255.255.0"};

  IPConfigs ip_configs1;
  ip_configs1.AddIPConfig(ip_config1_br1_);
  status             = SetIPConfigs(ip_configs1);
  IPConfigs actual_1 = GetIPConfigs();
  EXPECT_EQ(Status::OK, status);
  EXPECT_EQ(ip_config1_br1_, actual_1.GetIPConfig("br1"));

  IPConfigs ip_configs2;
  ip_configs2.AddIPConfig(ip_config2_br1_);
  status             = SetIPConfigs(ip_configs2);
  IPConfigs actual_2 = GetIPConfigs();
  EXPECT_EQ(Status::OK, status);
  EXPECT_EQ(ip_config2_br1_, *actual_2.GetIPConfig("br1"));
}

TEST_F(ConfigTest_Target, DeleteIpConfig) {
  SetBridgeConfig(seperated_);

  IPConfig ip_config1_br1_{"br1", IPSource::STATIC, "192.168.5.5", "255.255.255.0"};

  IPConfigs ip_configs1;
  ip_configs1.AddIPConfig(ip_config1_br1_);
  SetIPConfigs(ip_configs1);
  IPConfigs actual_1 = GetIPConfigs();
  ASSERT_EQ(ip_config1_br1_, actual_1.GetIPConfig("br1"));

  DeleteIPConfig("br1");
  IPConfigs actual_2 = GetIPConfigs();
  IPConfig expected{"br1", IPSource::NONE, netconf::ZeroIP, netconf::ZeroIP};
  EXPECT_EQ(expected,*actual_2.GetIPConfig("br1"));
}


}  // namespace api
}  // namespace netconf
