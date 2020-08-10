// SPDX-License-Identifier: GPL-2.0-or-later
#include <gtest/gtest.h>

#include <string>

#include <nlohmann/json.hpp>
#include "InterfaceConfig.hpp"

namespace netconf {
namespace api {

using nlohmann::json;

TEST(InterfaceConfigsTest, MacAddressToString)
{
  const uint8_t macbytes[] = {0x01,0x02,0x03,0x04,0x05,0x06};
  MacAddress mac(macbytes);

  EXPECT_EQ("01:02:03:04:05:06", mac.ToString());
  EXPECT_EQ("01-02-03-04-05-06", mac.ToString('-'));
}

}  // namespace api
}  // namespace netconf
