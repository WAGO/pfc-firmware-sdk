// SPDX-License-Identifier: LGPL-3.0-or-later

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


TEST(InterfaceConfigsTest_Target, InterfaceUpDownTest){
  const ::std::string interface = "ethX2";
  InterfaceState current_state;

  auto error = SetInterfaceState(interface,  InterfaceState::DOWN);
  EXPECT_TRUE(error.IsOk());
  error = GetInterfaceState(interface,  current_state);
  EXPECT_TRUE(error.IsOk());
  EXPECT_EQ(InterfaceState::DOWN, current_state);

  error = SetInterfaceState(interface,  InterfaceState::UP);
  EXPECT_TRUE(error.IsOk());
  error = GetInterfaceState(interface,  current_state);
  EXPECT_TRUE(error.IsOk());
  EXPECT_EQ(InterfaceState::UP, current_state);


}

}  // namespace api
}  // namespace netconf
