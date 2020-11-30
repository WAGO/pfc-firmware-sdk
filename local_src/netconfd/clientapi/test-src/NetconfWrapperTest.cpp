// SPDX-License-Identifier: LGPL-3.0-or-later

#include <gtest/gtest.h>

#include <string>

#include <nlohmann/json.hpp>
#include "IPConfig.h"

TEST(Netconf_Target, GetDipSwitchConfig){
  uint32_t ip, mask;
  uint8_t value;

   auto status = NetconfGetDipSwitchConfig(&ip, &mask, &value);

   ASSERT_EQ(0, status);
}

TEST(Netconf_Target, SetDipSwitchConfig){
  uint32_t ip = 0x11223344, mask=0x00FFFFFF;

   auto status = NetconfSetDipSwitchConfig(ip, mask);

   ASSERT_EQ(0, status);
}
