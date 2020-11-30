// SPDX-License-Identifier: LGPL-3.0-or-later

#include <gtest/gtest.h>
#include "JsonConverter.hpp"
#include <string>
#include <nlohmann/json.hpp>

namespace netconf {

using namespace testing;
using namespace std;
using namespace std::literals;
using namespace nlohmann;

class InterfaceConfigJsonTest : public ::testing::Test
{
 public:

  string single_port_config = R"({"device":"X1","state":"up","autonegotiation":"on","speed":100,"duplex":"full" })";
  string single_port_config_extra_member = R"({"device":"X1","state":"up","autonegotiation":"on","speed":100,"duplex":"full","tomuchinfo":"notneeded" })";
  string single_port_config_wrong = R"({"device":"X1","aasdfasd":"asdup","autonegotiation":"on","speed":100,"duplex":"full" })";
  string single_port_config_missing_state = R"({"device":"X1","autonegotiation":"on","speed":100,"duplex":"full" })";
  string multi_port_full = R"([{"device":"X1","state":"up","autonegotiation":"on","speed":100,"duplex":"full"},{"device":"X2","state":"down","autonegotiation":"on","speed":100,"duplex":"half"}])";
  string multi_port_minmal = R"([{"device":"X1","duplex":"full"},{"device":"X2","state":"down"}])";

  JsonConverter converter;
  InterfaceConfig expectedX1;
  InterfaceConfig expectedX2;

  void SetUp() override {
    expectedX1 = InterfaceConfig{
      "X1", InterfaceState::UP, Autonegotiation::ON, 100, Duplex::FULL
    };

    expectedX2 = InterfaceConfig{
      "X2", InterfaceState::DOWN, Autonegotiation::ON, 100, Duplex::HALF
    };
  }

  void ExpectEq(InterfaceConfig &expected, InterfaceConfig &actual) {
     EXPECT_EQ(expected.device_name_, actual.device_name_);
     EXPECT_EQ(expected.state_, actual.state_);
     EXPECT_EQ(expected.autoneg_, actual.autoneg_);
     EXPECT_EQ(expected.speed_, actual.speed_);
     EXPECT_EQ(expected.duplex_, actual.duplex_);
  }
};

TEST_F(InterfaceConfigJsonTest, parseSingleConfig){

  InterfaceConfigs port_configs;

  auto status = converter.FromJsonString(single_port_config, port_configs);

  EXPECT_TRUE(status.IsOk());
  EXPECT_EQ(1u, port_configs.size());
  ExpectEq(expectedX1, port_configs.at(0));
}



TEST_F(InterfaceConfigJsonTest, parseMultipleConfigs){

  InterfaceConfigs port_configs;

  auto status = converter.FromJsonString(multi_port_full, port_configs);

  EXPECT_TRUE(status.IsOk());
  EXPECT_EQ(2u, port_configs.size());

  ExpectEq(expectedX1, port_configs.at(0));
  ExpectEq(expectedX2, port_configs.at(1));
}

TEST_F(InterfaceConfigJsonTest, parseJsonWithWrongContents){

  InterfaceConfigs port_configs;

  auto status = converter.FromJsonString(single_port_config_wrong, port_configs);

  EXPECT_TRUE(status.IsNotOk());
  EXPECT_TRUE(port_configs.empty());

}

TEST_F(InterfaceConfigJsonTest, parseJsonWithExtraContents){

  InterfaceConfigs port_configs;

  auto status = converter.FromJsonString(single_port_config_extra_member, port_configs);

  EXPECT_TRUE(status.IsNotOk());
  EXPECT_TRUE(port_configs.empty());

}

TEST_F(InterfaceConfigJsonTest, parseJsonMissingContents){

  InterfaceConfigs port_configs;

  auto status = converter.FromJsonString(single_port_config_missing_state, port_configs);

  EXPECT_TRUE(status.IsOk());
  EXPECT_EQ(1u, port_configs.size());
  EXPECT_EQ(InterfaceState::UNKNOWN, port_configs.at(0).state_);
}

TEST_F(InterfaceConfigJsonTest, ConverToJson){
  auto full_config = R"({"device":"X1","state":"up","autonegotiation":"on","speed":100,"duplex":"full" })"_json;
  auto port_config = InterfaceConfig{
    "X1", InterfaceState::UP, Autonegotiation::ON, 100, Duplex::FULL
  };

  auto pc = InterfaceConfigs{port_config};

  auto json = converter.ToJsonString(pc);

  EXPECT_EQ(full_config.dump(), json);

}

TEST_F(InterfaceConfigJsonTest, ConvertMultiplePortsToJson){
  auto full_config = R"([{"device":"X1","state":"up","autonegotiation":"on","speed":100,"duplex":"full" }, {"device":"X2","state":"down","autonegotiation":"off","speed":42,"duplex":"half" }])"_json;

  auto pc = InterfaceConfigs{{"X1", InterfaceState::UP, Autonegotiation::ON, 100, Duplex::FULL}, {"X2", InterfaceState::DOWN, Autonegotiation::OFF, 42, Duplex::HALF}};

  auto json = converter.ToJsonString(pc);

  EXPECT_EQ(full_config.dump(), json);

}


}  // namespace netconf
