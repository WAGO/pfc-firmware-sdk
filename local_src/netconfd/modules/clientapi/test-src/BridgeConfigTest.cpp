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

struct BridgeConfigTest : public testing::Test {
  const std::string device_info = R"([{"ipreadonly":false,"label":"br0","name":"br0","type":"bridge"},{"ipreadonly":false,"label":"br1","name":"br1","type":"bridge"},{"ipreadonly":false,"label":"br2","name":"br2","type":"bridge"},{"ipreadonly":false,"label":"br3","name":"br3","type":"bridge"},{"ipreadonly":true,"label":"eth0","name":"eth0","type":"ethernet"},{"autonegsupported":true,"ipreadonly":true,"label":"X1","name":"ethX1","supportedlinkmodes":[{"duplex":"half","speed":10},{"duplex":"full","speed":10},{"duplex":"half","speed":100},{"duplex":"full","speed":100}],"type":"port"},{"autonegsupported":true,"ipreadonly":true,"label":"X2","name":"ethX2","supportedlinkmodes":[{"duplex":"half","speed":10},{"duplex":"full","speed":10},{"duplex":"half","speed":100},{"duplex":"full","speed":100}],"type":"port"},{"autonegsupported":true,"ipreadonly":true,"label":"X11","name":"ethX11","supportedlinkmodes":[{"duplex":"half","speed":10},{"duplex":"full","speed":10},{"duplex":"half","speed":100},{"duplex":"full","speed":100}],"type":"port"},{"autonegsupported":true,"ipreadonly":true,"label":"X12","name":"ethX12","supportedlinkmodes":[{"duplex":"half","speed":10},{"duplex":"full","speed":10},{"duplex":"half","speed":100},{"duplex":"full","speed":100}],"type":"port"}])";
  InterfaceInformations iis;
  void SetUp() override {
    JsonConverter converter;
    converter.FromJsonString(device_info, iis);
  }

  void ExpectBridgeConfigValidation(std::string cfg, StatusCode expected_result){
    BridgeConfig config;
    auto status = MakeBridgeConfig(cfg, config);
    ASSERT_TRUE(status.IsOk());
    auto result = ValidateBridgeConfig(config, iis);
    EXPECT_EQ(expected_result, result.GetStatusCode());


  }
};

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


TEST_F(BridgeConfigTest, ValidateBridgeConfig) {

  ExpectBridgeConfigValidation(R"({"br0":["X1", "X2"], "br1":["X11"], "br2":["X12"] })", StatusCode::OK);
  ExpectBridgeConfigValidation(R"({"br0":["X1", "X2"], "br1":["X11"], "br42":["X12"] })", StatusCode::OK);
  ExpectBridgeConfigValidation(R"({"br0":["X1", "X2"], "hans":["X11"], "br42":["X12"] })", StatusCode::OK);
  ExpectBridgeConfigValidation(R"({"br0":["X1", "X2"], "br1":["X1"], "br2":["X12"] })", StatusCode::ENTRY_DUPLICATE);
  ExpectBridgeConfigValidation(R"({"br0":["X1", "X1"], "br1":["X11"], "br2":["X12"] })", StatusCode::ENTRY_DUPLICATE);
  ExpectBridgeConfigValidation(R"({"br0":["X1", "X123"], "br1":["X11"], "br2":["X12"] })", StatusCode::INTERFACE_NOT_EXISTING);
  ExpectBridgeConfigValidation(R"({"br0":["X1", "X123"], "br1":["X11"], "br2":["ethX1"] })", StatusCode::INTERFACE_NOT_EXISTING);
}

}  // namespace api
}  // namespace netconf
