// SPDX-License-Identifier: GPL-2.0-or-later
//------------------------------------------------------------------------------
///  \file     IPConfiguratorTest.cpp
///
///  \brief    <short description of the file contents>
///
///  \author   <author> : WAGO Kontakttechnik GmbH & Co. KG
//------------------------------------------------------------------------------
#include "IPConfigurator.hpp"
#include "MockIIPController.hpp"
#include "MockIDHCPClientController.hpp"
#include "MockIIPLinks.hpp"
#include "MockIBridgeController.hpp"
#include "MockIEventManager.hpp"
#include "Types.hpp"
#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <MockIBootpClientController.hpp>
#include <MockIBridgeInformation.hpp>
#include <memory>
#include "IPLink.hpp"
#include "TypesTestHelper.hpp"

using namespace testing;

namespace netconf {

class AnIPConfigurator : public Test {
 public:

  MockIIPController mock_ip_controller_;
  MockIDHCPClientController mock_dhcp_client_controller_;
  MockIBootpController mock_bootp_controller_;
  MockIIPLinks mock_ip_links_;
  MockIEventManager mock_event_manager_;

  AnIPConfigurator() {

  }

  void SetUp() override {
    ip_configurator_ = new IPConfigurator(mock_ip_controller_, mock_dhcp_client_controller_, mock_bootp_controller_,
                                           mock_ip_links_);
  }

  void TearDown() override {

  }

  IPConfigurator *ip_configurator_;

};

TEST_F(AnIPConfigurator, SuccessfullySetsAStaticIPConfiguration) {

  IPConfig config {  "br0", IPSource::STATIC, "192.168.1.17", "255.255.255.0"  };

  EXPECT_CALL(mock_ip_controller_, SetIPConfig(Eq(config))).WillOnce(Return(Status(StatusCode::OK)));

  EXPECT_CALL(mock_dhcp_client_controller_, StopClient(Eq("br0"))).Times(1);

  EXPECT_CALL(mock_bootp_controller_, StopClient(Eq("br0"))).Times(1);

  Status status = ip_configurator_->Configure(config);

  ASSERT_EQ(StatusCode::OK, status.GetStatusCode());
}

TEST_F(AnIPConfigurator, SuccessfullySetsADHCPConfiguration) {

  IPConfig config {  "br0", IPSource::DHCP, "192.168.1.17", "255.255.255.0"  };

  IPConfig config_dhcp_flush = { "br0", IPSource::TEMPORARY, ZeroIP, ZeroIP };
  EXPECT_CALL(mock_ip_controller_, SetIPConfig(Eq(config_dhcp_flush))).WillOnce(Return(Status(StatusCode::OK)));

  EXPECT_CALL(mock_bootp_controller_, StopClient(Eq("br0"))).Times(1);

  EXPECT_CALL(mock_dhcp_client_controller_, GetStatus(Eq("br0"))).WillOnce(Return(DHCPClientStatus::STOPPED));

  EXPECT_CALL(mock_dhcp_client_controller_, StartClient(Eq("br0"))).WillOnce(Return(Status(StatusCode::OK)));

  Status status = ip_configurator_->Configure(config);

  ASSERT_EQ(StatusCode::OK, status.GetStatusCode());
}


TEST_F(AnIPConfigurator, TriesToSetAStaticIPConfiguration) {

  IPConfig config  { "br0", IPSource::STATIC, "192.168.1.17", "255.255.255.0"  };

  EXPECT_CALL(mock_dhcp_client_controller_, StopClient(Eq("br0"))).Times(1);

  EXPECT_CALL(mock_bootp_controller_, StopClient(Eq("br0"))).Times(1);

  EXPECT_CALL(mock_ip_controller_, SetIPConfig(Eq(config))).WillOnce(Return(Status(StatusCode::SET_IP)));

  Status status = ip_configurator_->Configure(config);

  EXPECT_EQ(StatusCode::SET_IP, status.GetStatusCode());
}

TEST_F(AnIPConfigurator, TriesToSetADHCPIPConfiguration) {

  IPConfig config {  "br0", IPSource::DHCP, "192.168.1.17", "255.255.255.0"  };

  IPConfig config_dhcp_flush = { "br0", IPSource::TEMPORARY, ZeroIP, ZeroIP};
  EXPECT_CALL(mock_ip_controller_, SetIPConfig(Eq(config_dhcp_flush))).WillOnce(Return(Status(StatusCode::OK)));

  EXPECT_CALL(mock_bootp_controller_, StopClient(Eq("br0"))).Times(1);

  EXPECT_CALL(mock_dhcp_client_controller_, GetStatus(Eq("br0"))).WillOnce(Return(DHCPClientStatus::STOPPED));

  EXPECT_CALL(mock_dhcp_client_controller_, StartClient(Eq("br0"))).WillOnce(Return(Status(StatusCode::DHCP_CLIENT_START)));

  Status status = ip_configurator_->Configure(config);

  EXPECT_EQ(StatusCode::DHCP_CLIENT_START, status.GetStatusCode());
}




} /* namespace netconf */
