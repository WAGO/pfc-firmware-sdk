// SPDX-License-Identifier: GPL-2.0-or-later
//------------------------------------------------------------------------------
///  \file     DataPointConfigurationTest.cpp
///
///  \brief    <short description of the file contents>
///
///  \author   <author> : WAGO Kontakttechnik GmbH & Co. KG
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// include files
//------------------------------------------------------------------------------
#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <memory>
#include <vector>
#include <algorithm>
#include <cstdio>
#include "BridgeController.hpp"

#include "CommandExecutor.hpp"

using namespace testing;

namespace netconf {

class ABridgeController_Target : public Test {
 public:
  ::std::shared_ptr<BridgeController> bridge_controller_;
  Bridge const test_br42_ = "br42";
  Bridge const test_br43_ = "br43";
  ::std::string const test_tap42_ = "tap42";
  ::std::string const test_tap43_ = "tap43";
  CommandExecutor executor_;

  ABridgeController_Target() {
    bridge_controller_ = ::std::make_shared<BridgeController>();
  }

  void SetUp() override {

  }

  void TearDown() override {
    DeleteTunInterface(test_tap42_);
    DeleteTunInterface(test_tap43_);
    DeleteBridge(test_br42_);
    DeleteBridge(test_br43_);

    // Reset /etc/network/interfaces file
    CommandExecutor executor;
    executor.Execute("/etc/config-tools/adapt_netconfig");
  }

  void AddTunInterface(::std::string name) {
    FILE* file;
    file = popen(("ip tuntap add mode tap dev " + name).c_str(), "r");
    pclose(file);
  }

  void DeleteTunInterface(::std::string name) {
    FILE* file;
    file = popen(("ip tuntap del mode tap dev " + name).c_str(), "r");
    pclose(file);
  }

  void AddBridge(::std::string name) {
    FILE* file;
    file = popen(("brctl addbr " + name).c_str(), "r");
    pclose(file);
  }

  void DeleteBridge(::std::string name) {
    FILE* file;
    file = popen(("brctl delbr " + name).c_str(), "r");
    pclose(file);
  }

  bool Contains(const Bridge& name,
                Bridges vector) {
    return ::std::find(vector.begin(), vector.end(), name) != vector.end();
  }
  bool Contains(const Interface& name,
                Interfaces vector) {
    return ::std::find(vector.begin(), vector.end(), name) != vector.end();
  }

};

//TEST_F(ABridgeController_Target, GetsBridgeBr0) {
//
//  Bridge bridge_name = "br0";
//  Bridges bridges = bridge_controller_->GetBridges();
//  ASSERT_LT(0, bridges.size());
//  EXPECT_TRUE(Contains(bridge_name, bridges));
//}
//
//TEST_F(ABridgeController_Target, GetsInterfaceEthX1OfBridgeBr0) {
//
//  Bridge bridge_name = "br0";
//  Interface interface_name = "ethX1";
//
//  Interfaces interfaces = bridge_controller_->GetBridgeInterfaces(bridge_name);
//  ASSERT_LT(0, interfaces.size());
//  EXPECT_TRUE(Contains(interface_name, interfaces));
//}
//
//TEST_F(ABridgeController_Target, AddsABridge) {
//
//  Status status = bridge_controller_->AddBridge(test_br42_);
//  ASSERT_EQ(StatusCode::OK, status.Get());
//
//  Bridges bridges = bridge_controller_->GetBridges();
//  ASSERT_LT(0, bridges.size());
//  ASSERT_TRUE(Contains(test_br42_, bridges));
//}
//
//TEST_F(ABridgeController_Target, RemovesABridge) {
//
//  AddBridge(test_br42_);
//
//  Status status = bridge_controller_->DeleteBridge(test_br42_);
//  ASSERT_EQ(StatusCode::OK, status.Get());
//
//  Bridges bridges = bridge_controller_->GetBridges();
//  ASSERT_LT(0, bridges.size());
//  ASSERT_FALSE(Contains(test_br42_, bridges));
//}
//
//TEST_F(ABridgeController_Target, AddsAnInterfaceToABridge) {
//
//  AddBridge(test_br43_);
//  AddTunInterface(test_tap43_);
//
//  Status status = bridge_controller_->AddInterface(test_br43_, test_tap43_);
//  ASSERT_EQ(StatusCode::OK, status.Get());
//
//  Interfaces interfaces = bridge_controller_->GetBridgeInterfaces(test_br43_);
//  ASSERT_LT(0, interfaces.size());
//  ASSERT_TRUE(Contains(test_tap43_, interfaces));
//}
//
//TEST_F(ABridgeController_Target, RemovesAnInterfaceFromABridge) {
//
//  AddTunInterface(test_tap43_);
//  AddBridge(test_br43_);
//  bridge_controller_->AddInterface(test_br43_, test_tap43_);
//
//  Status status = bridge_controller_->DeleteInterface(test_br43_, test_tap43_);
//  ASSERT_EQ(StatusCode::OK, status.Get());
//
//  Interfaces interfaces = bridge_controller_->GetBridgeInterfaces(test_br43_);
//  ASSERT_EQ(0, interfaces.size());
//  EXPECT_FALSE(Contains(test_tap43_, interfaces));
//}

TEST_F(ABridgeController_Target, GetsAllInterfaces) {

  Interfaces interfaces = bridge_controller_->GetInterfaces();
  ASSERT_LT(0, interfaces.size());

  EXPECT_FALSE(Contains(".", interfaces));
  EXPECT_FALSE(Contains("..", interfaces));
  EXPECT_TRUE(Contains("ethX1", interfaces));
  EXPECT_TRUE(Contains("ethX2", interfaces));
  EXPECT_TRUE(Contains("br0", interfaces));
  EXPECT_TRUE(Contains("br1", interfaces));
  EXPECT_TRUE(Contains("lo", interfaces));
}


//TEST_F(ABridgeController_Target, SetsInterfaceUpDown) {
//  AddTunInterface(test_tap43_);
//  Status status = bridge_controller_->SetInterfaceUp(test_tap43_);
//  ASSERT_EQ(StatusCode::OK, status.Get());
//
//  status = bridge_controller_->SetInterfaceDown(test_tap43_);
//  ASSERT_EQ(StatusCode::OK, status.Get());
//}
//
//TEST_F(ABridgeController_Target, GetsInterfaceUpDownState) {
//  AddTunInterface(test_tap43_);
//  Status status = bridge_controller_->SetInterfaceUp(test_tap43_);
//  ASSERT_EQ(StatusCode::OK, status.Get());
//
//  bool is_up;
//  status = bridge_controller_->IsInterfaceUp(test_tap43_,is_up);
//  EXPECT_TRUE(is_up);
//
//  status = bridge_controller_->SetInterfaceDown(test_tap43_);
//  ASSERT_EQ(StatusCode::OK, status.Get());
//
//  status = bridge_controller_->IsInterfaceUp(test_tap43_,is_up);
//  EXPECT_FALSE(is_up);
//}
//

}
