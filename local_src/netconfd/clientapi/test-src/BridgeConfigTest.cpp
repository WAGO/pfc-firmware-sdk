// SPDX-License-Identifier: LGPL-3.0-or-later

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <string>

#include <nlohmann/json.hpp>
#include "BridgeConfig.hpp"
#include "JsonConverter.hpp"

namespace netconf {
namespace api {

using nlohmann::json;

class BridgeConfigTest : public testing::Test {};

TEST_F(BridgeConfigTest, AddOneBridge) {
  ::std::string expected = R"({"Bridge":[]})";

  BridgeConfig dut{};
  auto br_name = "Bridge";

  dut.AddBridge(br_name);

  JsonConverter jc;
  EXPECT_EQ(expected, jc.ToJsonString(dut.GetConfig()));
}

TEST_F(BridgeConfigTest, AddSameBridgeTwice) {
  ::std::string expected = R"({"Bridge":[]})";

  BridgeConfig dut{};
  auto br_name = "Bridge";

  dut.AddBridge(br_name);
  dut.AddBridge(br_name);

  JsonConverter jc;
  EXPECT_EQ(expected, jc.ToJsonString(dut.GetConfig()));
}

TEST_F(BridgeConfigTest, DeleteOneBridge) {
  BridgeConfig dut{};
  dut.AddBridge("br0");
  dut.AddBridge("br1");

  JsonConverter jc;
  ASSERT_EQ(R"({"br0":[],"br1":[]})", jc.ToJsonString(dut.GetConfig()));

  dut.DeleteBridge("br0");
  EXPECT_EQ(R"({"br1":[]})", jc.ToJsonString(dut.GetConfig()));
}

TEST_F(BridgeConfigTest, DeleteNonExistingBridge) {
  BridgeConfig dut{};
  dut.AddBridge("br0");
  dut.AddBridge("br1");

  JsonConverter jc;
  ASSERT_EQ(R"({"br0":[],"br1":[]})", jc.ToJsonString(dut.GetConfig()));

  dut.DeleteBridge("br2");
  EXPECT_EQ(R"({"br0":[],"br1":[]})", jc.ToJsonString(dut.GetConfig()));
}

TEST_F(BridgeConfigTest, AssignInterfaceToBridge) {
  BridgeConfig dut{};
  dut.AddBridge("br0");

  JsonConverter jc;
  ASSERT_EQ(R"({"br0":[]})", jc.ToJsonString(dut.GetConfig()));

  dut.AssignInterfaceToBridge("X1", "br0");
  EXPECT_EQ(R"({"br0":["X1"]})", jc.ToJsonString(dut.GetConfig()));

  dut.AddBridge("br1");
  dut.AssignInterfaceToBridge("X1", "br1");

  EXPECT_EQ(R"({"br0":[],"br1":["X1"]})", jc.ToJsonString(dut.GetConfig()));
}

TEST_F(BridgeConfigTest, DeleteInterfaceFromBridge) {
  BridgeConfig dut{};
  dut.AddBridge("br0");
  dut.AssignInterfaceToBridge("X1", "br0");

  JsonConverter jc;
  ASSERT_EQ(R"({"br0":["X1"]})", jc.ToJsonString(dut.GetConfig()));

  dut.DeleteInterface("X1");
  EXPECT_EQ(R"({"br0":[]})", jc.ToJsonString(dut.GetConfig()));
}

TEST_F(BridgeConfigTest, GetBridgeOfInterface) {
  BridgeConfig dut{};
  ::std::string expected_bridge = "br0";
  dut.AddBridge(expected_bridge);
  dut.AssignInterfaceToBridge("X1", "br0");

  ::std::string actual_bridge = dut.GetBridgeOfInterface("X1");

  EXPECT_EQ(expected_bridge, actual_bridge);
}

TEST_F(BridgeConfigTest, InitializeWithConfiguration) {
  auto expected = R"({"br0":["X1"]})";
  BridgeConfig config;
  auto status = MakeBridgeConfig(expected,config);

  JsonConverter jc;
  EXPECT_EQ(expected, jc.ToJsonString(config.GetConfig()));
}

TEST_F(BridgeConfigTest, BridgeConfigToString) {
  BridgeConfig config;
  auto status = MakeBridgeConfig(R"({})", config);
  JsonConverter jc;
  EXPECT_EQ("{}", jc.ToJsonString(config.GetConfig()));
}

TEST_F(BridgeConfigTest, CheckIfBridgeIsEmpty) {
  BridgeConfig config;
  auto status = MakeBridgeConfig(R"({"br0":["X1", "X2", "X11","X12"], "br1":[]})", config);
  EXPECT_FALSE(config.BridgeIsEmpty("br0"));
  EXPECT_TRUE(config.BridgeIsEmpty("br1"));
}

TEST_F(BridgeConfigTest, CheckIfNonExistingBridgeIsEmpty) {
  BridgeConfig config;
  auto status = MakeBridgeConfig(R"({"br0":["X1", "X2", "X11","X12"]})", config);
  EXPECT_FALSE(config.BridgeIsEmpty("br0"));
  EXPECT_TRUE(config.BridgeIsEmpty("br1"));
}

TEST_F(BridgeConfigTest, CheckIfBridgeIsEmptyWhenBridgeIsNotAnArray) {
  // Just for sake of completeness, this won't happen in productivity code
  BridgeConfig config;
  auto status = MakeBridgeConfig(R"({"br0":["X1", "X2", "X11","X12"], "br1":[] })", config);
  EXPECT_FALSE(config.BridgeIsEmpty("br0"));
  EXPECT_TRUE(config.BridgeIsEmpty("br1"));
}

TEST_F(BridgeConfigTest, GetInterfaceListOfBridge) {
  BridgeConfig config;
  auto status = MakeBridgeConfig(R"({"br0":["X1", "X2", "X11","X12"], "br1":[] })", config);
  EXPECT_EQ(4, config.GetBridgeInterfaces("br0").size());
  EXPECT_EQ(0, config.GetBridgeInterfaces("br1").size());
  EXPECT_EQ(0, config.GetBridgeInterfaces("br42").size());
}

TEST_F(BridgeConfigTest, AreInSameBridge) {
  ::std::string config = R"({"br0":["X1", "X2", "X11","X12"], "br1":[] })";
  ::std::string config2 = R"({"br0":["X1", "X11","X12"], "br1":["X2"] })";
  ::std::string config3 = R"({"br0":[], "br1":[] })";

  BridgeConfig cfg1;
  auto status = MakeBridgeConfig(config, cfg1);
  BridgeConfig cfg2;
  status = MakeBridgeConfig(config2, cfg2);
  BridgeConfig cfg3;
  status = MakeBridgeConfig(config3, cfg3);

  EXPECT_TRUE(cfg1.AreInSameBridge({"X1", "X2"}));
  EXPECT_TRUE(cfg1.AreInSameBridge({"X1"}));
  EXPECT_FALSE(cfg1.AreInSameBridge({"X1", "X42"}));
  EXPECT_FALSE(cfg1.AreInSameBridge({"X111"}));
  EXPECT_FALSE(cfg1.AreInSameBridge({""}));

  EXPECT_TRUE(cfg2.AreInSameBridge({"X1"}));
  EXPECT_TRUE(cfg2.AreInSameBridge({"X1", "X12"}));
  EXPECT_FALSE(cfg2.AreInSameBridge({"X1", "X2"}));

  EXPECT_FALSE(cfg3.AreInSameBridge({"X1", "X2"}));

}

TEST_F(BridgeConfigTest, GetBridges) {
  BridgeConfig config;
  auto status = MakeBridgeConfig(R"({"br0":["X1", "X2"], "br1":["X11"], "br2":["X12"] })", config);
  auto bridges = config.GetBridges();
  EXPECT_EQ(3, bridges.size());
  EXPECT_THAT(bridges, ::testing::ElementsAre("br0", "br1", "br2"));
}

}  // namespace api
}  // namespace netconf
