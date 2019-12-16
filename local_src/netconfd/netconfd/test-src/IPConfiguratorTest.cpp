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
#include "Types.hpp"
#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <MockIBootpClientController.hpp>
#include <memory>

using namespace testing;

namespace netconfd {

class AnIPConfigurator : public Test {
 public:

  MockIIPController mock_ip_controller_;
  MockIInterfaceManagerInformation mock_interface_information_;
  MockIDHCPClientController mock_dhcp_client_controller_;
  MockIBootpController mock_bootp_controller_;

  AnIPConfigurator() {

  }

  void SetUp() override {
    ip_configurator_ = ::std::make_unique<IPConfigurator>(mock_ip_controller_, mock_dhcp_client_controller_,
                                                          mock_bootp_controller_, mock_interface_information_);
  }

  void TearDown() override {

  }

  ::std::unique_ptr<IPConfigurator> ip_configurator_;

};

TEST_F(AnIPConfigurator, SuccessfullySetsAStaticIPConfiguration) {

  IPConfigs config { { "br0", IPSource::STATIC, "192.168.1.17", "255.255.255.0", "192.168.1.255" } };

  EXPECT_CALL(mock_interface_information_, IsBridge(Eq("br0"))).WillOnce(Return(true));

  EXPECT_CALL(mock_interface_information_, HasAnyInterfacesUp(Eq("br0"))).WillOnce(Return(true));

  EXPECT_CALL(mock_ip_controller_, SetIPConfig(Eq(config[0]))).WillOnce(Return(Status(StatusCode::OK)));

  EXPECT_CALL(mock_dhcp_client_controller_, StopClient(Eq("br0"))).WillOnce(Return(Status(StatusCode::OK)));

  EXPECT_CALL(mock_bootp_controller_, StopClient(Eq("br0"))).WillOnce(Return(Status(StatusCode::OK)));

  Status status = ip_configurator_->Configure(config);

  ASSERT_EQ(StatusCode::OK, status.Get());
}

TEST_F(AnIPConfigurator, SuccessfullySetsADHCPConfiguration) {

  IPConfigs config { { "br0", IPSource::DHCP, "192.168.1.17", "255.255.255.0", "192.168.1.255" } };

  IPConfig config_dhcp_flush = { "br0", IPSource::TEMPORARY, "0.0.0.0", "0.0.0.0" };
  EXPECT_CALL(mock_ip_controller_, SetIPConfig(Eq(config_dhcp_flush))).WillOnce(Return(Status(StatusCode::OK)));

  EXPECT_CALL(mock_interface_information_, IsBridge(Eq("br0"))).WillOnce(Return(true));

  EXPECT_CALL(mock_bootp_controller_, StopClient(Eq("br0"))).WillOnce(Return(Status(StatusCode::OK)));

  EXPECT_CALL(mock_interface_information_, HasAnyInterfacesUp(Eq("br0"))).WillOnce(Return(true));

  EXPECT_CALL(mock_dhcp_client_controller_, GetStatus(Eq("br0"))).WillOnce(Return(DHCPClientStatus::STOPPED));

  EXPECT_CALL(mock_dhcp_client_controller_, StartClient(Eq("br0"))).WillOnce(Return(Status(StatusCode::OK)));

  Status status = ip_configurator_->Configure(config);

  ASSERT_EQ(StatusCode::OK, status.Get());
}

TEST_F(AnIPConfigurator, SuccessfullySetsTwoIPConfigurations) {

  IPConfigs config { { "br0", IPSource::STATIC, "192.168.1.17", "255.255.255.0", "192.168.1.255" }, { "br1",
      IPSource::STATIC, "", "", "" } };

  EXPECT_CALL(mock_interface_information_, IsBridge(Eq("br0"))).WillOnce(Return(true));
  EXPECT_CALL(mock_interface_information_, IsBridge(Eq("br1"))).WillOnce(Return(true));

  EXPECT_CALL(mock_interface_information_, HasAnyInterfacesUp(Eq("br0"))).WillOnce(Return(true));
  EXPECT_CALL(mock_interface_information_, HasAnyInterfacesUp(Eq("br1"))).WillOnce(Return(true));

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

  EXPECT_CALL(mock_interface_information_, IsBridge(Eq("br0"))).WillOnce(Return(true));

  EXPECT_CALL(mock_dhcp_client_controller_, StopClient(Eq("br0"))).WillOnce(Return(Status(StatusCode::OK)));

  EXPECT_CALL(mock_bootp_controller_, StopClient(Eq("br0"))).WillOnce(Return(Status(StatusCode::OK)));

  EXPECT_CALL(mock_interface_information_, HasAnyInterfacesUp(Eq("br0"))).WillOnce(Return(true));

  EXPECT_CALL(mock_ip_controller_, SetIPConfig(Eq(config[0]))).WillOnce(Return(Status(StatusCode::ERROR)));

  Status status = ip_configurator_->Configure(config);

  EXPECT_EQ(StatusCode::ERROR, status.Get());
}

TEST_F(AnIPConfigurator, TriesToSetADHCPIPConfiguration) {

  IPConfigs config { { "br0", IPSource::DHCP, "192.168.1.17", "255.255.255.0", "192.168.1.255" } };

  IPConfig config_dhcp_flush = { "br0", IPSource::TEMPORARY, "0.0.0.0", "0.0.0.0" };
  EXPECT_CALL(mock_ip_controller_, SetIPConfig(Eq(config_dhcp_flush))).WillOnce(Return(Status(StatusCode::OK)));

  EXPECT_CALL(mock_interface_information_, IsBridge(Eq("br0"))).WillOnce(Return(true));

  EXPECT_CALL(mock_bootp_controller_, StopClient(Eq("br0"))).WillOnce(Return(Status(StatusCode::OK)));

  EXPECT_CALL(mock_interface_information_, HasAnyInterfacesUp(Eq("br0"))).WillOnce(Return(true));

  EXPECT_CALL(mock_dhcp_client_controller_, GetStatus(Eq("br0"))).WillOnce(Return(DHCPClientStatus::STOPPED));

  EXPECT_CALL(mock_dhcp_client_controller_, StartClient(Eq("br0"))).WillOnce(Return(Status(StatusCode::ERROR)));

  Status status = ip_configurator_->Configure(config);

  EXPECT_EQ(StatusCode::ERROR, status.Get());
}

TEST_F(AnIPConfigurator, TriesToSetThreeStaticIPConfigurations) {

  IPConfigs config { { "br0", IPSource::STATIC, "192.168.1.17", "255.255.255.0", "192.168.1.255" }, { "br1",
      IPSource::STATIC, "", "", "" }, { "br3", IPSource::STATIC, "", "", "" } };

  EXPECT_CALL(mock_interface_information_, IsBridge(Eq("br0"))).WillOnce(Return(true));
  EXPECT_CALL(mock_interface_information_, IsBridge(Eq("br1"))).WillOnce(Return(true));
  EXPECT_CALL(mock_interface_information_, IsBridge(Eq("br3"))).WillOnce(Return(true));

  EXPECT_CALL(mock_interface_information_, HasAnyInterfacesUp(Eq("br0"))).WillOnce(Return(true));
  EXPECT_CALL(mock_interface_information_, HasAnyInterfacesUp(Eq("br1"))).WillOnce(Return(true));
  EXPECT_CALL(mock_interface_information_, HasAnyInterfacesUp(Eq("br3"))).WillOnce(Return(true));

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

  EXPECT_CALL(mock_interface_information_, IsBridge(Eq("br0"))).WillOnce(Return(true));
  EXPECT_CALL(mock_interface_information_, IsBridge(Eq("br1"))).WillOnce(Return(true));
  EXPECT_CALL(mock_interface_information_, IsBridge(Eq("br3"))).WillOnce(Return(true));

  EXPECT_CALL(mock_interface_information_, HasAnyInterfacesUp(Eq("br0"))).WillOnce(Return(true));
  EXPECT_CALL(mock_interface_information_, HasAnyInterfacesUp(Eq("br1"))).WillOnce(Return(true));
  EXPECT_CALL(mock_interface_information_, HasAnyInterfacesUp(Eq("br3"))).WillOnce(Return(true));

  EXPECT_CALL(mock_ip_controller_, SetIPConfig(Eq(config[0]))).WillOnce(Return(Status(StatusCode::ERROR)));
  IPConfig config_dhcp_flush = { "br1", IPSource::TEMPORARY, "0.0.0.0", "0.0.0.0" };
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

  IPConfig config = { "br0", IPSource::NONE, "192.168.1.17", "255.255.255.0", "192.168.1.255" };

  EXPECT_CALL(mock_ip_controller_, GetIPConfig(Eq("br0"),_)).WillOnce(
      DoAll(SetArgReferee<1>(config), Return(Status(StatusCode::OK))));

  EXPECT_CALL(mock_dhcp_client_controller_, GetStatus(Eq("br0"))).WillOnce(Return(DHCPClientStatus::STOPPED));

  EXPECT_CALL(mock_bootp_controller_, GetStatus(Eq("br0"))).WillOnce(Return(BootpClientStatus::STOPPED));

  Bridges bridges = { { "br0" } };
  IPConfigs configs = ip_configurator_->GetConfigurations(bridges);

  IPConfig config_expected = { "br0", IPSource::STATIC, "192.168.1.17", "255.255.255.0", "192.168.1.255" };
  EXPECT_TRUE(configs[0] == config_expected);
}

TEST_F(AnIPConfigurator, SuccessfullyGetsMultipleIPConfigurations) {

  IPConfigs configs { { "br0", IPSource::NONE, "192.168.1.17", "255.255.255.0", "192.168.1.255" }, { "br1",
      IPSource::NONE, "", "", "" } };

  EXPECT_CALL(mock_ip_controller_, GetIPConfig(Eq("br0"), _)).WillOnce(
      DoAll(SetArgReferee<1>(configs[0]), Return(Status(StatusCode::OK))));

  EXPECT_CALL(mock_dhcp_client_controller_, GetStatus(Eq("br0"))).WillOnce(Return(DHCPClientStatus::STOPPED));

  EXPECT_CALL(mock_bootp_controller_, GetStatus(Eq("br0"))).WillOnce(Return(BootpClientStatus::STOPPED));

  EXPECT_CALL(mock_bootp_controller_, GetStatus(Eq("br1"))).WillOnce(Return(BootpClientStatus::STOPPED));

  EXPECT_CALL(mock_ip_controller_, GetIPConfig(Eq("br1"), _)).WillOnce(
      DoAll(SetArgReferee<1>(configs[1]), Return(Status(StatusCode::OK))));

  EXPECT_CALL(mock_dhcp_client_controller_, GetStatus(Eq("br1"))).WillOnce(Return(DHCPClientStatus::RUNNING));

  Bridges bridges = { { "br0" }, { "br1" } };
  IPConfigs configs_returned = ip_configurator_->GetConfigurations(bridges);

  IPConfigs configs_expected { { "br0", IPSource::STATIC, "192.168.1.17", "255.255.255.0", "192.168.1.255" }, { "br1",
      IPSource::DHCP, "", "", "" } };

  EXPECT_TRUE(configs_returned[0] == configs_expected[0]);
  EXPECT_TRUE(configs_returned[1] == configs_expected[1]);
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

  EXPECT_CALL(mock_dhcp_client_controller_, GetStatus(Eq("br0"))).WillOnce(Return(DHCPClientStatus::STOPPED));

  Bridges bridges = { { "br0" }, { "br1" }, { "br3" } };
  IPConfigs configs_returned = ip_configurator_->GetConfigurations(bridges);

  EXPECT_TRUE(configs_returned.empty());
}

} /* namespace netconfd */
