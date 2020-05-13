#include <gtest/gtest.h>

#include <string>

#include <nlohmann/json.hpp>
#include "BridgeConfig.hpp"

namespace netconf {

using nlohmann::json;

class BridgeConfigTest : public testing::Test {};

TEST_F(BridgeConfigTest, AddOneBridge) {
  ::std::string expected = R"({"Bridge":[]})";

  BridgeConfig dut{};
  auto br_name = "Bridge";

  dut.AddBridge(br_name);
  EXPECT_EQ(expected, dut.ToJson());
}

TEST_F(BridgeConfigTest, AddSameBridgeTwice) {
  ::std::string expected = R"({"Bridge":[]})";

  BridgeConfig dut{};
  auto br_name = "Bridge";

  dut.AddBridge(br_name);
  dut.AddBridge(br_name);

  EXPECT_EQ(expected, dut.ToJson());
}

TEST_F(BridgeConfigTest, DeleteOneBridge) {
  BridgeConfig dut{};
  dut.AddBridge("br0");
  dut.AddBridge("br1");

  ASSERT_EQ(R"({"br0":[],"br1":[]})", dut.ToJson());

  dut.DeleteBridge("br0");
  EXPECT_EQ(R"({"br1":[]})", dut.ToJson());
}

TEST_F(BridgeConfigTest, DeleteNonExistingBridge) {
  BridgeConfig dut{};
  dut.AddBridge("br0");
  dut.AddBridge("br1");

  ASSERT_EQ(R"({"br0":[],"br1":[]})", dut.ToJson());

  dut.DeleteBridge("br2");
  EXPECT_EQ(R"({"br0":[],"br1":[]})", dut.ToJson());
}

TEST_F(BridgeConfigTest, AssignInterfaceToBridge) {
  BridgeConfig dut{};
  dut.AddBridge("br0");

  ASSERT_EQ(R"({"br0":[]})", dut.ToJson());

  dut.AssignInterfaceToBridge("X1", "br0");
  EXPECT_EQ(R"({"br0":["X1"]})", dut.ToJson());

  dut.AddBridge("br1");
  dut.AssignInterfaceToBridge("X1", "br1");

  EXPECT_EQ(R"({"br0":[],"br1":["X1"]})", dut.ToJson());
}

TEST_F(BridgeConfigTest, DeleteInterfaceFromBridge) {
  BridgeConfig dut{};
  dut.AddBridge("br0");
  dut.AssignInterfaceToBridge("X1", "br0");

  ASSERT_EQ(R"({"br0":["X1"]})", dut.ToJson());

  dut.DeleteInterface("X1");
  EXPECT_EQ(R"({"br0":[]})", dut.ToJson());
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
  ::std::string expected = R"({"br0":["X1"]})";
  BridgeConfig dut{expected};
  EXPECT_EQ(expected, dut.ToJson());
}

TEST_F(BridgeConfigTest, BridgeConfigToString) {

  ::std::string config = R"({"br0":["X1"], "br1":["X11","X12"]})";
  BridgeConfig dut{config};
  ::std::string expected = R"(br0=X1 br1=X11,X12)";
  EXPECT_EQ(expected, dut.ToString());
}

TEST_F(BridgeConfigTest, CheckIfBridgeIsEmpty) {

  ::std::string config = R"({"br0":["X1", "X2", "X11","X12"], "br1":[]})";
  BridgeConfig dut{config};
  EXPECT_FALSE(dut.BridgeIsEmpty("br0"));
  EXPECT_TRUE(dut.BridgeIsEmpty("br1"));
}

TEST_F(BridgeConfigTest, CheckIfNonExistingBridgeIsEmpty) {

  ::std::string config = R"({"br0":["X1", "X2", "X11","X12"]})";
  BridgeConfig dut{config};
  EXPECT_FALSE(dut.BridgeIsEmpty("br0"));
  EXPECT_TRUE(dut.BridgeIsEmpty("br1"));
}

TEST_F(BridgeConfigTest, CheckIfBridgeIsEmptyWhenBridgeIsNotAnArray) {
  // Just for sake of completeness, this won't happen in productivity code
  ::std::string config = R"({"br0":["X1", "X2", "X11","X12"], "br1":{} })";
  BridgeConfig dut{config};
  EXPECT_FALSE(dut.BridgeIsEmpty("br0"));
  EXPECT_TRUE(dut.BridgeIsEmpty("br1"));
}

}  // namespace netconf
