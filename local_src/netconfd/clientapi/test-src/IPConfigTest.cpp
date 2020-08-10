// SPDX-License-Identifier: GPL-2.0-or-later
#include <gtest/gtest.h>

#include <string>

#include <nlohmann/json.hpp>
#include "IPConfig.hpp"
#include "JsonConverter.hpp"

namespace netconf {
namespace api {

using nlohmann::json;

class IPConfigTest : public testing::Test {};

TEST_F(IPConfigTest, AddIpConfig) {
  IPConfigs ip_configs{};

  IPConfig expected{"itf", IPSource::STATIC, "192.168.1.1", "255.255.255.0"};
  ip_configs.AddIPConfig(expected);

  auto actual = ip_configs.GetIPConfig("itf");

  EXPECT_EQ(expected, *actual);
}

TEST_F(IPConfigTest, ReplaceIpConfig) {
  IPConfigs ip_configs{};

  IPConfig existing{"itf", IPSource::STATIC, "192.168.1.1", "255.255.255.0"};
  ip_configs.AddIPConfig(existing);
  IPConfig expected{"itf", IPSource::DHCP, "192.168.2.1", "255.255.255.0"};
  ip_configs.AddIPConfig(expected);

  auto actual = ip_configs.GetIPConfig("itf");

  EXPECT_EQ(expected, *actual);
}

TEST_F(IPConfigTest, RemoveIpConfig) {
  IPConfigs ip_configs{};

  IPConfig existing{"itf", IPSource::STATIC, "192.168.1.1", "255.255.255.0"};
  ip_configs.AddIPConfig(existing);

  ip_configs.RemoveIPConfig("itf");

  IPConfig expected{};
  auto actual = ip_configs.GetIPConfig("itf");

  EXPECT_FALSE(actual);
}

TEST_F(IPConfigTest, CreateFromJsonString) {
  auto ip_configs = MakeIPConfigs(R"({ "itf" : { "source": "dhcp" }})");

  auto actual = ip_configs.GetIPConfig("itf");
  IPConfig expected{"itf", IPSource::DHCP, ZeroIP, ZeroIP};

  EXPECT_EQ(expected, actual);
}

TEST_F(IPConfigTest, ToJson) {
  auto json_str = R"({ "itf" : {  "ipaddr": "", "netmask": "", "bcast" :"", "source": "dhcp"  }})";
  auto expected = json::parse(json_str);

  auto ip_configs = MakeIPConfigs(json_str);

  JsonConverter jc;
  auto actual_str = jc.ToJsonString(ip_configs.GetConfig());

  auto actual = json::parse(actual_str);

  EXPECT_EQ(expected.dump(), actual.dump());
}


TEST(IPSource, ToString) {
  using namespace std::literals;
  EXPECT_EQ("dhcp"s, ToString(IPSource::DHCP));
  EXPECT_EQ("static", ToString(IPSource::STATIC));
  EXPECT_EQ("bootp", ToString(IPSource::BOOTP));
}

TEST_F(IPConfigTest, CalculateBroadcast) {
  IPConfig ipcfg{"br123", IPSource::STATIC, "192.168.123.12", "255.255.255.0"};

  EXPECT_EQ("192.168.123.0", CalculateBroadcast(ipcfg));
  ipcfg.netmask_="255.255.0.0";
  EXPECT_EQ("192.168.0.0", CalculateBroadcast(ipcfg));


}


}  // namespace api
}  // namespace netconf
