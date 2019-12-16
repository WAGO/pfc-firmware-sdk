// SPDX-License-Identifier: GPL-2.0-or-later
//------------------------------------------------------------------------------
///  \file     AJsonParser.cpp
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
#include <JsonConfigConverter/JsonConfigConverter.hpp>
#include <Status.hpp>
#include <memory>

#include "BridgeConfigurator.hpp"
#include "TypesHelper.hpp"

using namespace testing;

namespace netconfd {

class AJsonConfigConverter : public Test {
 public:
  ::std::shared_ptr<JsonConfigConverter> parser_;

  AJsonConfigConverter() {
  }

  void SetUp() override {
    parser_ = ::std::make_shared<JsonConfigConverter>();
  }

  void TearDown() override {

  }

  ::std::string const json_valid_config_ =
      R"(
  {
    "br0": [ "X1", "X2" ],
    "br1": [ "X11" ],
    "br2": [ "X12" ]
  }
  )";

  ::std::string const json_invalid_config_ =
      R"(
  {
    "br0": [ "X1", "X2" ],
    "br1": [  ,
    "br2": [ "X12" ]
  }
  )";

  BridgeConfig bridge_config_emptybridge = { { "br0", { "X1", "X2" } }, { "br1", { } }, { "br2", { } }, { "br3",
      { "X12" } } };

  ::std::string const json_emptybridge_config_ =
      R"(
  {
    "br0": [ "X1", "X2" ],
    "br1": [  ],
    "br2": [  ],
    "br3": [ "X12" ]
  }
  )";

  ::std::string const json_emptybridgename_config_ =
      R"(
  {
    "br0": [ "X1", "X2" ],
    "": [ "X11" ],
    "br2": [ "X12" ]
  }
  )";

  ::std::string const json_invalid_config_duplicat_bridge_name_ =
      R"(
   {
     "br0": [ "X1" ],
     "br0": [ "X2" ]
   }
   )";

  IPConfigs one_ip_config_ { { "br0", IPSource::STATIC, "192.168.1.17", "255.255.255.0",
      "192.168.1.255" } };
  ::std::string const json_one_ip_config_ =
      R"(
  {
    "br0": {
      "bcast": "192.168.1.255",
      "ipaddr": "192.168.1.17",
      "netmask": "255.255.255.0",
      "source": "static"
    }
  }
   )";

  IPConfigs two_ip_config_ { { "br0", IPSource::STATIC, "192.168.1.17", "255.255.255.0",
      "192.168.1.255" }, { "br1", IPSource::STATIC, "192.168.2.17", "255.255.0.0",
      "192.168.255.255" } };

  ::std::string const json_two_ip_config_ =
      R"(
  {
    "br0": {
      "bcast": "192.168.1.255",
      "ipaddr": "192.168.1.17",
      "netmask": "255.255.255.0",
      "source": "static"
    },
    "br1": {
      "bcast": "192.168.255.255",
      "ipaddr": "192.168.2.17",
      "netmask": "255.255.0.0",
      "source": "static"
    }
  }
   )";

  IPConfigs one_ip_config_empty_elements_ { { "", IPSource::NONE, "", "255.255.255.0",
      "192.168.1.255" } };

  ::std::string const json_one_ip_config_empty_elements_ =
      R"(
  {
    "": {
      "bcast": "192.168.1.255",
      "ipaddr": "",
      "netmask": "255.255.255.0",
      "source": "none"
    }
  }
   )";

};

static void ExpectStringEqIgnoreNewlineAndBlank(::std::string expected,
                                                ::std::string value) {

  expected.erase(std::remove(expected.begin(), expected.end(), '\n'), expected.end());
  expected.erase(std::remove(expected.begin(), expected.end(), ' '), expected.end());

  value.erase(std::remove(value.begin(), value.end(), '\n'), value.end());
  value.erase(std::remove(value.begin(), value.end(), ' '), value.end());

  EXPECT_EQ(expected, value);
}

//Convert bridge configuration tests

TEST_F(AJsonConfigConverter, ConvertsAJsonConfiguration) {

  BridgeConfig bridge_config;
  Status status = parser_->JsonToBridgeConfig(json_valid_config_, bridge_config);

  EXPECT_EQ(bridge_config["br0"][0], "X1");
  EXPECT_EQ(bridge_config["br0"][1], "X2");
  EXPECT_EQ(bridge_config["br1"][0], "X11");
  EXPECT_EQ(bridge_config["br2"][0], "X12");
  ASSERT_EQ(StatusCode::OK, status.Get());

}

TEST_F(AJsonConfigConverter, ConvertsAnInvalidJsonConfiguration) {

  BridgeConfig bridge_config;
  Status status = parser_->JsonToBridgeConfig(json_invalid_config_, bridge_config);

  EXPECT_EQ(0, bridge_config.size());
  EXPECT_EQ(StatusCode::JSON_CONVERT_ERROR, status.Get());

}

TEST_F(AJsonConfigConverter, ConvertsAnEmptyJsonBridgeConfiguration) {

  BridgeConfig bridge_config;
  Status status = parser_->JsonToBridgeConfig(json_emptybridge_config_, bridge_config);

  EXPECT_EQ(bridge_config["br0"][0], "X1");
  EXPECT_EQ(bridge_config["br0"][1], "X2");
  EXPECT_EQ(bridge_config["br1"].size(), 0);
  EXPECT_EQ(bridge_config["br2"].size(), 0);
  EXPECT_EQ(bridge_config["br3"][0], "X12");
  ASSERT_EQ(StatusCode::OK, status.Get());

}

