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

  auto status = SetInterfaceState(interface,  InterfaceState::DOWN);
  EXPECT_TRUE(status.IsOk());
  status = GetInterfaceState(interface,  current_state);
  EXPECT_TRUE(status.IsOk());
  EXPECT_EQ(InterfaceState::DOWN, current_state);

  status = SetInterfaceState(interface,  InterfaceState::UP);
  EXPECT_TRUE(status.IsOk());
  status = GetInterfaceState(interface,  current_state);
  EXPECT_TRUE(status.IsOk());
  EXPECT_EQ(InterfaceState::UP, current_state);


}

TEST(InterfaceConfigsTest, ArrayIndexOperator)
{
  InterfaceConfigs c;

  auto& ic = c["X42"];
  EXPECT_EQ(ic.device_name_, "X42");
  EXPECT_EQ(ic.speed_, -1);
  ic.speed_ = 1337;

  auto& ic2 = c["X42"];
  EXPECT_EQ(ic2.device_name_, "X42");
  EXPECT_EQ(ic2.speed_, 1337);

  const auto& const_c = c;
  EXPECT_THROW(const_c["X43"], std::out_of_range);
  EXPECT_NO_THROW(const_c["X42"]);

}


}  // namespace api
}  // namespace netconf
