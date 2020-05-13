#include <gtest/gtest.h>

#include <string>

#include <nlohmann/json.hpp>
#include "InterfaceConfig.hpp"

namespace netconf
{

  using nlohmann::json;

  class InterfaceConfigTest: public testing::Test
  {
  };

  TEST_F(InterfaceConfigTest, AddInterfaceConfig)
  {
    InterfaceConfigs config{
        R"([{"autonegotiation":"on","device":"X1","state":"up"},{"autonegotiation":"on","device":"X12","state":"up"},{"autonegotiation":"on","device":"X2","state":"up"},{"autonegotiation":"on","device":"X11","state":"up"}])"};
  }

  TEST_F(InterfaceConfigTest, InterfaceConfigToString)
  {
    InterfaceConfigs config{
        R"([{"device":"X1","state":"up","autonegotiation":"on"}])"};

    ::std::string expected = "device=X1 state=up autonegotiation=on ";

    ::std::string actual = config.ToString();

    EXPECT_EQ(expected, actual);
  }

}  // namespace netconf
