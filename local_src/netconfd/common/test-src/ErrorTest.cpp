// SPDX-License-Identifier: LGPL-3.0-or-later

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "Error.hpp"

namespace netconf {

TEST(ErrorTest, AddsParameterToErrorMsg) {

  Error e { ErrorCode::SYSTEM_CALL, "bla" };
  EXPECT_EQ(e.ToString(), "System call error: bla");
}

TEST(ErrorTest, RemovesEmptyErrorMsgPlaceholder) {
  Error e { ErrorCode::SYSTEM_CALL };
  EXPECT_EQ(e.ToString(), "System call error: ");
}

TEST(ErrorTest, ProvidesErrorStatus) {
  Error e = Error::Ok();
  EXPECT_TRUE(e.IsOk());

  e = Error { ErrorCode::IPV4_FORMAT };
  EXPECT_TRUE(e.IsNotOk());
}

TEST(ErrorTest, ProvidesErrorInformation) {
  auto e = Error { ErrorCode::NETWORK_CONFLICT, "bla", "blub" };
  EXPECT_EQ(e.GetErrorCode(), ErrorCode::NETWORK_CONFLICT);

  auto parameter = e.GetParameter();
  EXPECT_TRUE(std::find(parameter.begin(), parameter.end(), "bla") != parameter.end());
  EXPECT_TRUE(std::find(parameter.begin(), parameter.end(), "blub") != parameter.end());
}

}  // namespace netconf
