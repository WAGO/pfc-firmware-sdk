// SPDX-License-Identifier: GPL-2.0-or-later

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "DeviceType.hpp"

namespace netconf {


TEST(DeviceTypeTest, Subtract) {
  DeviceType orig = DeviceType::Virtual |DeviceType::Bridge;
  orig -= DeviceType::Bridge;

  EXPECT_EQ(DeviceType::Virtual, orig);
}

TEST(DeviceTypeTest, LogicalAnd) {
  EXPECT_TRUE((DeviceType::Virtual |DeviceType::Bridge) && DeviceType::Virtual);
  EXPECT_TRUE(DeviceType::Virtual && (DeviceType::Virtual |DeviceType::Bridge));
  EXPECT_FALSE((DeviceType::Virtual |DeviceType::Bridge) && DeviceType::Ethernet);
  EXPECT_FALSE(DeviceType::Bridge && DeviceType::Ethernet);
  EXPECT_TRUE(DeviceType::Ethernet && DeviceType::Ethernet);
}

using Conversions = ::std::pair<::std::string, DeviceType>;
class DeviceTypeConversion : public ::testing::Test, public ::testing::WithParamInterface<Conversions> {
 public:

  std::string str;
  DeviceType type;

  void SetUp() override {
    ::std::tie(str, type) = GetParam();
  }

};

INSTANTIATE_TEST_CASE_P(Instance, DeviceTypeConversion, ::testing::Values(
    Conversions{"bridge", DeviceType::Bridge},
    Conversions{"wwan", DeviceType::Wwan},
    Conversions{"virtual", DeviceType::Virtual},
    Conversions{"port", DeviceType::Port},
    Conversions{"loopback", DeviceType::Loopback},
    Conversions{"ethernet", DeviceType::Ethernet},
    Conversions{"other", DeviceType::Other},
    Conversions{"bridge|wwan|port", DeviceType::Bridge|DeviceType::Wwan|DeviceType::Port},
    Conversions{"", static_cast<DeviceType>(0)}
));

TEST_P(DeviceTypeConversion, DeviceTypeFromString) {
  auto result = DeviceTypeFromString(str);
  EXPECT_EQ(type, result);
}

TEST_P(DeviceTypeConversion, ToString) {
  auto result = ToString(type);
  EXPECT_EQ(str, result);
}


}  // namespace netconf
