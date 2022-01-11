

#include <gtest/gtest.h>
#include "NetworkConfiguratorSettings.hpp"
#include "BaseTypes.hpp"


namespace netconf {

TEST(NetworkConfiguratorSettingsTest, StartWithPortstateEnumMustMatchInterfaceStateEnum){

  /* Ensure the enum values are equal,
   * so we can simply use a static_cast when mapping one to the other. */
  EXPECT_EQ(StartWithPortstate::Normal, static_cast<StartWithPortstate>(InterfaceState::UNKNOWN));
  EXPECT_EQ(StartWithPortstate::Down, static_cast<StartWithPortstate>(InterfaceState::DOWN));
  EXPECT_EQ(StartWithPortstate::Up, static_cast<StartWithPortstate>(InterfaceState::UP));

}

}  // namespace netconf
