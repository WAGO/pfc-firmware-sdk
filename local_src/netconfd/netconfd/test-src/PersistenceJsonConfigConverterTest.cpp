// SPDX-License-Identifier: GPL-2.0-or-later
//------------------------------------------------------------------------------
///  \file     PersistenceProviderTest.cpp
///
///  \brief    <short description of the file contents>
///
///  \author   <author> : WAGO Kontakttechnik GmbH & Co. KG
//------------------------------------------------------------------------------
#include "TypesHelper.hpp"

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <PersistenceJsonConfigConverter.hpp>

using namespace testing;

namespace netconfd {

class APersistenceJsonConfigConverter : public Test {
 public:

  ::std::unique_ptr<PersistenceJsonConfigConverter> json_converter_;

  APersistenceJsonConfigConverter() {
    json_converter_ = ::std::make_unique<PersistenceJsonConfigConverter>();
  }

  void SetUp() override {
  }

  void TearDown() override {

  }

  BridgeConfig bridge_config = { { "br0", { "X1" } }, { "br1", { "X2" } } };

  IPConfigs ip_configs { { "br0", IPSource::STATIC, "192.168.2.17", "255.255.255.0",
      "192.168.2.255" }, { "br1", IPSource::DHCP, "172.29.233.17", "255.255.0.0",
      "172.29.255.255" } };

  IPConfigs ip_configs_empty { };

  ::std::string json_content_ =
      R"(
    {
    "bridge-config" : {
      "br0": ["X1"],
      "br1": ["X2"]
    },
    "ip-config" : {
      "br0" : {
        "bcast" : "192.168.2.255",
        "ipaddr" : "192.168.2.17",
        "netmask" : "255.255.255.0",
        "source" : "static"
      },
    "br1" : {
        "bcast" : "172.29.255.255",
        "ipaddr" : "172.29.233.17",
        "netmask" : "255.255.0.0",
        "source" : "dhcp"
      }
    }
    }
)";

  ::std::string json_content_empty_ip_config =
      R"(
    {
    "bridge-config" : {
      "br0": ["X1"],
      "br1": ["X2"]
    },
    "ip-config" : {}
    }
)";

  ::std::string json_content_missing_key_bridgeconfig =
      R"(
    {
    "xxx" : {
      "br0": ["X1"],
      "br1": ["X2"]
    },
    "ip-config" : {
      "br0" : {
        "bcast" : "192.168.2.255",
        "ipaddr" : "192.168.2.17",
        "netmask" : "255.255.255.0",
        "source" : "static"
      },
      "br1" : {
        "bcast" : "172.29.255.255",
        "ipaddr" : "172.29.233.17",
        "netmask" : "255.255.0.0",
        "source" : "dhcp"
      }
    }
    }
)";

  ::std::string json_content_missing_key_ipconfig =
      R"(
    {
    "bridge-config" : {
      "br0": ["X1"],
      "br1": ["X2"]
    },
    "xxx" : {
      "br0" : {
        "source" : "static",
        "ipaddr" : "192.168.2.17",
        "netmask" : "255.255.255.0",
        "bcast" : "192.168.2.255"
      },
      "br1" : {
        "source" : "dhcp",
        "ipaddr" : "172.29.233.17",
        "netmask" : "255.255.0.0",
        "bcast" : "172.29.255.255"
      }
    }
    }
)";



  BridgeConfig bridge_config_bridge_without_interface = { { "br0", {} }, { "br1", { "X2" } } };

  ::std::string json_content_bridge_without_interface =
      R"(
    {
    "bridge-config" : {
      "br0": [ ],
      "br1": ["X2"]
    },
    "ip-config" : {}
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

TEST_F(APersistenceJsonConfigConverter, ConvertsConfigsToJson) {

  ::std::string json;
  Status status = json_converter_->ToJson(bridge_config, ip_configs, false, json);

  ExpectStringEqIgnoreNewlineAndBlank(json_content_, json);
  ASSERT_EQ(StatusCode::OK, status.Get());

}

TEST_F(APersistenceJsonConfigConverter, ConvertsConfigsToPrettyJson) {

  ::std::string json;
  Status status = json_converter_->ToJson(bridge_config, ip_configs, true, json);

  ExpectStringEqIgnoreNewlineAndBlank(json_content_, json);
  ASSERT_EQ(StatusCode::OK, status.Get());

}

TEST_F(APersistenceJsonConfigConverter, ConvertsABridgeConfigWithEmptyIPConfigToPrettyJson) {

  ::std::string json;
  Status status = json_converter_->ToJson(bridge_config, ip_configs_empty, true, json);

  ExpectStringEqIgnoreNewlineAndBlank(json_content_empty_ip_config, json);
  ASSERT_EQ(StatusCode::OK, status.Get());

}

TEST_F(APersistenceJsonConfigConverter, ConvertsABridgeConfigWithoutAssignedInterfaceToPrettyJson) {


  ::std::string json;
  Status status = json_converter_->ToJson(bridge_config_bridge_without_interface, ip_configs_empty, true, json);

  ExpectStringEqIgnoreNewlineAndBlank(json_content_bridge_without_interface, json);
  ASSERT_EQ(StatusCode::OK, status.Get());

}


TEST_F(APersistenceJsonConfigConverter, ConvertsJsonToConfigs) {

  BridgeConfig bridge_config_generated;
  IPConfigs ip_configs_generated;
  Status status = json_converter_->ToConfigs(json_content_, bridge_config_generated,
                                             ip_configs_generated);

  ASSERT_EQ(StatusCode::OK, status.Get());
  EXPECT_TRUE(IsEqual(bridge_config, bridge_config_generated));
  EXPECT_TRUE(IsEqual(ip_configs, ip_configs_generated));

}

TEST_F(APersistenceJsonConfigConverter, TriesToConvertsConfigsToJsonInCaseOfMissingKey_bridgeconfig) {

  BridgeConfig bridge_config_generated;
  IPConfigs ip_configs_generated;
  Status status = json_converter_->ToConfigs(json_content_missing_key_bridgeconfig,
                                             bridge_config_generated,
                                             ip_configs_generated);

  EXPECT_EQ(StatusCode::JSON_CONVERT_ERROR, status.Get());
  EXPECT_EQ(0, bridge_config_generated.size());
  EXPECT_EQ(0, ip_configs_generated.size());

}

TEST_F(APersistenceJsonConfigConverter, TriesToConvertsConfigsToJsonInCaseOfMissingKey_ipconfig) {

  BridgeConfig bridge_config_generated;
  IPConfigs ip_configs_generated;
  Status status = json_converter_->ToConfigs(json_content_missing_key_ipconfig,
                                             bridge_config_generated,
                                             ip_configs_generated);

  EXPECT_EQ(StatusCode::JSON_CONVERT_ERROR, status.Get());
  EXPECT_EQ(0, bridge_config_generated.size());
  EXPECT_EQ(0, ip_configs_generated.size());

}



} /* namespace netconfd */
