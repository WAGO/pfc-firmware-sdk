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

  IPConfigs config { { "br0", IPSource::STATIC, "192.168.1.17", "255.255.255.0" } };

  EXPECT_CALL(mock_ip_controller_, SetIPConfig(Eq(config[0]))).WillOnce(Return(Error(ErrorCode::OK)));

  EXPECT_CALL(mock_dhcp_client_controller_, StopClient(Eq("br0"))).Times(1);

  EXPECT_CALL(mock_bootp_controller_, StopClient(Eq("br0"))).Times(1);

  Error status = ip_configurator_->Configure(config);

  ASSERT_EQ(ErrorCode::OK, status.GetErrorCode());
}

TEST_F(AnIPConfigurator, SuccessfullySetsADHCPConfiguration) {

  IPConfigs config { { "br0", IPSource::DHCP, "192.168.1.17", "255.255.255.0" } };

  IPConfig config_dhcp_flush = { "br0", IPSource::TEMPORARY, ZeroIP, ZeroIP };
  EXPECT_CALL(mock_ip_controller_, SetIPConfig(Eq(config_dhcp_flush))).WillOnce(Return(Error(ErrorCode::OK)));

  EXPECT_CALL(mock_bootp_controller_, StopClient(Eq("br0"))).Times(1);

  EXPECT_CALL(mock_dhcp_client_controller_, GetStatus(Eq("br0"))).WillOnce(Return(DHCPClientStatus::STOPPED));

  EXPECT_CALL(mock_dhcp_client_controller_, StartClient(Eq("br0"))).WillOnce(Return(Error(ErrorCode::OK)));

  Error status = ip_configurator_->Configure(config);

  ASSERT_EQ(ErrorCode::OK, status.GetErrorCode());
}

TEST_F(AnIPConfigurator, SuccessfullySetsTwoIPConfigurations) {

  IPConfigs config { { "br0", IPSource::STATIC, "192.168.1.17", "255.255.255.0" }, { "br1",
      IPSource::STATIC, "", ""} };

  EXPECT_CALL(mock_ip_controller_, SetIPConfig(Eq(config[0]))).WillOnce(Return(Error(ErrorCode::OK)));

  EXPECT_CALL(mock_ip_controller_, SetIPConfig(Eq(config[1]))).WillOnce(Return(Error(ErrorCode::OK)));

  EXPECT_CALL(mock_dhcp_client_controller_, StopClient(Eq("br0"))).Times(1);

  EXPECT_CALL(mock_bootp_controller_, StopClient(Eq("br0"))).Times(1);

  EXPECT_CALL(mock_dhcp_client_controller_, StopClient(Eq("br1"))).Times(1);

  EXPECT_CALL(mock_bootp_controller_, StopClient(Eq("br1"))).Times(1);

  Error status = ip_configurator_->Configure(config);

  ASSERT_EQ(ErrorCode::OK, status.GetErrorCode());
}

TEST_F(AnIPConfigurator, TriesToSetAStaticIPConfiguration) {

  IPConfigs config { { "br0", IPSource::STATIC, "192.168.1.17", "255.255.255.0" } };

  EXPECT_CALL(mock_dhcp_client_controller_, StopClient(Eq("br0"))).Times(1);

  EXPECT_CALL(mock_bootp_controller_, StopClient(Eq("br0"))).Times(1);

  EXPECT_CALL(mock_ip_controller_, SetIPConfig(Eq(config[0]))).WillOnce(Return(Error(ErrorCode::SET_IP)));

  Error status = ip_configurator_->Configure(config);

  EXPECT_EQ(ErrorCode::SET_IP, status.GetErrorCode());
}

