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
#include "MockIInterfaceInformation.hpp"
#include "MockIDHCPClientController.hpp"
#include "MockIIPLinks.hpp"
#include "MockIBridgeController.hpp"
#include "MockIEventManager.hpp"
#include "Types.hpp"
#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <MockIBootpClientController.hpp>
#include <memory>
#include "IPLink.hpp"
#include "TypesTestHelper.hpp"

using namespace testing;

namespace netconfd {

class AnIPConfigurator : public Test {
 public:

  MockIIPController mock_ip_controller_;
  MockIInterfaceManagerInformation mock_interface_information_;
  MockIDHCPClientController mock_dhcp_client_controller_;
  MockIBootpController mock_bootp_controller_;
  MockIIPLinks mock_ip_links_;
  MockIEventManager mock_event_manager_;

  AnIPConfigurator() {

  }

  void SetUp() override {
    ip_configurator_ = new IPConfigurator(mock_ip_controller_, mock_dhcp_client_controller_, mock_bootp_controller_,
                                          mock_interface_information_, mock_ip_links_);
  }

  void TearDown() override {

  }

  IPConfigurator *ip_configurator_;

};

TEST_F(AnIPConfigurator, SuccessfullySetsAStaticIPConfiguration) {

  IPConfigs config { { "br0", IPSource::STATIC, "192.168.1.17", "255.255.255.0", "192.168.1.255" } };

  EXPECT_CALL(mock_ip_controller_, SetIPConfig(Eq(config[0]))).WillOnce(Return(Status(StatusCode::OK)));

  EXPECT_CALL(mock_dhcp_client_controller_, StopClient(Eq("br0"))).WillOnce(Return(Status(StatusCode::OK)));

  EXPECT_CALL(mock_bootp_controller_, StopClient(Eq("br0"))).WillOnce(Return(Status(StatusCode::OK)));

  Status status = ip_configurator_->Configure(config);

  ASSERT_EQ(StatusCode::OK, status.Get());
}

TEST_F(AnIPConfigurator, SuccessfullySetsADHCPConfiguration) {

  IPConfigs config { { "br0", IPSource::DHCP, "192.168.1.17", "255.255.255.0", "192.168.1.255" } };

  IPConfig config_dhcp_flush = { "br0", IPSource::TEMPORARY, ZeroIP, ZeroIP };
  EXPECT_CALL(mock_ip_controller_, SetIPConfig(Eq(config_dhcp_flush))).WillOnce(Return(Status(StatusCode::OK)));

  EXPECT_CALL(mock_bootp_controller_, StopClient(Eq("br0"))).WillOnce(Return(Status(StatusCode::OK)));

  EXPECT_CALL(mock_dhcp_client_controller_, GetStatus(Eq("br0"))).WillOnce(Return(DHCPClientStatus::STOPPED));

  EXPECT_CALL(mock_dhcp_client_controller_, StartClient(Eq("br0"))).WillOnce(Return(Status(StatusCode::OK)));

  Status status = ip_configurator_->Configure(config);

  ASSERT_EQ(StatusCode::OK, status.Get());
}

TEST_F(AnIPConfigurator, SuccessfullySetsTwoIPConfigurations) {

  IPConfigs config { { "br0", IPSource::STATIC, "192.168.1.17", "255.255.255.0", "192.168.1.255" }, { "br1",
      IPSource::STATIC, "", "", "" } };

  EXPECT_CALL(mock_ip_controller_, SetIPConfig(Eq(config[0]))).WillOnce(Return(Status(StatusCode::OK)));

  EXPECT_CALL(mock_ip_controller_, SetIPConfig(Eq(config[1]))).WillOnce(Return(Status(StatusCode::OK)));

  EXPECT_CALL(mock_dhcp_client_controller_, StopClient(Eq("br0"))).WillOnce(Return(Status(StatusCode::OK)));

  EXPECT_CALL(mock_bootp_controller_, StopClient(Eq("br0"))).WillOnce(Return(Status(StatusCode::OK)));

  EXPECT_CALL(mock_dhcp_client_controller_, StopClient(Eq("br1"))).WillOnce(Return(Status(StatusCode::OK)));

  EXPECT_CALL(mock_bootp_controller_, StopClient(Eq("br1"))).WillOnce(Return(Status(StatusCode::OK)));

  Status status = ip_configurator_->Configure(config);

  ASSERT_EQ(StatusCode::OK, status.Get());
}

TEST_F(AnIPConfigurator, TriesToSetAStaticIPConfiguration) {

  IPConfigs config { { "br0", IPSource::STATIC, "192.168.1.17", "255.255.255.0", "192.168.1.255" } };

  EXPECT_CALL(mock_dhcp_client_controller_, StopClient(Eq("br0"))).WillOnce(Return(Status(StatusCode::OK)));

  EXPECT_CALL(mock_bootp_controller_, StopClient(Eq("br0"))).WillOnce(Return(Status(StatusCode::OK)));

  EXPECT_CALL(mock_ip_controller_, SetIPConfig(Eq(config[0]))).WillOnce(Return(Status(StatusCode::ERROR)));

  Status status = ip_configurator_->Configure(config);

  EXPECT_EQ(StatusCode::ERROR, status.Get());
}

TEST_F(AnIPConfigurator, TriesToSetADHCPIPConfiguration) {

  IPConfigs config { { "br0", IPSource::DHCP, "192.168.1.17", "255.255.255.0", "192.168.1.255" } };

  IPConfig config_dhcp_flush = { "br0", IPSource::TEMPORARY, ZeroIP, ZeroIP };
  EXPECT_CALL(mock_ip_controller_, SetIPConfig(Eq(config_dhcp_flush))).WillOnce(Return(Status(StatusCode::OK)));

  EXPECT_CALL(mock_bootp_controller_, StopClient(Eq("br0"))).WillOnce(Return(Status(StatusCode::OK)));

  EXPECT_CALL(mock_dhcp_client_controller_, GetStatus(Eq("br0"))).WillOnce(Return(DHCPClientStatus::STOPPED));

  EXPECT_CALL(mock_dhcp_client_controller_, StartClient(Eq("br0"))).WillOnce(Return(Status(StatusCode::ERROR)));

  Status status = ip_configurator_->Configure(config);

  EXPECT_EQ(StatusCode::ERROR, status.Get());
}

TEST_F(AnIPConfigurator, TriesToSetThreeStaticIPConfigurations) {

  IPConfigs config { { "br0", IPSource::STATIC, "192.168.1.17", "255.255.255.0", "192.168.1.255" }, { "br1",
      IPSource::STATIC, "", "", "" }, { "br3", IPSource::STATIC, "", "", "" } };

  EXPECT_CALL(mock_ip_controller_, SetIPConfig(Eq(config[0]))).WillOnce(Return(Status(StatusCode::OK)));
  EXPECT_CALL(mock_ip_controller_, SetIPConfig(Eq(config[1]))).WillOnce(Return(Status(StatusCode::ERROR)));
  EXPECT_CALL(mock_ip_controller_, SetIPConfig(Eq(config[2]))).WillOnce(Return(Status(StatusCode::OK)));

  EXPECT_CALL(mock_dhcp_client_controller_, StopClient(Eq("br0"))).WillOnce(Return(Status(StatusCode::OK)));
  EXPECT_CALL(mock_bootp_controller_, StopClient(Eq("br0"))).WillOnce(Return(Status(StatusCode::OK)));

  EXPECT_CALL(mock_dhcp_client_controller_, StopClient(Eq("br1"))).WillOnce(Return(Status(StatusCode::OK)));
  EXPECT_CALL(mock_bootp_controller_, StopClient(Eq("br1"))).WillOnce(Return(Status(StatusCode::OK)));

  EXPECT_CALL(mock_dhcp_client_controller_, StopClient(Eq("br3"))).WillOnce(Return(Status(StatusCode::OK)));
  EXPECT_CALL(mock_bootp_controller_, StopClient(Eq("br3"))).WillOnce(Return(Status(StatusCode::OK)));

  Status status = ip_configurator_->Configure(config);

  EXPECT_EQ(StatusCode::ERROR, status.Get());
}

TEST_F(AnIPConfigurator, TriesToSetThreeStaticAndDHCPConfigurations) {

  IPConfigs config { { "br0", IPSource::STATIC, "192.168.1.17", "255.255.255.0", "192.168.1.255" }, { "br1",
      IPSource::DHCP, "", "", "" }, { "br3", IPSource::STATIC, "", "", "" } };

  EXPECT_CALL(mock_ip_controller_, SetIPConfig(Eq(config[0]))).WillOnce(Return(Status(StatusCode::ERROR)));
  IPConfig config_dhcp_flush = { "br1", IPSource::TEMPORARY, ZeroIP, ZeroIP };
  EXPECT_CALL(mock_ip_controller_, SetIPConfig(Eq(config_dhcp_flush))).WillOnce(Return(Status(StatusCode::OK)));
  EXPECT_CALL(mock_ip_controller_, SetIPConfig(Eq(config[2]))).WillOnce(Return(Status(StatusCode::OK)));

  EXPECT_CALL(mock_bootp_controller_, StopClient(Eq("br0"))).WillOnce(Return(Status(StatusCode::OK)));
  EXPECT_CALL(mock_dhcp_client_controller_, StopClient(Eq("br0"))).WillOnce(Return(Status(StatusCode::OK)));

  EXPECT_CALL(mock_bootp_controller_, StopClient(Eq("br1"))).WillOnce(Return(Status(StatusCode::OK)));
  EXPECT_CALL(mock_dhcp_client_controller_, GetStatus(Eq("br1"))).WillOnce(Return(DHCPClientStatus::STOPPED));
  EXPECT_CALL(mock_dhcp_client_controller_, StartClient(Eq("br1"))).WillOnce(Return(Status(StatusCode::ERROR)));

  EXPECT_CALL(mock_bootp_controller_, StopClient(Eq("br3"))).WillOnce(Return(Status(StatusCode::OK)));
  EXPECT_CALL(mock_dhcp_client_controller_, StopClient(Eq("br3"))).WillOnce(Return(Status(StatusCode::OK)));

  Status status = ip_configurator_->Configure(config);

  EXPECT_EQ(StatusCode::ERROR, status.Get());
}

TEST_F(AnIPConfigurator, SuccessfullyGetsAnIPConfiguration) {

  IPConfig config_returnedByIpController = { "br0", IPSource::NONE, "192.168.1.17", "255.255.255.0", "192.168.1.255" };

  EXPECT_CALL(mock_ip_controller_, GetIPConfig(Eq("br0"),_)).Times(0);

  auto netdev = ::std::make_shared<NetDev>();
  auto ip_link = ::std::make_shared<IPLink>(*ip_configurator_, mock_event_manager_, netdev);
  ip_link->SetIPConfig(IPConfig("br0", IPSource::STATIC, "192.168.1.17", "255.255.255.0", "192.168.1.255"));
  EXPECT_CALL(mock_ip_links_, Get(Eq("br0"))).WillOnce(Return(ip_link));

  Bridges bridges = { { "br0" } };
  IPConfigs configs = ip_configurator_->GetConfigurations(bridges);

  IPConfig config_expected = { "br0", IPSource::STATIC, "192.168.1.17", "255.255.255.0", "192.168.1.255" };
  EXPECT_EQ(config_expected, configs[0]);
}

TEST_F(AnIPConfigurator, SuccessfullyGetsMultipleIPConfigurations) {

  IPConfigs configs { { "br0", IPSource::NONE, "192.168.1.17", "255.255.255.0", "192.168.1.255" }, { "br1",
      IPSource::NONE, "192.168.22.17", "255.255.255.0", "192.168.22.255" } };

  EXPECT_CALL(mock_ip_controller_, GetIPConfig(Eq("br0"), _)).Times(0);

  EXPECT_CALL(mock_ip_controller_, GetIPConfig(Eq("br1"), _)).WillOnce(
      DoAll(SetArgReferee<1>(configs[1]), Return(Status(StatusCode::OK))));

  auto netdev1 = ::std::make_shared<NetDev>();
  auto ip_link1 = ::std::make_shared<IPLink>(*ip_configurator_, mock_event_manager_, netdev1);
  ip_link1->SetIPConfig(IPConfig( "br0", IPSource::STATIC, "192.168.1.17", "255.255.255.0", "192.168.1.255" ));
  EXPECT_CALL(mock_ip_links_, Get(Eq("br0"))).WillOnce(Return(ip_link1));

  auto netdev2 = ::std::make_shared<NetDev>();
  auto ip_link2 = ::std::make_shared < IPLink > (*ip_configurator_, mock_event_manager_,netdev2);
  ip_link2->SetIPConfig(IPConfig( "br1",
                                  IPSource::DHCP, ZeroIP, ZeroIP, ZeroIP ));
  EXPECT_CALL(mock_ip_links_, Get(Eq("br1"))).WillOnce(Return(ip_link2));

  Bridges bridges = { { "br0" }, { "br1" } };
  IPConfigs configs_returned = ip_configurator_->GetConfigurations(bridges);

  IPConfigs configs_expected { { "br0", IPSource::STATIC, "192.168.1.17", "255.255.255.0", "192.168.1.255" }, { "br1",
      IPSource::DHCP, "192.168.22.17", "255.255.255.0", "192.168.22.255" } };

  EXPECT_EQ(configs_expected[0], configs_returned[0]);
  EXPECT_EQ(configs_expected[1], configs_returned[1]);
}

TEST_F(AnIPConfigurator, TriesToGetAnInvalidIPConfiguration) {

  IPConfig config = { "br0", IPSource::STATIC, "192.168.1.17", "255.255.255.0", "192.168.1.255" };

  EXPECT_CALL(mock_ip_controller_, GetIPConfig(Eq("br0"),_)).WillOnce(
      DoAll(SetArgReferee<1>(config), Return(Status(StatusCode::ERROR))));

  Bridges bridges = { { "br0" } };
  IPConfigs configs_returned = ip_configurator_->GetConfigurations(bridges);

  EXPECT_TRUE(configs_returned.empty());
}

TEST_F(AnIPConfigurator, TriesToGetMultipleIPConfigurationsContainingOneInvalid) {

  IPConfigs configs { { "br0", IPSource::STATIC, "192.168.1.17", "255.255.255.0", "192.168.1.255" }, { "br1",
      IPSource::DHCP, "", "", "" }, { "br3", IPSource::DHCP, "", "", "" } };

  EXPECT_CALL(mock_ip_controller_, GetIPConfig(Eq("br0"), _)).WillOnce(
      DoAll(SetArgReferee<1>(configs[0]), Return(Status(StatusCode::OK))));

  EXPECT_CALL(mock_ip_controller_, GetIPConfig(Eq("br1"), _)).WillOnce(
      DoAll(SetArgReferee<1>(configs[1]), Return(Status(StatusCode::ERROR))));

  EXPECT_CALL(mock_ip_controller_, GetIPConfig(Eq("br3"), _)).WillOnce(
      DoAll(SetArgReferee<1>(configs[2]), Return(Status(StatusCode::OK))));

  Bridges bridges = { { "br0" }, { "br1" }, { "br3" } };
  IPConfigs configs_returned = ip_configurator_->GetConfigurations(bridges);

  EXPECT_EQ(2u, configs_returned.size());
}

} /* namespace netconfd */
