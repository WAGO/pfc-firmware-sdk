#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "FixIP.hpp"
#include "TypesTestHelper.hpp"
namespace netconf {

using namespace testing;

struct FixIPTest : public ::testing::Test {

  const IPConfigs original_config {
    { "br0", IPSource::DHCP },
    { "br2", IPSource::STATIC, "10.20.30.40", "255.0.0.0" },
    { "br1", IPSource::NONE },
    { "br29", IPSource::BOOTP, "1.2.3.4", "255.224.0.0" },
    { "eth41", IPSource::STATIC, "99.99.99.99", "255.224.0.0" },
    {},
    { "noindex", IPSource::STATIC, "13.37.47.11", "255.224.0.0" },
  };

  const IPConfigs expected_config {
    { "br0", IPSource::FIXIP, "192.168.1.17", "255.255.255.0"},
    { "br2", IPSource::FIXIP, "192.168.3.17", "255.255.255.0" },
    // Order doesn't matter
    { "br1", IPSource::FIXIP, "192.168.2.17", "255.255.255.0"},
    { "br29", IPSource::FIXIP, "192.168.30.17", "255.255.255.0" },
    // Name prefix doesn't matter
    { "eth41", IPSource::FIXIP, "192.168.42.17", "255.255.255.0" },
    // Empty is untouched
    {},
    // Interfaces w/o index are not changed
    { "noindex", IPSource::STATIC, "13.37.47.11", "255.224.0.0" },
  };

};


TEST_F(FixIPTest, SetFixIPsAcoordingToInterfaceName){
  IPConfigs changed_config = original_config;
  SetFixedIps(changed_config);

  ASSERT_THAT(changed_config, Eq(expected_config));

}

}  // namespace netconf
