// SPDX-License-Identifier: GPL-2.0-or-later
//------------------------------------------------------------------------------
///  \file     IPValidatorTest.cpp
///
///  \brief    <short description of the file contents>
///
///  \author   <author> : WAGO Kontakttechnik GmbH & Co. KG
//------------------------------------------------------------------------------

#include "IPValidator.hpp"

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <string>
#include <boost/asio/ip/network_v4.hpp>
#include <boost/asio/ip/address_v4.hpp>


#include "MockIIPController.hpp"
#include "NetDev.hpp"

using namespace std::literals;
using namespace testing;

namespace netconfd {

class AnIPValidator : public Test {
 public:
  MockIIPController mock_ip_controller_;
  IPValidator validator_;

  AnIPValidator()
      : validator_ { mock_ip_controller_ } {

  }

  void SetUp() override {
  }

  void TearDown() override {

  }

};

class AnIPValidatorP : public AnIPValidator, public testing::WithParamInterface<uint16_t> {
};

TEST_F(AnIPValidator, ValidatesAnIPConfig) {

  NetDevs actual_interfaces = { std::make_shared<NetDev>(0, "br0", NetDev::Kind::Bridge) };
  Interfaces not_assignable_interfaces = { };
  IPConfigs ip_configs = { { "br0", IPSource::STATIC, "192.168.1.1", "255.255.255.0", "192.168.1.255" } };
  bool isInterfaceApplicableToSystem = validator_.IsInterfaceApplicableToSystem(ip_configs, actual_interfaces,
                                                                                not_assignable_interfaces);

  EXPECT_TRUE(isInterfaceApplicableToSystem);
}

TEST_F(AnIPValidator, ValidatesAnIPConfigWithoutBroadcast) {

  NetDevs actual_interfaces = { std::make_shared<NetDev>(0, "br0", NetDev::Kind::Bridge) };
  Interfaces not_assignable_interfaces = { };
  IPConfigs ip_configs = { { "br0", IPSource::STATIC, "192.168.1.1", "255.255.255.0" } };
  bool isInterfaceApplicableToSystem = validator_.IsInterfaceApplicableToSystem(ip_configs, actual_interfaces,
                                                                                not_assignable_interfaces);

  EXPECT_TRUE(isInterfaceApplicableToSystem);
}

TEST_F(AnIPValidator, ValidatesIPConfigs) {

  NetDevs actual_interfaces = { std::make_shared<NetDev>(0, "br0", NetDev::Kind::Bridge), std::make_shared<NetDev>(
       0, "br1", NetDev::Kind::Bridge) };
  Interfaces not_assignable_interfaces = { };
  IPConfigs ip_configs = { { "br0", IPSource::STATIC, "192.168.1.1", "255.255.255.0", "192.168.1.255" }, { "br1",
      IPSource::STATIC, "192.168.2.1", "255.255.255.0", "192.168.2.255" } };
  bool isInterfaceApplicableToSystem = validator_.IsInterfaceApplicableToSystem(ip_configs, actual_interfaces,
                                                                                not_assignable_interfaces);

  EXPECT_TRUE(isInterfaceApplicableToSystem);
}

TEST_F(AnIPValidator, ValidatesIPConfigsThatResetsIPParameterToZero) {

  NetDevs actual_interfaces = { std::make_shared<NetDev>(0, "br0", NetDev::Kind::Bridge), std::make_shared<NetDev>(
       0,"br1", NetDev::Kind::Bridge), std::make_shared<NetDev>( 0, "br2", NetDev::Kind::Ethernet) };
  Interfaces not_assignable_interfaces = { };
  IPConfigs ip_configs = { { "br0", IPSource::STATIC, ZeroIP, ZeroIP }, { "br1", IPSource::STATIC, "192.168.2.1",
      "255.255.255.0" }, { "br2", IPSource::STATIC, ZeroIP, ZeroIP } };
  bool isInterfaceApplicableToSystem = validator_.IsInterfaceApplicableToSystem(ip_configs, actual_interfaces,
                                                                                not_assignable_interfaces);

  EXPECT_TRUE(isInterfaceApplicableToSystem);
}

TEST_F(AnIPValidator, FailedToValidatesStaticAnIPConfigWhichContainsAnInterfaceThatNotExist) {

  NetDevs actual_interfaces = { };
  Interfaces not_assignable_interfaces = { };
  IPConfigs ip_configs = { { "ethX1", IPSource::STATIC, "192.168.1.1", "255.255.255.0", "192.168.1.255" } };
  bool isInterfaceApplicableToSystem = validator_.IsInterfaceApplicableToSystem(ip_configs, actual_interfaces,
                                                                                not_assignable_interfaces);

  EXPECT_FALSE(isInterfaceApplicableToSystem);
}

TEST_F(AnIPValidator, FailedToValidatesADHCPIPConfigWhichContainsAnInterfaceThatNotExist) {

  NetDevs actual_interfaces = { };
  Interfaces not_assignable_interfaces = { };
  IPConfigs ip_configs = { { "ethX1", IPSource::DHCP, "192.168.1.1", "255.255.255.0", "192.168.1.255" } };
  bool isInterfaceApplicableToSystem = validator_.IsInterfaceApplicableToSystem(ip_configs, actual_interfaces,
                                                                                not_assignable_interfaces);

  EXPECT_FALSE(isInterfaceApplicableToSystem);
}

TEST_F(AnIPValidator, FailedToValidatesABootpIPConfigWhichContainsAnInterfaceThatNotExist) {

  NetDevs actual_interfaces = { };
  Interfaces not_assignable_interfaces = { };
  IPConfigs ip_configs = { { "ethX1", IPSource::BOOTP, "192.168.1.1", "255.255.255.0", "192.168.1.255" } };
  bool isInterfaceApplicableToSystem = validator_.IsInterfaceApplicableToSystem(ip_configs, actual_interfaces,
                                                                                not_assignable_interfaces);

  EXPECT_FALSE(isInterfaceApplicableToSystem);
}

TEST_F(AnIPValidator, FailedToValidateAStaticIPConfigWhichContainsAnInterfaceThatIsNotAssignable) {

  NetDevs actual_interfaces = { std::make_shared<NetDev>(0, "ethX1", NetDev::Kind::Ethernet) };
  Interfaces not_assignable_interfaces = { "ethX1" };
  IPConfigs ip_configs = { { "ethX1", IPSource::STATIC, "192.168.1.1", "255.255.255.0", "192.168.1.255" } };
  bool isInterfaceApplicableToSystem = validator_.IsInterfaceApplicableToSystem(ip_configs, actual_interfaces,
                                                                                not_assignable_interfaces);

  EXPECT_FALSE(isInterfaceApplicableToSystem);
}

TEST_F(AnIPValidator, FailedToValidateADHCPIPConfigWhichContainsAnInterfaceThatIsNotAssignable) {

  NetDevs actual_interfaces = { std::make_shared<NetDev>( 0, "ethX1", NetDev::Kind::Ethernet) };
  Interfaces not_assignable_interfaces = { "ethX1" };
  IPConfigs ip_configs = { { "ethX1", IPSource::DHCP, "192.168.1.1", "255.255.255.0", "192.168.1.255" } };
  bool isInterfaceApplicableToSystem = validator_.IsInterfaceApplicableToSystem(ip_configs, actual_interfaces,
                                                                                not_assignable_interfaces);

  EXPECT_FALSE(isInterfaceApplicableToSystem);
}

TEST_F(AnIPValidator, FailedToValidateABootpIPConfigWhichContainsAnInterfaceThatIsNotAssignable) {

  NetDevs actual_interfaces = { std::make_shared<NetDev>( 0, "ethX1", NetDev::Kind::Ethernet) };
  Interfaces not_assignable_interfaces = { "ethX1" };
  IPConfigs ip_configs = { { "ethX1", IPSource::BOOTP, "192.168.1.1", "255.255.255.0", "192.168.1.255" } };
  bool isInterfaceApplicableToSystem = validator_.IsInterfaceApplicableToSystem(ip_configs, actual_interfaces,
                                                                                not_assignable_interfaces);

  EXPECT_FALSE(isInterfaceApplicableToSystem);
}

TEST_F(AnIPValidator, FailedToValidateAnIPConfigWhichContainsOneInterfaceSeveralTimes) {

  NetDevs actual_interfaces = { std::make_shared<NetDev>( 0, "ethX1", NetDev::Kind::Ethernet) };
  Interfaces not_assignable_interfaces = { };
  IPConfigs ip_configs = { { "ethX1", IPSource::STATIC, "192.168.2.1", "255.255.255.0", "192.168.2.255" }, { "ethX1",
      IPSource::STATIC, "192.168.1.1", "255.255.255.0", "192.168.1.255" } };

  bool isInterfaceApplicableToSystem = validator_.IsInterfaceApplicableToSystem(ip_configs, actual_interfaces,
                                                                                not_assignable_interfaces);
  EXPECT_TRUE(isInterfaceApplicableToSystem);

  Status status = validator_.ValidateIPConfigs(ip_configs, true);
  EXPECT_EQ(StatusCode::INVALID_PARAMETER, status.Get());
}

TEST_F(AnIPValidator, AssigningNetworkAddressAsHostIsAccepted) {

  // This is an exception for the wwan0 interface, the modem assigns such addresses :-(

  NetDevs actual_interfaces = { std::make_shared<NetDev>( 0, "br0", NetDev::Kind::Ethernet) };
  Interfaces not_assignable_interfaces = { };
  IPConfigs ip_configs = { { "br0", IPSource::STATIC, "192.168.1.128", "255.255.255.128", "192.168.1.255" }};

  bool isInterfaceApplicableToSystem = validator_.IsInterfaceApplicableToSystem(ip_configs, actual_interfaces,
                                                                                not_assignable_interfaces);
  EXPECT_TRUE(isInterfaceApplicableToSystem);

  Status status = validator_.ValidateIPConfigs(ip_configs, true);
  EXPECT_EQ(StatusCode::OK, status.Get());
}

TEST_F(AnIPValidator, FailedToValidateAnIPConfigWhichContainsAnInvalidAddress) {

  NetDevs actual_interfaces = { std::make_shared<NetDev>( 0, "ethX1", NetDev::Kind::Ethernet) };
  Interfaces not_assignable_interfaces = { };
  IPConfigs ip_configs = { { "ethX1", IPSource::STATIC, "192.168.1.1111", "255.255.255.0", "192.168.1.255" } };
  bool isInterfaceApplicableToSystem = validator_.IsInterfaceApplicableToSystem(ip_configs, actual_interfaces,
                                                                                not_assignable_interfaces);
  EXPECT_TRUE(isInterfaceApplicableToSystem);

  Status status = validator_.ValidateIPConfigs(ip_configs, true);
  EXPECT_EQ(StatusCode::INVALID_PARAMETER, status.Get());
}

TEST_F(AnIPValidator, FailedToValidateAnIPConfigWhichAddressContainsOnlyOnes) {

  NetDevs actual_interfaces = { std::make_shared<NetDev>( 0, "ethX1", NetDev::Kind::Ethernet) };
  Interfaces not_assignable_interfaces = { };
  IPConfigs ip_configs = { { "ethX1", IPSource::STATIC, "255.255.255.255", "255.255.255.0", "255.255.255.0" } };
  bool isInterfaceApplicableToSystem = validator_.IsInterfaceApplicableToSystem(ip_configs, actual_interfaces,
                                                                                not_assignable_interfaces);
  EXPECT_TRUE(isInterfaceApplicableToSystem);

  Status status = validator_.ValidateIPConfigs(ip_configs, true);
  EXPECT_EQ(StatusCode::INVALID_PARAMETER, status.Get());
}

TEST_F(AnIPValidator, FailedToValidateAnIPConfigWhichAddressContainsZeros) {

  NetDevs actual_interfaces = { std::make_shared<NetDev>( 0, "ethX1", NetDev::Kind::Ethernet) };
  Interfaces not_assignable_interfaces = { };
  IPConfigs ip_configs = { { "ethX1", IPSource::STATIC, ZeroIP, "255.255.255.0", "0.0.0.255" } };
  bool isInterfaceApplicableToSystem = validator_.IsInterfaceApplicableToSystem(ip_configs, actual_interfaces,
                                                                                not_assignable_interfaces);
  EXPECT_TRUE(isInterfaceApplicableToSystem);

  Status status = validator_.ValidateIPConfigs(ip_configs, true);
  EXPECT_EQ(StatusCode::INVALID_PARAMETER, status.Get());
}

TEST_F(AnIPValidator, FailedToValidateAnIPConfigWhichContainsAnInvalidNetmask) {

  NetDevs actual_interfaces = { std::make_shared<NetDev>( 0, "ethX1", NetDev::Kind::Ethernet) };
  Interfaces not_assignable_interfaces = { };
  IPConfigs ip_configs = { { "ethX1", IPSource::STATIC, "192.168.1.1", "255.X.255.0", "192.168.1.255" } };
  bool isInterfaceApplicableToSystem = validator_.IsInterfaceApplicableToSystem(ip_configs, actual_interfaces,
                                                                                not_assignable_interfaces);

  EXPECT_TRUE(isInterfaceApplicableToSystem);

  Status status = validator_.ValidateIPConfigs(ip_configs, true);
  EXPECT_EQ(StatusCode::INVALID_PARAMETER, status.Get());
}

TEST_F(AnIPValidator, FailedToValidateAnIPConfigWhichContainsZeroNetmask) {

  NetDevs actual_interfaces = { std::make_shared<NetDev>( 0, "ethX1", NetDev::Kind::Ethernet) };
  Interfaces not_assignable_interfaces = { };
  IPConfigs ip_configs = { { "ethX1", IPSource::STATIC, "192.168.1.1", ZeroIP, "192.168.1.255" } };
  bool isInterfaceApplicableToSystem = validator_.IsInterfaceApplicableToSystem(ip_configs, actual_interfaces,
                                                                                not_assignable_interfaces);
  EXPECT_TRUE(isInterfaceApplicableToSystem);

  Status status = validator_.ValidateIPConfigs(ip_configs, true);
  EXPECT_EQ(StatusCode::INVALID_PARAMETER, status.Get());
}

TEST_F(AnIPValidator, FailedToValidateAnIPConfigWhichSubnetmaskContainsZeros) {

  NetDevs actual_interfaces = { std::make_shared<NetDev>( 0, "ethX1", NetDev::Kind::Ethernet) };
  Interfaces not_assignable_interfaces = { };
  IPConfigs ip_configs = { { "ethX1", IPSource::STATIC, "192.168.1.1", "255.111.255.0", "192.168.1.255" } };
  bool isInterfaceApplicableToSystem = validator_.IsInterfaceApplicableToSystem(ip_configs, actual_interfaces,
                                                                                not_assignable_interfaces);
  EXPECT_TRUE(isInterfaceApplicableToSystem);

  Status status = validator_.ValidateIPConfigs(ip_configs, true);
  EXPECT_EQ(StatusCode::INVALID_PARAMETER, status.Get());
}

TEST_F(AnIPValidator, FailedToValidateAnIPConfigWhichContainsAnInvalidBroadcast) {

  NetDevs actual_interfaces = { std::make_shared<NetDev>( 0, "ethX1", NetDev::Kind::Ethernet) };
  Interfaces not_assignable_interfaces = { };
  IPConfigs ip_configs = { { "ethX1", IPSource::STATIC, "192.168.1.1", "255.255.255.0", "42" } };
  bool isInterfaceApplicableToSystem = validator_.IsInterfaceApplicableToSystem(ip_configs, actual_interfaces,
                                                                                not_assignable_interfaces);
  EXPECT_TRUE(isInterfaceApplicableToSystem);

  Status status = validator_.ValidateIPConfigs(ip_configs, true);
  EXPECT_EQ(StatusCode::INVALID_PARAMETER, status.Get());
}

TEST_F(AnIPValidator, FailedToValidateAnIPConfigWhichBroadcastDoesNotMatchTheIPAddress) {

  NetDevs actual_interfaces = { std::make_shared<NetDev>( 0, "ethX1", NetDev::Kind::Ethernet) };
  Interfaces not_assignable_interfaces = { };
  IPConfigs ip_configs = { { "ethX1", IPSource::STATIC, "192.168.1.1", "255.255.0.0", "172.168.255.255" } };
  bool isInterfaceApplicableToSystem = validator_.IsInterfaceApplicableToSystem(ip_configs, actual_interfaces,
                                                                                not_assignable_interfaces);
  EXPECT_TRUE(isInterfaceApplicableToSystem);

  Status status = validator_.ValidateIPConfigs(ip_configs, true);
  EXPECT_EQ(StatusCode::INVALID_PARAMETER, status.Get());
}

TEST_F(AnIPValidator, FailedToValidateAnIPConfigWhichBroadcastDoesNotMatchTheNetmask) {

  NetDevs actual_interfaces = { std::make_shared<NetDev>( 0, "ethX1", NetDev::Kind::Ethernet) };
  Interfaces not_assignable_interfaces = { };
  IPConfigs ip_configs = { { "ethX1", IPSource::STATIC, "192.168.1.1", "255.255.255.0", "192.168.255.255" } };
  bool isInterfaceApplicableToSystem = validator_.IsInterfaceApplicableToSystem(ip_configs, actual_interfaces,
                                                                                not_assignable_interfaces);
  EXPECT_TRUE(isInterfaceApplicableToSystem);

  Status status = validator_.ValidateIPConfigs(ip_configs, true);
  EXPECT_EQ(StatusCode::INVALID_PARAMETER, status.Get());
}

TEST_F(AnIPValidator, FailedToValidateIPConfigsWithSeveralIpAddresses) {

  NetDevs actual_interfaces = { std::make_shared<NetDev>( 0, "ethX1", NetDev::Kind::Ethernet), std::make_shared<NetDev>(
       0,"ethX2", NetDev::Kind::Ethernet), std::make_shared<NetDev>( 0, "ethX3", NetDev::Kind::Ethernet) };
  Interfaces not_assignable_interfaces = { };
  IPConfigs ip_configs = { { "ethX1", IPSource::STATIC, "192.168.1.1", "255.255.255.0", "192.168.1.255" }, { "ethX2",
      IPSource::STATIC, "192.168.2.1", "255.255.0.0", "192.168.255.255" }, { "ethX3", IPSource::STATIC, "192.168.1.1",
      "255.0.0.0", "192.255.255.255" } };
  bool isInterfaceApplicableToSystem = validator_.IsInterfaceApplicableToSystem(ip_configs, actual_interfaces,
                                                                                not_assignable_interfaces);
  EXPECT_TRUE(isInterfaceApplicableToSystem);

  Status status = validator_.ValidateIPConfigs(ip_configs, true);
  EXPECT_EQ(StatusCode::INVALID_PARAMETER, status.Get());
}

TEST_F(AnIPValidator, FailedToValidateIPConfigsWithSeveralNetworks) {

  NetDevs actual_interfaces = { std::make_shared<NetDev>( 0, "ethX1", NetDev::Kind::Ethernet), std::make_shared<NetDev>(
       0, "ethX2", NetDev::Kind::Ethernet), std::make_shared<NetDev>( 0, "ethX3", NetDev::Kind::Ethernet) };
  Interfaces not_assignable_interfaces = { };
  IPConfigs ip_configs = { { "ethX1", IPSource::STATIC, "192.168.1.1", "255.255.255.0", "192.168.1.255" }, { "ethX2",
      IPSource::STATIC, "192.168.1.2", "255.255.255.0", "192.168.1.255" }, { "ethX3", IPSource::STATIC, "192.168.2.1",
      "255.255.255.0", "192.168.2.255" } };
  bool isInterfaceApplicableToSystem = validator_.IsInterfaceApplicableToSystem(ip_configs, actual_interfaces,
                                                                                not_assignable_interfaces);
  EXPECT_TRUE(isInterfaceApplicableToSystem);

  Status status = validator_.ValidateIPConfigs(ip_configs, true);
  EXPECT_EQ(StatusCode::INVALID_PARAMETER, status.Get());
}

TEST_F(AnIPValidator, FailedToValidateAnIPConfigWhichIPIsAlreadyAssignedOnTheSystem) {

  IPConfig actual_ip_config = { "ethX1", IPSource::STATIC, "192.168.1.1", "255.255.255.0", "192.168.1.255" };
  EXPECT_CALL(mock_ip_controller_, GetIPConfig(Eq("ethX1"),_)).WillOnce(
      DoAll(SetArgReferee<1>(actual_ip_config), Return(Status())));

  NetDevs actual_interfaces = { std::make_shared<NetDev>( 0, "ethX1", NetDev::Kind::Ethernet), std::make_shared<NetDev>(
       0, "ethX2", NetDev::Kind::Ethernet) };
  Interfaces not_assignable_interfaces = { };
  IPConfigs ip_configs = { { "ethX2", IPSource::STATIC, "192.168.1.1", "255.255.0.0", "192.168.255.255" } };

  bool isInterfaceApplicableToSystem = validator_.IsInterfaceApplicableToSystem(ip_configs, actual_interfaces,
                                                                                not_assignable_interfaces);
  EXPECT_TRUE(isInterfaceApplicableToSystem);

  Status status = validator_.ValidateIPConfigIsApplicableToSystem(ip_configs, actual_interfaces);
  EXPECT_EQ(StatusCode::INVALID_PARAMETER, status.Get());
}

TEST_F(AnIPValidator, FailedToValidateAnIPConfigWhichNetworkAlreadyExistsOnTheSystem) {

  IPConfig actual_ip_config = { "ethX1", IPSource::STATIC, "192.168.1.2", "255.255.255.0", "192.168.1.255" };
  EXPECT_CALL(mock_ip_controller_, GetIPConfig(Eq("ethX1"),_)).WillOnce(
      DoAll(SetArgReferee<1>(actual_ip_config), Return(Status())));

  NetDevs actual_interfaces = { std::make_shared<NetDev>( 0, "ethX1", NetDev::Kind::Ethernet), std::make_shared<NetDev>(
       0, "ethX2", NetDev::Kind::Ethernet) };
  Interfaces not_assignable_interfaces = { };
  IPConfigs ip_configs = { { "ethX2", IPSource::STATIC, "192.168.1.1", "255.255.255.0", "192.168.1.255" } };

  bool isInterfaceApplicableToSystem = validator_.IsInterfaceApplicableToSystem(ip_configs, actual_interfaces,
                                                                                not_assignable_interfaces);
  EXPECT_TRUE(isInterfaceApplicableToSystem);

  Status status = validator_.ValidateIPConfigIsApplicableToSystem(ip_configs, actual_interfaces);
  EXPECT_EQ(StatusCode::INVALID_PARAMETER, status.Get());
}

TEST_F(AnIPValidator, FailedToValidateMultipleIPConfigsWhichIPAndNetworkAlreadyExistsOnTheSystem) {

  //ip already exist ethX1 == ethX4
  IPConfig actual_ip_config_1 = { "ethX1", IPSource::STATIC, "192.168.1.2", "255.255.255.0", "192.168.1.255" };
  EXPECT_CALL(mock_ip_controller_, GetIPConfig(Eq("ethX1"),_)).WillOnce(
      DoAll(SetArgReferee<1>(actual_ip_config_1), Return(Status())));

  //network already exist ethX2 == ethX5
  IPConfig actual_ip_config_2 = { "ethX2", IPSource::STATIC, "192.168.2.2", "255.255.255.0", "192.168.2.255" };
  EXPECT_CALL(mock_ip_controller_, GetIPConfig(Eq("ethX2"),_)).WillOnce(
      DoAll(SetArgReferee<1>(actual_ip_config_2), Return(Status())));

  //ip and network already exist ethX3 == ethX6
  IPConfig actual_ip_config_3 = { "ethX3", IPSource::STATIC, "192.168.3.2", "255.255.255.0", "192.168.3.255" };
  EXPECT_CALL(mock_ip_controller_, GetIPConfig(Eq("ethX3"),_)).WillOnce(
      DoAll(SetArgReferee<1>(actual_ip_config_3), Return(Status())));

  NetDevs actual_interfaces = { std::make_shared<NetDev>( 0, "ethX1", NetDev::Kind::Ethernet), std::make_shared<NetDev>(
       0, "ethX2", NetDev::Kind::Ethernet), std::make_shared<NetDev>( 0, "ethX3", NetDev::Kind::Ethernet),
      std::make_shared<NetDev>( 0, "ethX4", NetDev::Kind::Ethernet), std::make_shared<NetDev>( 0, "ethX5",
                                                                                             NetDev::Kind::Ethernet),
      std::make_shared<NetDev>( 0, "ethX6", NetDev::Kind::Ethernet) };
  Interfaces not_assignable_interfaces = { };
  IPConfigs ip_configs = { { "ethX4", IPSource::STATIC, "192.168.1.2", "255.255.0.0", "192.168.255.255" }, { "ethX5",
      IPSource::STATIC, "192.168.2.1", "255.255.255.0", "192.168.2.255" }, { "ethX6", IPSource::STATIC, "192.168.3.2",
      "255.255.255.0", "192.168.3.255" } };

  bool isInterfaceApplicableToSystem = validator_.IsInterfaceApplicableToSystem(ip_configs, actual_interfaces,
                                                                                not_assignable_interfaces);
  EXPECT_TRUE(isInterfaceApplicableToSystem);

  Status status = validator_.ValidateIPConfigIsApplicableToSystem(ip_configs, actual_interfaces);
  EXPECT_EQ(StatusCode::INVALID_PARAMETER, status.Get());
}

TEST_F(AnIPValidator, DoesNotValidateIPParameterOfDHCPBootpAndNoneIPSources) {

  NetDevs actual_interfaces = { std::make_shared<NetDev>( 0, "ethX1", NetDev::Kind::Ethernet), std::make_shared<NetDev>(
       0, "ethX2", NetDev::Kind::Ethernet), std::make_shared<NetDev>( 0, "ethX3", NetDev::Kind::Ethernet) };
  Interfaces not_assignable_interfaces = { };
  IPConfigs ip_configs = { { "ethX1", IPSource::NONE, ZeroIP, ZeroIP, ZeroIP }, { "ethX2", IPSource::DHCP,
      ZeroIP, ZeroIP, ZeroIP }, { "ethX3", IPSource::BOOTP, ZeroIP, ZeroIP, ZeroIP } };
  bool isInterfaceApplicableToSystem = validator_.IsInterfaceApplicableToSystem(ip_configs, actual_interfaces,
                                                                                not_assignable_interfaces);

  EXPECT_TRUE(isInterfaceApplicableToSystem);
}

TEST_F(AnIPValidator, ShouldAcceptRFC3021Ips) {
  // RFC3021 defines that subnet mask 255.255.255.254 contains exactly 2 hosts without broadcast address.
  // This is used in Point-to-Point IP links like modems.
  // Further 255.255.255.255 is used to define exactly one IP address.
  auto netmask_30 = IPConfigs {{ "wwan0", IPSource::TEMPORARY, "10.12.88.128", "255.255.255.254"},
  { "wwan1", IPSource::TEMPORARY, "10.12.88.133", "255.255.255.254"},
  { "wwan2", IPSource::TEMPORARY, "10.12.88.130", "255.255.255.255"}};

  auto status = validator_.ValidateIPConfigs(netmask_30, true);

  EXPECT_EQ(StatusCode::OK, status.Get()) << status.GetMessage();
}

} /* namespace netconfd */
