// SPDX-License-Identifier: LGPL-3.0-or-later

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <Status.hpp>

namespace netconf {

TEST(StatusTest, AddsParameterToStatusMsg) {

  Status e { StatusCode::SYSTEM_CALL, "bla" };
  EXPECT_EQ(e.ToString(), "System call error: bla");
}

TEST(StatusTest, RemovesEmptyStatusMsgPlaceholder) {
  Status e { StatusCode::SYSTEM_CALL };
  EXPECT_EQ(e.ToString(), "System call error: ");
}

TEST(StatusTest, ProvidesStatusStatus) {
  Status e = Status::Ok();
  EXPECT_TRUE(e.IsOk());

  e = Status { StatusCode::IPV4_FORMAT };
  EXPECT_TRUE(e.IsNotOk());
}

TEST(StatusTest, ProvidesStatusInformation) {
  auto e = Status { StatusCode::NETWORK_CONFLICT, "bla", "blub" };
  EXPECT_EQ(e.GetStatusCode(), StatusCode::NETWORK_CONFLICT);

  auto parameter = e.GetParameter();
  EXPECT_TRUE(std::find(parameter.begin(), parameter.end(), "bla") != parameter.end());
  EXPECT_TRUE(std::find(parameter.begin(), parameter.end(), "blub") != parameter.end());
}

TEST(StatusTest, AppendTextAfterEnd) {
  auto e = Status { StatusCode::NETWORK_CONFLICT, "bla", "blub" };

  e.Append("Additional Text");
  EXPECT_THAT(e.ToString(), ::testing::EndsWith(": Additional Text") );
}

}  // namespace netconf
