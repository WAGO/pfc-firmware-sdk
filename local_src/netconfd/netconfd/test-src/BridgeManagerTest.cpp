// SPDX-License-Identifier: GPL-2.0-or-later

#include "BridgeManager.hpp"
#include "INetDevConstruction.hpp"
#include "MockIBridgeController.hpp"
#include "MockINetDevManager.hpp"
#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <MockIDeviceProperties.hpp>
#include <memory>

using namespace testing;

namespace netconf {

class InterfaceManagerTest : public Test {
 public:

  MockIBridgeController mock_bridge_controller_;
  MockIDeviceProperties mock_properties_provider_;
  MockINetDevManager mock_netdev_manager_;
  Bridges any_bridges;
  Interfaces any_interfaces_of_br0;
  Interfaces any_interfaces_of_br1;

  void SetUp() override {
    any_bridges = Bridges { "br0", "br1" };
    any_interfaces_of_br0 = Interfaces { "ethX1" };
    any_interfaces_of_br1 = Interfaces { "ethX2", "ethX3" };

    EXPECT_CALL(mock_bridge_controller_, SetInterfaceUp(_)).WillOnce(Return(Error()));
    interface_manager_ = ::std::make_unique<BridgeManager>(mock_bridge_controller_, mock_properties_provider_,
                                                           mock_netdev_manager_);
  }

  void TearDown() override {

  }

  ::std::unique_ptr<BridgeManager> interface_manager_;
};


} /* namespace netconfd */
