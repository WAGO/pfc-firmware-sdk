// SPDX-License-Identifier: LGPL-3.0-or-later

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <algorithm>
#include <cctype>
#include <memory>

#include "JsonConverter.hpp"
#include "Status.hpp"
#include "TypesHelper.hpp"

using namespace testing;

namespace netconf {

class JsonConverterDynamicIPEventTest : public Test {
 public:
  ::std::shared_ptr<JsonConverter> parser_;

  JsonConverterDynamicIPEventTest() {
  }

  void SetUp() override {
    parser_ = ::std::make_shared<JsonConverter>();
  }

  void TearDown() override {
  }
};

TEST_F(JsonConverterDynamicIPEventTest, ParsesRenewAction) {
  ::std::string const json_dynamic_ip_event = R"(
  {
    "interface": "br0",
    "action": "renew"
  }
   )";

  ::std::string itf_name;
  DynamicIPEventAction action;
  Status status = parser_->FromJsonString(json_dynamic_ip_event, itf_name, action);

  EXPECT_EQ(itf_name, "br0");
  EXPECT_EQ(action, DynamicIPEventAction::RENEW);
  ASSERT_EQ(StatusCode::OK, status.GetStatusCode());
}

TEST_F(JsonConverterDynamicIPEventTest, ParsesUnknowAction) {
  ::std::string const json_dynamic_ip_event = R"(
    {
      "interface": "br0",
      "action": "xxx"
    }
     )";

  ::std::string itf_name;
  DynamicIPEventAction action;
  Status status = parser_->FromJsonString(json_dynamic_ip_event, itf_name, action);

  EXPECT_EQ(itf_name, "br0");
  EXPECT_EQ(action, DynamicIPEventAction::UNKNOWN);
  ASSERT_EQ(StatusCode::OK, status.GetStatusCode());
}

TEST_F(JsonConverterDynamicIPEventTest, ParsesIncorrectJsonString) {
  ::std::string const json_dynamic_ip_event = R"(
    {
      "interface": "br0
      "action": "xxx"
    }
     )";

  ::std::string itf_name;
  DynamicIPEventAction action;
  Status status = parser_->FromJsonString(json_dynamic_ip_event, itf_name, action);

  EXPECT_EQ(itf_name, "");
  EXPECT_EQ(action, DynamicIPEventAction::UNKNOWN);
  ASSERT_EQ(StatusCode::JSON_CONVERT, status.GetStatusCode());
}

}  // namespace netconf
