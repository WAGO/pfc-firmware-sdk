// SPDX-License-Identifier: GPL-2.0-or-later

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <Status.hpp>
#include "TypesHelper.hpp"
#include <memory>
#include <algorithm>
#include <cctype>

#include "JsonConverter.hpp"

using namespace testing;

namespace netconf {

class JsonConverterIPConfigTest : public Test {
 public:
  ::std::shared_ptr<JsonConverter> parser_;

  JsonConverterIPConfigTest() {
  }

  void SetUp() override {
    parser_ = ::std::make_shared<JsonConverter>();
  }

  void TearDown() override {

  }

  IPConfigs one_ip_config_ { { "br0", IPSource::STATIC, "192.168.1.17", "255.255.255.0" } };
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

  IPConfigs two_ip_config_ { { "br0", IPSource::STATIC, "192.168.1.17", "255.255.255.0"}, { "br1", IPSource::STATIC, "192.168.2.17", "255.255.0.0"} };

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

  IPConfigs one_ip_config_empty_elements_ { {} };

  ::std::string const json_one_ip_config_empty_elements_ =
      R"(
  {
    "": {
      "bcast": "0.0.0.0",
      "ipaddr": "0.0.0.0",
      "netmask": "0.0.0.0",
      "source": "none"
    }
  }
   )";

};

static std::string& RemoveWhitespaces(std::string& str){
  auto f = [](unsigned char const c) { return std::isspace(c); };
  str.erase(std::remove_if(str.begin(), str.end(), f), str.end());
  return str;
}

static void ExpectStringEqIgnoreNewlineAndBlank(::std::string expected,
                                                ::std::string value) {

  RemoveWhitespaces(expected);
  RemoveWhitespaces(value);

  EXPECT_EQ(expected, value);
}

// Convert IP configuration tests
TEST_F(JsonConverterIPConfigTest, ParsesJsonContainingOneConfigToIPConfig) {

  IPConfigs configs;
  Status status = parser_->FromJsonString(json_one_ip_config_, configs);

  EXPECT_TRUE(configs[0] == one_ip_config_[0]);
  ASSERT_EQ(StatusCode::OK, status.Get());
}

TEST_F(JsonConverterIPConfigTest, ParsesJsonContainingTwoConfigToIPConfig) {

  IPConfigs configs;
  Status status = parser_->FromJsonString(json_two_ip_config_, configs);

  EXPECT_TRUE(configs[0] == two_ip_config_[0]);
  EXPECT_TRUE(configs[1] == two_ip_config_[1]);
  ASSERT_EQ(StatusCode::OK, status.Get());
}

TEST_F(JsonConverterIPConfigTest, ParsesJsonContainingEmptyElements) {

  IPConfigs configs;
  Status status = parser_->FromJsonString(json_one_ip_config_empty_elements_, configs);

  EXPECT_TRUE(configs[0] == one_ip_config_empty_elements_[0]);
  ASSERT_EQ(StatusCode::OK, status.Get());
}

TEST_F(JsonConverterIPConfigTest, ParsesOneIPConfigToJson) {

  auto json = parser_->ToJsonString(one_ip_config_);

  ExpectStringEqIgnoreNewlineAndBlank(json_one_ip_config_, json);
}

TEST_F(JsonConverterIPConfigTest, ParsesTwoToJsonString) {

  auto json = parser_->ToJsonString(two_ip_config_);

  ExpectStringEqIgnoreNewlineAndBlank(json_two_ip_config_, json);
}

TEST_F(JsonConverterIPConfigTest, ParsesToJsonStringInCaseOfMissingInterface) {

  auto json = parser_->ToJsonString(one_ip_config_empty_elements_);

  ExpectStringEqIgnoreNewlineAndBlank(json_one_ip_config_empty_elements_, json);
}

}
