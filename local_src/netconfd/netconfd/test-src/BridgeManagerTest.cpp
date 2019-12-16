// SPDX-License-Identifier: GPL-2.0-or-later

#include "BridgeManager.hpp"
#include "MockIBridgeController.hpp"
#include "MockIDevicePropertiesProvider.hpp"
#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <memory>

using namespace testing;

namespace netconfd {

class InterfaceManagerTest : public Test {
 public:

  MockIBridgeController mock_bridge_controller_;
  MockIDevicePropertiesProvider mock_properties_provider_;
  Bridges any_bridges;
  Interfaces any_interfaces_of_br0;
  Interfaces any_interfaces_of_br1;

  void SetUp() override {
    any_bridges = Bridges { "br0", "br1" };
    any_interfaces_of_br0 = Interfaces { "ethX1" };
    any_interfaces_of_br1 = Interfaces { "ethX2", "ethX3" };

    EXPECT_CALL(mock_bridge_controller_, SetInterfaceUp(_)).WillOnce(Return(Status()));
    interface_manager_ = ::std::make_unique<BridgeManager>(mock_bridge_controller_, mock_properties_provider_);
  }

  void TearDown() override {

  }

  ::std::unique_ptr<BridgeManager> interface_manager_;
};

TEST_F(InterfaceManagerTest, GetBridgeOfInterface) {
  EXPECT_CALL(mock_bridge_controller_, GetBridges()).WillRepeatedly(Return(any_bridges));
  EXPECT_CALL(mock_bridge_controller_, GetBridgeInterfaces(Eq("br0"))).WillRepeatedly(Return(any_interfaces_of_br0));
  EXPECT_CALL(mock_bridge_controller_, GetBridgeInterfaces(Eq("br1"))).WillRepeatedly(Return(any_interfaces_of_br1));

  auto bridge = interface_manager_->GetBridgeOfInterface("ethX1");
  EXPECT_EQ("br0", bridge);

  bridge = interface_manager_->GetBridgeOfInterface("ethX3");
  EXPECT_EQ("br1", bridge);
}

} /* namespace netconfd */
