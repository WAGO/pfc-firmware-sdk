// SPDX-License-Identifier: LGPL-3.0-or-later

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <string>

#include "InterfaceConfig.hpp"

namespace netconf {
namespace api {


TEST(GetMacAddress_Target, GetMac){

  MacAddress mac = GetMacAddress("br0");

  EXPECT_THAT(mac.ToString(), ::testing::StartsWith("00:30:DE:"));

}

}  // namespace api
}  // namespace netconf
