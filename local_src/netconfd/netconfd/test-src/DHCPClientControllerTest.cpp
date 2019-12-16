// SPDX-License-Identifier: GPL-2.0-or-later
//------------------------------------------------------------------------------
///  \file     DHCPClientControllerTest.cpp
///
///  \brief    <short description of the file contents>
///
///  \author   <author> : WAGO Kontakttechnik GmbH & Co. KG
//------------------------------------------------------------------------------

#include "CommandExecutor.hpp"
#include "DevicePropertiesProvider.hpp"
#include "FileEditor.hpp"
#include "BridgeController.hpp"

#include "DHCPClientController.hpp"
#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <boost/filesystem.hpp>

using namespace testing;

namespace netconfd {

class ADHCPClientController_Target : public Test {
 public:
  CommandExecutor command_executor_;
  BridgeController bridge_controller_;
  DevicePropertiesProvider properties_provider_;
  FileEditor file_editor_;

  DHCPClientController dhcp_client_controller_;

  const ::std::string br_test = "br_test";

  ADHCPClientController_Target()
      : properties_provider_{bridge_controller_},dhcp_client_controller_(command_executor_, properties_provider_, file_editor_) {

  }

  void SetUp() override {
    AddBridge(br_test);
  }

  void TearDown() override {

    DeleteBridge(br_test);
  }

  void AddBridge(::std::string name) {
    command_executor_.Execute("brctl addbr " + name);
  }

  void DeleteBridge(::std::string name) {
    command_executor_.Execute("ifconfig " + name + " down");
    command_executor_.Execute("brctl delbr " + name);
  }

};

TEST_F(ADHCPClientController_Target, StartsStopsAndGetsStateOfADHCPClient) {

  Status status;

  DHCPClientStatus state = dhcp_client_controller_.GetStatus(br_test);
  EXPECT_EQ(DHCPClientStatus::STOPPED, state);

  status = dhcp_client_controller_.StartClient(br_test);
  EXPECT_TRUE(status.Ok());

  state = dhcp_client_controller_.GetStatus(br_test);
  EXPECT_EQ(DHCPClientStatus::RUNNING, state);

  status = dhcp_client_controller_.StopClient(br_test);
  EXPECT_TRUE(status.Ok());

  state = dhcp_client_controller_.GetStatus(br_test);
  EXPECT_EQ(DHCPClientStatus::STOPPED, state);

}

} /* namespace netconfd */
