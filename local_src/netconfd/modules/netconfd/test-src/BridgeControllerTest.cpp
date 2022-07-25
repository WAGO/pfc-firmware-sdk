// SPDX-License-Identifier: GPL-2.0-or-later
//------------------------------------------------------------------------------
///  \file     DataPointConfigurationTest.cpp
///
///  \brief    <short description of the file contents>
///
///  \author   <author> : WAGO GmbH & Co. KG
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// include files
//------------------------------------------------------------------------------
#include "BridgeController.hpp"

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <memory>
#include <vector>
#include <algorithm>
#include <cstdio>
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



}
