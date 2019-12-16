// SPDX-License-Identifier: GPL-2.0-or-later
//------------------------------------------------------------------------------
///  \file     ABridgeConfigurator.cpp
///
///  \brief    <short description of the file contents>
///
///  \author   <author> : WAGO Kontakttechnik GmbH & Co. KG
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// include files
//------------------------------------------------------------------------------
#include "BridgeConfigurator.hpp"

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <memory>

#include "MockIBridgeController.hpp"
#include "MockIMacDistributor.hpp"

#include "Helper.hpp"

using namespace testing;
using ::testing::Sequence;

namespace netconfd {

class ABridgeConfigurator : public Test {
 public:
  StrictMock<MockIBridgeController> mock_bridge_controller_;
  StrictMock<MockIMacDistributor> mock_mac_distributor_;
  ::std::shared_ptr<BridgeConfigurator> bridge_configurator_;

  ABridgeConfigurator() {

    bridge_configurator_ = ::std::make_shared<BridgeConfigurator>(mock_bridge_controller_,
                                                                  mock_mac_distributor_);
  }

  void SetUp() override {

  }

  void TearDown() override {

  }

};

TEST_F(ABridgeConfigurator, SuccessfullyConfiguresABridge) {

  BridgeConfig target_config;
  Bridges actual_bridges;

  EXPECT_CALL(mock_bridge_controller_, GetBridges()).WillRepeatedly(
      Return(actual_bridges));

  Status status = bridge_configurator_->Configure(target_config);

  ASSERT_EQ(StatusCode::OK, status.Get());
}

TEST_F(ABridgeConfigurator, AddsOneBridgeAndOneCorrespondingInterface) {

  BridgeConfig target_config { { "br0", { "ethX1" } } };
  Bridges actual_bridges;

  EXPECT_CALL(mock_bridge_controller_, GetBridges()).WillRepeatedly(
      Return(actual_bridges));

  EXPECT_CALL(mock_bridge_controller_, AddBridge(_)).WillOnce(Return(Status(StatusCode::OK)));
  EXPECT_CALL(mock_mac_distributor_, SetMac(Eq("br0"))).WillOnce(
      Return(Status(StatusCode::OK)));
  EXPECT_CALL(mock_bridge_controller_, AddInterface(_,_)).WillOnce(
      Return(Status(StatusCode::OK)));
  EXPECT_CALL(mock_bridge_controller_, SetInterfaceUp("ethX1")).WillOnce(
      Return(Status(StatusCode::OK)));

  Status status = bridge_configurator_->Configure(target_config);
  ASSERT_EQ(StatusCode::OK, status.Get());
}

TEST_F(ABridgeConfigurator, TriesToAddsOneBridgeWhileSetMacFailed) {

  BridgeConfig target_config { { "br0", { } } };
  ;
  Bridges actual_bridges;

  EXPECT_CALL(mock_bridge_controller_, GetBridges()).WillRepeatedly(
      Return(actual_bridges));

  EXPECT_CALL(mock_bridge_controller_, AddBridge(_)).WillOnce(Return(Status(StatusCode::OK)));
  EXPECT_CALL(mock_mac_distributor_, SetMac(Eq("br0"))).WillOnce(
      Return(Status(StatusCode::ERROR)));

  Status status = bridge_configurator_->Configure(target_config);
  EXPECT_EQ(StatusCode::ERROR, status.Get());
}

TEST_F(ABridgeConfigurator, AddsOneBridgeAndFourCorrespondingInterface) {

  BridgeConfig target_config { { "br0", { "ethX1", "ethX2", "ethX11", "ethX12" } } };
  Bridges actual_bridges;

  EXPECT_CALL(mock_bridge_controller_, GetBridges()).WillRepeatedly(
      Return(actual_bridges));

  EXPECT_CALL(mock_bridge_controller_, AddBridge(Eq("br0"))).WillOnce(
      Return(Status(StatusCode::OK)));
  EXPECT_CALL(mock_mac_distributor_, SetMac(Eq("br0"))).WillOnce(
      Return(Status(StatusCode::OK)));
  EXPECT_CALL(mock_bridge_controller_, AddInterface(Eq("br0"),Eq("ethX1"))).WillOnce(
      Return(Status(StatusCode::OK)));
  EXPECT_CALL(mock_bridge_controller_, AddInterface(Eq("br0"),Eq("ethX2"))).WillOnce(
      Return(Status(StatusCode::OK)));
  EXPECT_CALL(mock_bridge_controller_, AddInterface(Eq("br0"),Eq("ethX11"))).WillOnce(
      Return(Status(StatusCode::OK)));
  EXPECT_CALL(mock_bridge_controller_, AddInterface(Eq("br0"),Eq("ethX12"))).WillOnce(
      Return(Status(StatusCode::OK)));

  EXPECT_CALL(mock_bridge_controller_, SetInterfaceUp("ethX1")).WillOnce(
      Return(Status(StatusCode::OK)));
  EXPECT_CALL(mock_bridge_controller_, SetInterfaceUp("ethX2")).WillOnce(
      Return(Status(StatusCode::OK)));
  EXPECT_CALL(mock_bridge_controller_, SetInterfaceUp("ethX11")).WillOnce(
      Return(Status(StatusCode::OK)));
  EXPECT_CALL(mock_bridge_controller_, SetInterfaceUp("ethX12")).WillOnce(
      Return(Status(StatusCode::OK)));

  Status status = bridge_configurator_->Configure(target_config);
  ASSERT_EQ(StatusCode::OK, status.Get());
}

TEST_F(ABridgeConfigurator, AddsTwoBridgesWithTwoCorrespondingInterfaces) {

  BridgeConfig target_config { { "br0", { "ethX1", "ethX2" } }, { "br1", { "ethX11",
      "ethX12" } } };
  Bridges actual_bridges;

  EXPECT_CALL(mock_bridge_controller_, GetBridges()).WillRepeatedly(
      Return(actual_bridges));

  EXPECT_CALL(mock_bridge_controller_, AddBridge(Eq("br0"))).WillOnce(
      Return(Status(StatusCode::OK)));
  EXPECT_CALL(mock_bridge_controller_, AddBridge(Eq("br1"))).WillOnce(
      Return(Status(StatusCode::OK)));
  EXPECT_CALL(mock_mac_distributor_, SetMac(Eq("br0"))).WillOnce(
      Return(Status(StatusCode::OK)));
  EXPECT_CALL(mock_mac_distributor_, SetMac(Eq("br1"))).WillOnce(
      Return(Status(StatusCode::OK)));
  EXPECT_CALL(mock_bridge_controller_, AddInterface(Eq("br0"),Eq("ethX1"))).WillOnce(
      Return(Status(StatusCode::OK)));
  EXPECT_CALL(mock_bridge_controller_, AddInterface(Eq("br0"),Eq("ethX2"))).WillOnce(
      Return(Status(StatusCode::OK)));
  EXPECT_CALL(mock_bridge_controller_, AddInterface(Eq("br1"),Eq("ethX11"))).WillOnce(
      Return(Status(StatusCode::OK)));
  EXPECT_CALL(mock_bridge_controller_, AddInterface(Eq("br1"),Eq("ethX12"))).WillOnce(
      Return(Status(StatusCode::OK)));

  EXPECT_CALL(mock_bridge_controller_, SetInterfaceUp("ethX1")).WillOnce(
      Return(Status(StatusCode::OK)));
  EXPECT_CALL(mock_bridge_controller_, SetInterfaceUp("ethX2")).WillOnce(
      Return(Status(StatusCode::OK)));
  EXPECT_CALL(mock_bridge_controller_, SetInterfaceUp("ethX11")).WillOnce(
      Return(Status(StatusCode::OK)));
  EXPECT_CALL(mock_bridge_controller_, SetInterfaceUp("ethX12")).WillOnce(
      Return(Status(StatusCode::OK)));

  Status status = bridge_configurator_->Configure(target_config);
  ASSERT_EQ(StatusCode::OK, status.Get());

}

TEST_F(ABridgeConfigurator, ModifyOneBridgeByAddingOneInterface) {

  BridgeConfig target_config { { "br0", { "ethX1", "ethX2" } } };
  Bridges actual_bridges = { "br0" };
  Interfaces actual_bridges_interfaces = { "ethX1" };

  EXPECT_CALL(mock_bridge_controller_, GetBridges()).WillRepeatedly(
      Return(actual_bridges));

  EXPECT_CALL(mock_bridge_controller_, GetBridgeInterfaces(Eq("br0"))).WillRepeatedly(
      Return(actual_bridges_interfaces));

  EXPECT_CALL(mock_bridge_controller_, DeleteInterface(_,_)).Times(0);

  EXPECT_CALL(mock_bridge_controller_, AddInterface(Eq("br0"),Eq("ethX2"))).WillOnce(
      Return(Status(StatusCode::OK)));

  EXPECT_CALL(mock_bridge_controller_, SetInterfaceUp("ethX2")).WillOnce(
      Return(Status(StatusCode::OK)));

  Status status = bridge_configurator_->Configure(target_config);
  ASSERT_EQ(StatusCode::OK, status.Get());

}

TEST_F(ABridgeConfigurator, ModifyOneBridgeByRemovingOneInterface) {

  BridgeConfig target_config { { "br0", { "ethX2" } } };
  Bridges actual_bridges = { "br0" };
  Interfaces actual_bridges_interfaces = { "ethX1", "ethX2" };

  EXPECT_CALL(mock_bridge_controller_, GetBridges()).WillRepeatedly(
      Return(actual_bridges));

  EXPECT_CALL(mock_bridge_controller_, GetBridgeInterfaces(Eq("br0"))).WillRepeatedly(
      Return(actual_bridges_interfaces));

  EXPECT_CALL(mock_bridge_controller_, DeleteInterface(Eq("br0"),Eq("ethX1"))).WillOnce(
      Return(Status(StatusCode::OK)));

  EXPECT_CALL(mock_bridge_controller_, AddInterface(_,_)).Times(0);

  EXPECT_CALL(mock_bridge_controller_, SetInterfaceDown("ethX1")).WillOnce(
      Return(Status(StatusCode::OK)));

  Status status = bridge_configurator_->Configure(target_config);
  ASSERT_EQ(StatusCode::OK, status.Get());
}

TEST_F(ABridgeConfigurator, ModifyOneBridgeByChangingOneInterface) {

  BridgeConfig target_config { { "br0", { "ethX2", "ethX11" } } };
  Bridges actual_bridges = { "br0" };
  Interfaces actual_bridges_interfaces = { "ethX1", "ethX2" };

  EXPECT_CALL(mock_bridge_controller_, GetBridges()).WillRepeatedly(
      Return(actual_bridges));

  EXPECT_CALL(mock_bridge_controller_, GetBridgeInterfaces(Eq("br0"))).WillRepeatedly(
      Return(actual_bridges_interfaces));

  EXPECT_CALL(mock_bridge_controller_, DeleteInterface(Eq("br0"),Eq("ethX1"))).WillOnce(
      Return(Status(StatusCode::OK)));

  EXPECT_CALL(mock_bridge_controller_, AddInterface(Eq("br0"),Eq("ethX11"))).WillOnce(
      Return(Status(StatusCode::OK)));

  EXPECT_CALL(mock_bridge_controller_, SetInterfaceDown("ethX1")).WillOnce(
      Return(Status(StatusCode::OK)));
  EXPECT_CALL(mock_bridge_controller_, SetInterfaceUp("ethX11")).WillOnce(
      Return(Status(StatusCode::OK)));

  Status status = bridge_configurator_->Configure(target_config);
  ASSERT_EQ(StatusCode::OK, status.Get());
}

TEST_F(ABridgeConfigurator, AddSecondBridgeAndThreeInterfaces) {

  BridgeConfig target_config { { "br0", { "ethX1", "ethX2" } }, { "br1", { "ethX11",
      "ethX12", "XUnmapped" } } };
  Bridges actual_bridges = { "br0" };
  Interfaces actual_bridges_interfaces = { "ethX1", "ethX2" };

  EXPECT_CALL(mock_bridge_controller_, GetBridges()).WillRepeatedly(
      Return(actual_bridges));

  EXPECT_CALL(mock_bridge_controller_, GetBridgeInterfaces(Eq("br0"))).WillRepeatedly(
      Return(actual_bridges_interfaces));

  EXPECT_CALL(mock_bridge_controller_, AddBridge(Eq("br1"))).WillOnce(
      Return(Status(StatusCode::OK)));
  EXPECT_CALL(mock_mac_distributor_, SetMac(Eq("br1"))).WillOnce(
      Return(Status(StatusCode::OK)));

  EXPECT_CALL(mock_bridge_controller_, DeleteInterface(_,_)).Times(0);

  EXPECT_CALL(mock_bridge_controller_, AddInterface(Eq("br1"),Eq("ethX11"))).WillOnce(
      Return(Status(StatusCode::OK)));
  EXPECT_CALL(mock_bridge_controller_, AddInterface(Eq("br1"),Eq("ethX12"))).WillOnce(
      Return(Status(StatusCode::OK)));
  EXPECT_CALL(mock_bridge_controller_, AddInterface(Eq("br1"),Eq("XUnmapped"))).WillOnce(
      Return(Status(StatusCode::OK)));

  EXPECT_CALL(mock_bridge_controller_, SetInterfaceUp("ethX11")).WillOnce(
      Return(Status(StatusCode::OK)));
  EXPECT_CALL(mock_bridge_controller_, SetInterfaceUp("ethX12")).WillOnce(
      Return(Status(StatusCode::OK)));
  EXPECT_CALL(mock_bridge_controller_, SetInterfaceUp("XUnmapped")).WillOnce(
      Return(Status(StatusCode::OK)));

  Status status = bridge_configurator_->Configure(target_config);
  ASSERT_EQ(StatusCode::OK, status.Get());
}

TEST_F(ABridgeConfigurator, RemoveOneBridgeAndCorrespondingInterfaces) {

  BridgeConfig target_config { { "br0", { "ethX1", "ethX2" } } };
  Bridges actual_bridges = { "br0", "br1" };
  Interfaces actual_br0_interfaces = { "ethX1", "ethX2" };
  Interfaces actual_br1_interfaces = { "ethX11", "ethX12", "XUnmapped" };

  EXPECT_CALL(mock_bridge_controller_, GetBridges()).WillRepeatedly(
      Return(actual_bridges));

  EXPECT_CALL(mock_bridge_controller_, GetBridgeInterfaces(Eq("br0"))).WillRepeatedly(
      Return(actual_br0_interfaces));

  EXPECT_CALL(mock_bridge_controller_, GetBridgeInterfaces(Eq("br1"))).WillRepeatedly(
      Return(actual_br1_interfaces));

  EXPECT_CALL(mock_bridge_controller_, AddBridge(_)).Times(0);

  EXPECT_CALL(mock_bridge_controller_, DeleteBridge(Eq("br1"))).WillOnce(
      Return(Status(StatusCode::OK)));

  EXPECT_CALL(mock_bridge_controller_, SetInterfaceDown("ethX11")).WillOnce(
      Return(Status(StatusCode::OK)));
  EXPECT_CALL(mock_bridge_controller_, SetInterfaceDown("ethX12")).WillOnce(
      Return(Status(StatusCode::OK)));
  EXPECT_CALL(mock_bridge_controller_, SetInterfaceDown("XUnmapped")).WillOnce(
      Return(Status(StatusCode::OK)));
  EXPECT_CALL(mock_bridge_controller_, SetInterfaceDown("br1")).WillOnce(
      Return(Status(StatusCode::OK)));

  Status status = bridge_configurator_->Configure(target_config);
  ASSERT_EQ(StatusCode::OK, status.Get());
}

TEST_F(ABridgeConfigurator, MoveOneInterfaceToAnotherBridge) {

  BridgeConfig target_config { { "br0", { "ethX1" } }, { "br1", { "ethX2", "ethX11" } } };
  Bridges actual_bridges = { "br0", "br1" };
  Interfaces actual_br0_interfaces = { "ethX1", "ethX2" };
  Interfaces actual_br1_interfaces = { "ethX11" };

  EXPECT_CALL(mock_bridge_controller_, GetBridges()).WillRepeatedly(
      Return(actual_bridges));

  EXPECT_CALL(mock_bridge_controller_, GetBridgeInterfaces(Eq("br0"))).WillRepeatedly(
      Return(actual_br0_interfaces));

  EXPECT_CALL(mock_bridge_controller_, GetBridgeInterfaces(Eq("br1"))).WillRepeatedly(
      Return(actual_br1_interfaces));

  EXPECT_CALL(mock_bridge_controller_, AddBridge(_)).Times(0);
  EXPECT_CALL(mock_bridge_controller_, DeleteBridge(_)).Times(0);

  EXPECT_CALL(mock_bridge_controller_, DeleteInterface(Eq("br0"),Eq("ethX2"))).WillOnce(
      Return(Status(StatusCode::OK)));
  EXPECT_CALL(mock_bridge_controller_, AddInterface(Eq("br1"),Eq("ethX2"))).WillOnce(
      Return(Status(StatusCode::OK)));

  EXPECT_CALL(mock_bridge_controller_, SetInterfaceDown("ethX2")).WillOnce(
      Return(Status(StatusCode::OK)));
  EXPECT_CALL(mock_bridge_controller_, SetInterfaceUp("ethX2")).WillOnce(
      Return(Status(StatusCode::OK)));

  Status status = bridge_configurator_->Configure(target_config);
  ASSERT_EQ(StatusCode::OK, status.Get());
}

TEST_F(ABridgeConfigurator, FirstSetsAnInterfaceUpBeforeAddingToBridge) {
  Sequence s1;

  BridgeConfig target_config { { "br0", { "ethX1" } } };
  Bridges actual_bridges;
  Interfaces actual_br0_interfaces;

  EXPECT_CALL(mock_bridge_controller_, GetBridges()).WillRepeatedly(
      Return(actual_bridges));
  EXPECT_CALL(mock_bridge_controller_, GetBridgeInterfaces(Eq("br0"))).WillRepeatedly(
      Return(actual_br0_interfaces));

  EXPECT_CALL(mock_bridge_controller_, AddBridge(Eq("br0"))).InSequence(s1);
  EXPECT_CALL(mock_mac_distributor_, SetMac(Eq("br0"))).WillOnce(
      Return(Status(StatusCode::OK)));

  EXPECT_CALL(mock_bridge_controller_, SetInterfaceUp("ethX1")).InSequence(s1).WillOnce(
      Return(Status(StatusCode::OK)));

  EXPECT_CALL(mock_bridge_controller_, AddInterface(Eq("br0"),Eq("ethX1"))).InSequence(s1)
      .WillOnce(Return(Status(StatusCode::OK)));

  bridge_configurator_->Configure(target_config);
}

TEST_F(ABridgeConfigurator, GetsConfigurationContainingOneBridgeAndOneInterface) {

  Bridges actual_bridges = { "br0" };
  Interfaces actual_br0_interfaces = { "ethX1" };

  EXPECT_CALL(mock_bridge_controller_, GetBridges()).WillRepeatedly(
      Return(actual_bridges));

  EXPECT_CALL(mock_bridge_controller_, GetBridgeInterfaces(Eq("br0"))).WillRepeatedly(
      Return(actual_br0_interfaces));

  BridgeConfig config = bridge_configurator_->GetConfiguration();

  EXPECT_EQ(config["br0"][0], "ethX1");
}

TEST_F(ABridgeConfigurator, GetsConfigurationContainingBridgesAndInterfaces) {

Bridges actual_bridges = { "br0", "br1" };
  EXPECT_CALL(mock_bridge_controller_, GetBridges()).WillRepeatedly(
      Return(actual_bridges));

  Interfaces actual_br0_interfaces = { "ethX1", "ethX2" };
  EXPECT_CALL(mock_bridge_controller_, GetBridgeInterfaces(Eq("br0"))).WillRepeatedly(
      Return(actual_br0_interfaces));

  Interfaces actual_br1_interfaces = { "ethX11", "ethX12" };
  EXPECT_CALL(mock_bridge_controller_, GetBridgeInterfaces(Eq("br1"))).WillRepeatedly(
      Return(actual_br1_interfaces));

  BridgeConfig config = bridge_configurator_->GetConfiguration();

  EXPECT_EQ(config["br0"][0], "ethX1");
  EXPECT_EQ(config["br0"][1], "ethX2");
  EXPECT_EQ(config["br1"][0], "ethX11");
  EXPECT_EQ(config["br1"][1], "ethX12");
}

TEST_F(ABridgeConfigurator, SetsUpBridges) {

  EXPECT_CALL(mock_bridge_controller_, SetInterfaceUp("br0")).WillOnce(
      Return(Status(StatusCode::OK)));
  EXPECT_CALL(mock_bridge_controller_, SetInterfaceUp("br1")).WillOnce(
      Return(Status(StatusCode::OK)));

  Status status = bridge_configurator_->SetBridgeUp("br0");
  status = bridge_configurator_->SetBridgeUp("br1");

  ASSERT_EQ(StatusCode::OK, status.Get());
}

TEST_F(ABridgeConfigurator, SetsDownBridges) {

  EXPECT_CALL(mock_bridge_controller_, SetInterfaceDown("br0")).WillOnce(
      Return(Status(StatusCode::OK)));
  EXPECT_CALL(mock_bridge_controller_, SetInterfaceDown("br1")).WillOnce(
      Return(Status(StatusCode::OK)));

  Status status = bridge_configurator_->SetBridgeDown("br0");
  status = bridge_configurator_->SetBridgeDown("br1");

  ASSERT_EQ(StatusCode::OK, status.Get());
}

TEST_F(ABridgeConfigurator, SetsUpBridgesFailed) {

  EXPECT_CALL(mock_bridge_controller_, SetInterfaceUp("br0")).WillOnce(
      Return(Status(StatusCode::OK)));
  EXPECT_CALL(mock_bridge_controller_, SetInterfaceUp("br1")).WillOnce(
      Return(Status(StatusCode::ERROR)));

  Status status = bridge_configurator_->SetBridgeUp("br0");
  status = bridge_configurator_->SetBridgeUp("br1");

  EXPECT_EQ(StatusCode::ERROR, status.Get());
}

TEST_F(ABridgeConfigurator, SetsDownBridgesFailed) {

  EXPECT_CALL(mock_bridge_controller_, SetInterfaceDown("br0")).WillOnce(
      Return(Status(StatusCode::ERROR)));
  EXPECT_CALL(mock_bridge_controller_, SetInterfaceDown("br1")).WillOnce(
      Return(Status(StatusCode::OK)));

  Status status1 = bridge_configurator_->SetBridgeDown("br0");
  Status status2 = bridge_configurator_->SetBridgeDown("br1");

  EXPECT_EQ(StatusCode::ERROR, status1.Get());
}

TEST_F(ABridgeConfigurator, GetsBridgeAssignedInterfaces) {

  Bridges bridges = { "br0", "br1" };
  EXPECT_CALL(mock_bridge_controller_, GetBridges()).WillOnce(Return(bridges));

  Interfaces interfaces_br0 = { "ethX1" };
  EXPECT_CALL(mock_bridge_controller_, GetBridgeInterfaces(Eq("br0"))).WillOnce(
      Return(interfaces_br0));

  Interfaces interfaces_br1 = { "ethX2", "ethX3" };
  EXPECT_CALL(mock_bridge_controller_, GetBridgeInterfaces(Eq("br1"))).WillOnce(
      Return(interfaces_br1));

  Interfaces assigned_interfaces = bridge_configurator_->GetBridgeAssignedInterfaces();

  EXPECT_EQ(3, assigned_interfaces.size());
  EXPECT_TRUE(IsIncluded("ethX1", assigned_interfaces));
  EXPECT_TRUE(IsIncluded("ethX2", assigned_interfaces));
  EXPECT_TRUE(IsIncluded("ethX3", assigned_interfaces));
}

//TODO Test hinzufügen der prüft ob die interfaces erst up geschaltet werden bevor sie einer bridge hinzugefügt werden
}

//---- End of source file ------------------------------------------------------