TEST_F(AnIPConfigurator, TriesToSetADHCPIPConfiguration) {

  IPConfigs config { { "br0", IPSource::DHCP, "192.168.1.17", "255.255.255.0" } };

  IPConfig config_dhcp_flush = { "br0", IPSource::TEMPORARY, ZeroIP, ZeroIP};
  EXPECT_CALL(mock_ip_controller_, SetIPConfig(Eq(config_dhcp_flush))).WillOnce(Return(Error(ErrorCode::OK)));

  EXPECT_CALL(mock_bootp_controller_, StopClient(Eq("br0"))).Times(1);

  EXPECT_CALL(mock_dhcp_client_controller_, GetStatus(Eq("br0"))).WillOnce(Return(DHCPClientStatus::STOPPED));

  EXPECT_CALL(mock_dhcp_client_controller_, StartClient(Eq("br0"))).WillOnce(Return(Error(ErrorCode::DHCP_CLIENT_START)));

  Error status = ip_configurator_->Configure(config);

  EXPECT_EQ(ErrorCode::DHCP_CLIENT_START, status.GetErrorCode());
}

TEST_F(AnIPConfigurator, TriesToSetThreeStaticIPConfigurations) {

  IPConfigs config { { "br0", IPSource::STATIC, "192.168.1.17", "255.255.255.0" }, { "br1",
      IPSource::STATIC, "", "" }, { "br3", IPSource::STATIC, "", "" } };

  EXPECT_CALL(mock_ip_controller_, SetIPConfig(Eq(config[0]))).WillOnce(Return(Error(ErrorCode::OK)));
  EXPECT_CALL(mock_ip_controller_, SetIPConfig(Eq(config[1]))).WillOnce(Return(Error(ErrorCode::SET_IP)));
  EXPECT_CALL(mock_ip_controller_, SetIPConfig(Eq(config[2]))).WillOnce(Return(Error(ErrorCode::OK)));

  EXPECT_CALL(mock_dhcp_client_controller_, StopClient(Eq("br0"))).Times(1);
  EXPECT_CALL(mock_bootp_controller_, StopClient(Eq("br0"))).Times(1);

  EXPECT_CALL(mock_dhcp_client_controller_, StopClient(Eq("br1"))).Times(1);
  EXPECT_CALL(mock_bootp_controller_, StopClient(Eq("br1"))).Times(1);

  EXPECT_CALL(mock_dhcp_client_controller_, StopClient(Eq("br3"))).Times(1);
  EXPECT_CALL(mock_bootp_controller_, StopClient(Eq("br3"))).Times(1);

  Error status = ip_configurator_->Configure(config);

  EXPECT_EQ(ErrorCode::SET_IP, status.GetErrorCode());
}

TEST_F(AnIPConfigurator, TriesToSetThreeStaticAndDHCPConfigurations) {

  IPConfigs config { { "br0", IPSource::STATIC, "192.168.1.17", "255.255.255.0" }, { "br1",
      IPSource::DHCP, "", "" }, { "br3", IPSource::STATIC, "", "" } };

  EXPECT_CALL(mock_ip_controller_, SetIPConfig(Eq(config[0]))).WillOnce(Return(Error(ErrorCode::SET_IP)));
  IPConfig config_dhcp_flush = { "br1", IPSource::TEMPORARY, ZeroIP, ZeroIP };
  EXPECT_CALL(mock_ip_controller_, SetIPConfig(Eq(config_dhcp_flush))).WillOnce(Return(Error(ErrorCode::OK)));
  EXPECT_CALL(mock_ip_controller_, SetIPConfig(Eq(config[2]))).WillOnce(Return(Error(ErrorCode::OK)));

  EXPECT_CALL(mock_bootp_controller_, StopClient(Eq("br0"))).Times(1);
  EXPECT_CALL(mock_dhcp_client_controller_, StopClient(Eq("br0"))).Times(1);

  EXPECT_CALL(mock_bootp_controller_, StopClient(Eq("br1"))).Times(1);
  EXPECT_CALL(mock_dhcp_client_controller_, GetStatus(Eq("br1"))).WillOnce(Return(DHCPClientStatus::STOPPED));
  EXPECT_CALL(mock_dhcp_client_controller_, StartClient(Eq("br1"))).WillOnce(Return(Error(ErrorCode::DHCP_CLIENT_START)));

  EXPECT_CALL(mock_bootp_controller_, StopClient(Eq("br3"))).Times(1);
  EXPECT_CALL(mock_dhcp_client_controller_, StopClient(Eq("br3"))).Times(1);

  Error status = ip_configurator_->Configure(config);

  EXPECT_EQ(ErrorCode::DHCP_CLIENT_START, status.GetErrorCode());
}


} /* namespace netconf */
