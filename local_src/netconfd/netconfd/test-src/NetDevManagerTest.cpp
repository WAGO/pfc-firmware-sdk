// SPDX-License-Identifier: GPL-2.0-or-later

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <MockIDeviceProperties.hpp>
#include "NetDevManager.hpp"
#include "MockIInterfaceMonitor.hpp"
#include "MockINetDevConstruction.hpp"
#include "MockIBridgeController.hpp"
#include "MockIMacDistributor.hpp"
#include <limits>
#include <memory>
#include <string.h>

using namespace testing;
namespace netconf {

class NetDevManagerTest : public ::testing::Test {
 public:

  MockIDeviceProperties mock_device_properties_;
  NiceMock<MockIInterfaceMonitor> mock_itf_monitor_;
  ::std::unique_ptr<NetDevManager> netdev_manager_;
  MockIBridgeController mock_bridge_controller_;
  ::std::shared_ptr<IInterfaceMonitor> shared_itf_monitor_;
  MockIMacDistributor mock_mac_distributor_;

  void SetUp() override {
    shared_itf_monitor_ = ::std::shared_ptr<IInterfaceMonitor>(&mock_itf_monitor_, [](IInterfaceMonitor*) {
    });

    Interfaces itfs = { "ethX1", "ethX2", "ethX3", "ethX4", "eth0" };

    EXPECT_CALL(mock_itf_monitor_, RegisterEventHandler(_));
    EXPECT_CALL(mock_bridge_controller_, GetInterfaces()).WillOnce(Return(itfs));

    netdev_manager_ = ::std::make_unique<NetDevManager>(shared_itf_monitor_, mock_bridge_controller_,
                                                        mock_mac_distributor_);

  }

