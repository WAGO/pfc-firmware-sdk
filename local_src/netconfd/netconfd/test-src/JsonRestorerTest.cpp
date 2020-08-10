
#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <utility>


#include "JsonRestorer.hpp"
#include "Types.hpp"

using namespace netconf;

TEST(JsonRestorer, restoreInterfaceConfig) {
  auto bfc =
R"({"bridge-config":{"br0":["X1"],"br1":["X2"]},
"ip-config":{"br0":{"source":"static","ipaddr":"192.168.2.17","netmask":"255.255.255.0","bcast":"192.168.2.255"},"br1":{"source":"dhcp","ipaddr":"172.29.233.17","netmask":"255.255.0.0","bcast":"172.29.255.255"}},
"interface-config":{"autonegotiation":true,"device":"X1","duplex":"full","speed":100,"state":"up"}})";

  auto restorer = JsonRestorer{std::move(bfc) };

  InterfaceConfigs configs;
  restorer.Restore("interface-config", configs);

  EXPECT_EQ(1U, configs.size());
}

