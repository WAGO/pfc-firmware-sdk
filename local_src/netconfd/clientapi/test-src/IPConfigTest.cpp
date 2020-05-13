#include <gtest/gtest.h>

#include <string>

#include <nlohmann/json.hpp>
#include "IPConfig.hpp"

namespace netconf {

using nlohmann::json;

class IPConfigTest : public testing::Test {};

TEST_F(IPConfigTest, AddIpConfig) {
  IPConfigs ip_configs{};

  IPConfig expected{"itf", IPSource::STATIC, "192.168.1.1", "255.255.255.0", "192.168.1.255"};
  ip_configs.AddIPConfig(expected);

  auto actual = ip_configs.GetIPConfig("itf");

  EXPECT_EQ(expected, *actual);
}

TEST_F(IPConfigTest, ReplaceIpConfig) {
  IPConfigs ip_configs{};

  IPConfig existing{"itf", IPSource::STATIC, "192.168.1.1", "255.255.255.0", "192.168.1.255"};
  ip_configs.AddIPConfig(existing);
  IPConfig expected{"itf", IPSource::DHCP, "192.168.2.1", "255.255.255.0", "192.168.2.255"};
  ip_configs.AddIPConfig(expected);

  auto actual = ip_configs.GetIPConfig("itf");

  EXPECT_EQ(expected, *actual);
}

TEST_F(IPConfigTest, RemoveIpConfig) {
  IPConfigs ip_configs{};

  IPConfig existing{"itf", IPSource::STATIC, "192.168.1.1", "255.255.255.0", "192.168.1.255"};
  ip_configs.AddIPConfig(existing);

  ip_configs.RemoveIPConfig("itf");

  IPConfig expected{};
  auto actual = ip_configs.GetIPConfig("itf");

  EXPECT_FALSE(actual);
}

TEST_F(IPConfigTest, CreateFromJsonString) {
  auto json_str = R"({ "itf" : { "source": "dhcp" }})";
  IPConfigs ip_configs{json_str};

  auto actual = ip_configs.GetIPConfig("itf");
  IPConfig expected{"itf", IPSource::DHCP, "", "", ""};

  EXPECT_EQ(expected, actual);
}

TEST_F(IPConfigTest, ToJson) {
  auto json_str = R"({ "itf" : {  "ipaddr": "", "netmask": "", "bcast" :"", "source": "dhcp"  }})";
  auto expected = json::parse(json_str);

  IPConfigs ip_configs{json_str};

  auto actual_str = ip_configs.ToJson();

  auto actual = json::parse(actual_str);

  EXPECT_EQ(expected, actual);
}

}  // namespace netconf