  void TearDown() override {
//    netdev_manager_.reset();
  }

};

TEST_F(NetDevManagerTest, CreatesNetDevsForEachSystemInterface) {

  Interfaces itfs = { "ethX1", "ethX2" };

  EXPECT_CALL(mock_itf_monitor_, RegisterEventHandler(_));
  EXPECT_CALL(mock_bridge_controller_, GetInterfaces()).WillOnce(Return(itfs));
  NetDevManager netdev_manager { shared_itf_monitor_, mock_bridge_controller_, mock_mac_distributor_ };

  auto netdev = netdev_manager.GetByName("ethX1");

  EXPECT_NE(nullptr, netdev.get());
  EXPECT_EQ("ethX1", netdev->GetName());
  EXPECT_EQ("X1", netdev->GetLabel());

  netdev = netdev_manager.GetByName("ethX2");

  EXPECT_NE(nullptr, netdev.get());
  EXPECT_EQ("ethX2", netdev->GetName());
  EXPECT_EQ("X2", netdev->GetLabel());

}

TEST_F(NetDevManagerTest, ProvidesAllPortNetDevs) {

  Interfaces itfs = { "eth0", "ethX3" };

  EXPECT_CALL(mock_itf_monitor_, RegisterEventHandler(_));
  EXPECT_CALL(mock_bridge_controller_, GetInterfaces()).WillOnce(Return(itfs));
  NetDevManager netdev_manager { shared_itf_monitor_, mock_bridge_controller_, mock_mac_distributor_ };

  auto netdevs = netdev_manager.GetPortNetDevs();

  EXPECT_EQ(1, netdevs.size());
  EXPECT_EQ("ethX3", netdevs.front()->GetName());

}

TEST_F(NetDevManagerTest, DoesNotCopyNetdevObjects) {

  Interfaces itfs = { "ethX1" };

  EXPECT_CALL(mock_itf_monitor_, RegisterEventHandler(_));
  EXPECT_CALL(mock_bridge_controller_, GetInterfaces()).WillOnce(Return(itfs));
  NetDevManager netdev_manager { shared_itf_monitor_, mock_bridge_controller_, mock_mac_distributor_ };

  auto get_by_label = netdev_manager.GetByLabel("X1");
  auto get_ethernet = netdev_manager.GetPortNetDevs().front();

  EXPECT_EQ(get_by_label.get(), get_ethernet.get());
  EXPECT_EQ(3, get_by_label.use_count());
}

TEST_F(NetDevManagerTest, CreateBridgeAndInterfaceRelations) {

  BridgeConfig config = { { "br0", { "X1" } } };

  netdev_manager_->ConfigureBridges(config);

  auto bridge = netdev_manager_->GetByName("br0");
  ASSERT_TRUE(bridge);

  auto x1 = netdev_manager_->GetByLabel("X1");
  ASSERT_EQ(1, bridge->GetChildren().size());
  EXPECT_EQ(x1, bridge->GetChildren().front());

  EXPECT_EQ(bridge, x1->GetParent());
}

TEST_F(NetDevManagerTest, AddInterfaceRelationsToBridge) {

  BridgeConfig config1 = { { "br0", { "X1" } } };

  netdev_manager_->ConfigureBridges(config1);

  BridgeConfig config2 = { { "br0", { "X1", "X2" } } };

  netdev_manager_->ConfigureBridges(config2);

  auto bridge = netdev_manager_->GetByName("br0");
  ASSERT_TRUE(bridge);

  auto x1 = netdev_manager_->GetByLabel("X1");
  auto x2 = netdev_manager_->GetByLabel("X2");
  ASSERT_EQ(2, bridge->GetChildren().size());
  EXPECT_EQ(x1, *std::find(bridge->GetChildren().begin(), bridge->GetChildren().end(), x1));
  EXPECT_EQ(x2, *std::find(bridge->GetChildren().begin(), bridge->GetChildren().end(), x2));

  EXPECT_EQ(bridge, x1->GetParent());
  EXPECT_EQ(bridge, x2->GetParent());

}

TEST_F(NetDevManagerTest, RemoveBridgesAndInterfaceRelations) {

  BridgeConfig config1 = { { "br0", { "X1" } } };

  netdev_manager_->ConfigureBridges(config1);

  BridgeConfig config2 { };

  netdev_manager_->ConfigureBridges(config2);

  auto x1 = netdev_manager_->GetByLabel("X1");
  EXPECT_FALSE(x1->GetParent());
  EXPECT_FALSE(netdev_manager_->GetByName("br0"));
}

TEST_F(NetDevManagerTest, RemoveInterfaceFromBridgeAndResetRelations) {

  BridgeConfig config1 = { { "br0", { "X1", "X2" } } };

  netdev_manager_->ConfigureBridges(config1);

  BridgeConfig config2 = { { "br0", { "X2" } } };

  netdev_manager_->ConfigureBridges(config2);

  auto bridge = netdev_manager_->GetByName("br0");
  ASSERT_TRUE(bridge);

  auto x1 = netdev_manager_->GetByLabel("X1");
  auto x2 = netdev_manager_->GetByLabel("X2");
  ASSERT_EQ(1, bridge->GetChildren().size());
  EXPECT_EQ(bridge->GetChildren().end(), std::find(bridge->GetChildren().begin(), bridge->GetChildren().end(), x1));
  EXPECT_EQ(x2, *std::find(bridge->GetChildren().begin(), bridge->GetChildren().end(), x2));

  EXPECT_FALSE(x1->GetParent());
  EXPECT_EQ(bridge, x2->GetParent());
}

ACTION_P(SaveNetDevName, name){
name.get() = arg0->GetName();
}

TEST_F(NetDevManagerTest, InformsRegisterdClientsAboutBridgeConstruction) {

  MockINetDevConstruction construction_event;

  netdev_manager_->RegisterForNetDevConstructionEvents(construction_event);

  ::std::string contructed_netdev_name;
  EXPECT_CALL(construction_event, OnNetDevCreated(_)).WillOnce(SaveNetDevName(::std::ref(contructed_netdev_name)));

  BridgeConfig config = { { "br0", { "X2" } } };
  netdev_manager_->ConfigureBridges(config);

  EXPECT_EQ("br0", contructed_netdev_name);

}

}