TEST_F(AJsonConfigConverter, ConvertsAnEmptyBridgeNameJsonConfiguration) {

  BridgeConfig bridge_config;
  Status status = parser_->JsonToBridgeConfig(json_emptybridgename_config_,
                                              bridge_config);

  EXPECT_EQ(0, bridge_config.size());
  EXPECT_EQ(StatusCode::JSON_CONVERT_ERROR, status.Get());
}

TEST_F(AJsonConfigConverter, ConvertsAnInvalidJsonConfigurationWithTwoIdenticalNamedBridges) {

  BridgeConfig bridge_config;
  Status status = parser_->JsonToBridgeConfig(json_invalid_config_duplicat_bridge_name_,
                                              bridge_config);

  EXPECT_EQ(0, bridge_config.size());
  EXPECT_EQ(StatusCode::JSON_CONVERT_ERROR, status.Get());

}

// Converts json configuration tests

TEST_F(AJsonConfigConverter, ConvertsABridgeConfiguration) {

  BridgeConfig bridge_config = { { "br0", { "X1", "X2" } }, { "br1", { "X11" } }, { "br2",
      { "X12" } } };

  ::std::string json;
  Status status = parser_->BridgeConfigToJson(bridge_config, json);

  ExpectStringEqIgnoreNewlineAndBlank(json_valid_config_, json);
  ASSERT_EQ(StatusCode::OK, status.Get());
}

TEST_F(AJsonConfigConverter, ConvertsABridgeConfigurationWithoutInterface) {

  ::std::string json;
  Status status = parser_->BridgeConfigToJson(bridge_config_emptybridge, json);

  ExpectStringEqIgnoreNewlineAndBlank(json_emptybridge_config_, json);
  ASSERT_EQ(StatusCode::OK, status.Get());
}

TEST_F(AJsonConfigConverter, ConvertsBackAndForth) {
  BridgeConfig bridge_config1;
  Status status = parser_->JsonToBridgeConfig(json_valid_config_, bridge_config1);

  ASSERT_EQ(StatusCode::OK, status.Get());

  ::std::string json;
  status = parser_->BridgeConfigToJson(bridge_config1, json);

  ASSERT_EQ(StatusCode::OK, status.Get());

  BridgeConfig bridge_config2;
  status = parser_->JsonToBridgeConfig(json, bridge_config2);

  EXPECT_TRUE(IsEqual(bridge_config1, bridge_config2));
  ASSERT_EQ(StatusCode::OK, status.Get());
}

// Convert interfaces tests

TEST_F(AJsonConfigConverter, ParsesAnEmptyInterfacesList) {
  Interfaces interfaces = { };
  ::std::string interface_json = parser_->InterfacesToJson(interfaces);

  EXPECT_EQ("[]", interface_json);
}

TEST_F(AJsonConfigConverter, ParsesAnInterfacesListContainingOneElement) {

  Interfaces interfaces = { "X1" };
  ::std::string interface_json = parser_->InterfacesToJson(interfaces);

  EXPECT_EQ(R"(["X1"])", interface_json);
}

TEST_F(AJsonConfigConverter, ParsesAnInterfacesListContainingMoreThanOneElement) {

  Interfaces interfaces = { "X1", "X2" };
  ::std::string interface_json = parser_->InterfacesToJson(interfaces);

  EXPECT_EQ(R"(["X1","X2"])", interface_json);
}

// Convert IP configuration tests
TEST_F(AJsonConfigConverter, ParsesJsonContainingOneConfigToIPConfig) {

  IPConfigs configs;
  Status status = parser_->JsonToIPConfigs(json_one_ip_config_, configs);

  EXPECT_TRUE(configs[0] == one_ip_config_[0]);
  ASSERT_EQ(StatusCode::OK, status.Get());
}

TEST_F(AJsonConfigConverter, ParsesJsonContainingTwoConfigToIPConfig) {

  IPConfigs configs;
  Status status = parser_->JsonToIPConfigs(json_two_ip_config_, configs);

  EXPECT_TRUE(configs[0] == two_ip_config_[0]);
  EXPECT_TRUE(configs[1] == two_ip_config_[1]);
  ASSERT_EQ(StatusCode::OK, status.Get());
}

TEST_F(AJsonConfigConverter, ParsesJsonContainingEmptyElements) {

  IPConfigs configs;
  Status status = parser_->JsonToIPConfigs(json_one_ip_config_empty_elements_, configs);

  EXPECT_TRUE(configs[0] == one_ip_config_empty_elements_[0]);
  ASSERT_EQ(StatusCode::OK, status.Get());
}

TEST_F(AJsonConfigConverter, ParsesOneIPConfigToJson) {

  auto json = parser_->IPConfigsToJson(one_ip_config_);

  ExpectStringEqIgnoreNewlineAndBlank(json_one_ip_config_, json);
}

TEST_F(AJsonConfigConverter, ParsesTwoIPConfigsToJson) {

  auto json = parser_->IPConfigsToJson(two_ip_config_);

  ExpectStringEqIgnoreNewlineAndBlank(json_two_ip_config_, json);
}

TEST_F(AJsonConfigConverter, ParsesIPConfigsToJsonInCaseOfAnEmptyBridge) {

  auto json = parser_->IPConfigsToJson(one_ip_config_empty_elements_);

  ExpectStringEqIgnoreNewlineAndBlank(json_one_ip_config_empty_elements_, json);
}

}
