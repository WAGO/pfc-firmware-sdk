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

#include "MockIIPController.hpp"

using namespace testing;

namespace netconfd {

class AIPValidator : public Test {
 public:
  MockIIPController mock_ip_controller_;
  IPValidator validator_;

  AIPValidator()
      : validator_ { mock_ip_controller_ } {

  }

  void SetUp() override {
  }

  void TearDown() override {

  }

};

TEST_F(AIPValidator, ValidatesAnIPConfig) {

  Interfaces actual_interfaces = { "br0" };
  Interfaces not_assignable_interfaces = { };
  IPConfigs ip_configs = { { "br0", IPSource::STATIC, "192.168.1.1",
      "255.255.255.0", "192.168.1.255" } };
  bool isInterfaceApplicableToSystem = validator_.IsInterfaceApplicableToSystem(
      ip_configs, actual_interfaces, not_assignable_interfaces);

  EXPECT_TRUE(isInterfaceApplicableToSystem);
}

TEST_F(AIPValidator, ValidatesAnIPConfigWithoutBroadcast) {

  Interfaces actual_interfaces = { "br0" };
  Interfaces not_assignable_interfaces = { };
  IPConfigs ip_configs = { { "br0", IPSource::STATIC, "192.168.1.1",
      "255.255.255.0" } };
  bool isInterfaceApplicableToSystem = validator_.IsInterfaceApplicableToSystem(
      ip_configs, actual_interfaces, not_assignable_interfaces);

  EXPECT_TRUE(isInterfaceApplicableToSystem);
}

TEST_F(AIPValidator, ValidatesIPConfigs) {

  Interfaces actual_interfaces = { "br0", "br1" };
  Interfaces not_assignable_interfaces = { };
  IPConfigs ip_configs = { { "br0", IPSource::STATIC, "192.168.1.1",
      "255.255.255.0", "192.168.1.255" }, { "br1", IPSource::STATIC,
      "192.168.2.1", "255.255.255.0", "192.168.2.255" } };
  bool isInterfaceApplicableToSystem = validator_.IsInterfaceApplicableToSystem(
      ip_configs, actual_interfaces, not_assignable_interfaces);

  EXPECT_TRUE(isInterfaceApplicableToSystem);
}

TEST_F(AIPValidator, ValidatesIPConfigsThatResetsIPParameterToZero) {

  Interfaces actual_interfaces = { "br0", "br1", "br2" };
  Interfaces not_assignable_interfaces = { };
  IPConfigs ip_configs = { { "br0", IPSource::STATIC, "0.0.0.0", "0.0.0.0" }, {
      "br1", IPSource::STATIC, "192.168.2.1", "255.255.255.0" }, { "br2",
      IPSource::STATIC, "0.0.0.0", "0.0.0.0" } };
  bool isInterfaceApplicableToSystem = validator_.IsInterfaceApplicableToSystem(
      ip_configs, actual_interfaces, not_assignable_interfaces);

  EXPECT_TRUE(isInterfaceApplicableToSystem);
}

TEST_F(AIPValidator, FailedToValidatesStaticAnIPConfigWhichContainsAnInterfaceThatNotExist) {

  Interfaces actual_interfaces = { };
  Interfaces not_assignable_interfaces = { };
  IPConfigs ip_configs = { { "ethX1", IPSource::STATIC, "192.168.1.1",
      "255.255.255.0", "192.168.1.255" } };
  bool isInterfaceApplicableToSystem = validator_.IsInterfaceApplicableToSystem(
      ip_configs, actual_interfaces, not_assignable_interfaces);

  EXPECT_FALSE(isInterfaceApplicableToSystem);
}

TEST_F(AIPValidator, FailedToValidatesADHCPIPConfigWhichContainsAnInterfaceThatNotExist) {

  Interfaces actual_interfaces = { };
  Interfaces not_assignable_interfaces = { };
  IPConfigs ip_configs = { { "ethX1", IPSource::DHCP, "192.168.1.1",
      "255.255.255.0", "192.168.1.255" } };
  bool isInterfaceApplicableToSystem = validator_.IsInterfaceApplicableToSystem(
      ip_configs, actual_interfaces, not_assignable_interfaces);

  EXPECT_FALSE(isInterfaceApplicableToSystem);
}

TEST_F(AIPValidator, FailedToValidatesABootpIPConfigWhichContainsAnInterfaceThatNotExist) {

  Interfaces actual_interfaces = { };
  Interfaces not_assignable_interfaces = { };
  IPConfigs ip_configs = { { "ethX1", IPSource::BOOTP, "192.168.1.1",
      "255.255.255.0", "192.168.1.255" } };
  bool isInterfaceApplicableToSystem = validator_.IsInterfaceApplicableToSystem(
      ip_configs, actual_interfaces, not_assignable_interfaces);

  EXPECT_FALSE(isInterfaceApplicableToSystem);
}

TEST_F(AIPValidator, FailedToValidateAStaticIPConfigWhichContainsAnInterfaceThatIsNotAssignable) {

  Interfaces actual_interfaces = { "ethX1" };
  Interfaces not_assignable_interfaces = { "ethX1" };
  IPConfigs ip_configs = { { "ethX1", IPSource::STATIC, "192.168.1.1",
      "255.255.255.0", "192.168.1.255" } };
  bool isInterfaceApplicableToSystem = validator_.IsInterfaceApplicableToSystem(
      ip_configs, actual_interfaces, not_assignable_interfaces);

  EXPECT_FALSE(isInterfaceApplicableToSystem);
}

TEST_F(AIPValidator, FailedToValidateADHCPIPConfigWhichContainsAnInterfaceThatIsNotAssignable) {

  Interfaces actual_interfaces = { "ethX1" };
  Interfaces not_assignable_interfaces = { "ethX1" };
  IPConfigs ip_configs = { { "ethX1", IPSource::DHCP, "192.168.1.1",
      "255.255.255.0", "192.168.1.255" } };
  bool isInterfaceApplicableToSystem = validator_.IsInterfaceApplicableToSystem(
      ip_configs, actual_interfaces, not_assignable_interfaces);

  EXPECT_FALSE(isInterfaceApplicableToSystem);
}

TEST_F(AIPValidator, FailedToValidateABootpIPConfigWhichContainsAnInterfaceThatIsNotAssignable) {

  Interfaces actual_interfaces = { "ethX1" };
  Interfaces not_assignable_interfaces = { "ethX1" };
  IPConfigs ip_configs = { { "ethX1", IPSource::BOOTP, "192.168.1.1",
      "255.255.255.0", "192.168.1.255" } };
  bool isInterfaceApplicableToSystem = validator_.IsInterfaceApplicableToSystem(
      ip_configs, actual_interfaces, not_assignable_interfaces);

  EXPECT_FALSE(isInterfaceApplicableToSystem);
}

TEST_F(AIPValidator, FailedToValidateAnIPConfigWhichContainsOneInterfaceSeveralTimes) {

  Interfaces actual_interfaces = { "ethX1" };
  Interfaces not_assignable_interfaces = { };
  IPConfigs ip_configs = { { "ethX1", IPSource::STATIC, "192.168.2.1",
      "255.255.255.0", "192.168.2.255" }, { "ethX1", IPSource::STATIC,
      "192.168.1.1", "255.255.255.0", "192.168.1.255" } };

  bool isInterfaceApplicableToSystem = validator_.IsInterfaceApplicableToSystem(
      ip_configs, actual_interfaces, not_assignable_interfaces);
  EXPECT_TRUE(isInterfaceApplicableToSystem);

  Status status = validator_.ValidateIPConfigs(ip_configs, true);
  EXPECT_EQ(StatusCode::INVALID_PARAMETER, status.Get());
}

TEST_F(AIPValidator, FailedToValidateAnIPConfigWhichContainsAnInvalidAddress) {

  Interfaces actual_interfaces = { "ethX1" };
  Interfaces not_assignable_interfaces = { };
  IPConfigs ip_configs = { { "ethX1", IPSource::STATIC, "192.168.1.1111",
      "255.255.255.0", "192.168.1.255" } };
  bool isInterfaceApplicableToSystem = validator_.IsInterfaceApplicableToSystem(
      ip_configs, actual_interfaces, not_assignable_interfaces);
  EXPECT_TRUE(isInterfaceApplicableToSystem);

  Status status = validator_.ValidateIPConfigs(ip_configs, true);
  EXPECT_EQ(StatusCode::INVALID_PARAMETER, status.Get());
}

TEST_F(AIPValidator, FailedToValidateAnIPConfigWhichAddressContainsOnlyOnes) {

  Interfaces actual_interfaces = { "ethX1" };
  Interfaces not_assignable_interfaces = { };
  IPConfigs ip_configs = { { "ethX1", IPSource::STATIC, "255.255.255.255",
      "255.255.255.0", "255.255.255.0" } };
  bool isInterfaceApplicableToSystem = validator_.IsInterfaceApplicableToSystem(
      ip_configs, actual_interfaces, not_assignable_interfaces);
  EXPECT_TRUE(isInterfaceApplicableToSystem);

  Status status = validator_.ValidateIPConfigs(ip_configs, true);
  EXPECT_EQ(StatusCode::INVALID_PARAMETER, status.Get());
}

TEST_F(AIPValidator, FailedToValidateAnIPConfigWhichAddressContainsZeros) {

  Interfaces actual_interfaces = { "ethX1" };
  Interfaces not_assignable_interfaces = { };
  IPConfigs ip_configs = { { "ethX1", IPSource::STATIC, "0.0.0.0",
      "255.255.255.0", "0.0.0.255" } };
  bool isInterfaceApplicableToSystem = validator_.IsInterfaceApplicableToSystem(
      ip_configs, actual_interfaces, not_assignable_interfaces);
  EXPECT_TRUE(isInterfaceApplicableToSystem);

  Status status = validator_.ValidateIPConfigs(ip_configs, true);
  EXPECT_EQ(StatusCode::INVALID_PARAMETER, status.Get());
}

TEST_F(AIPValidator, FailedToValidateAnIPConfigWhichContainsAnInvalidNetmask) {

  Interfaces actual_interfaces = { "ethX1" };
  Interfaces not_assignable_interfaces = { };
  IPConfigs ip_configs = { { "ethX1", IPSource::STATIC, "192.168.1.1",
      "255.X.255.0", "192.168.1.255" } };
  bool isInterfaceApplicableToSystem = validator_.IsInterfaceApplicableToSystem(
      ip_configs, actual_interfaces, not_assignable_interfaces);

  EXPECT_TRUE(isInterfaceApplicableToSystem);

  Status status = validator_.ValidateIPConfigs(ip_configs, true);
  EXPECT_EQ(StatusCode::INVALID_PARAMETER, status.Get());
}

TEST_F(AIPValidator, FailedToValidateAnIPConfigWhichContainsZeroNetmask) {

  Interfaces actual_interfaces = { "ethX1" };
  Interfaces not_assignable_interfaces = { };
  IPConfigs ip_configs = { { "ethX1", IPSource::STATIC, "192.168.1.1",
      "0.0.0.0", "192.168.1.255" } };
  bool isInterfaceApplicableToSystem = validator_.IsInterfaceApplicableToSystem(
      ip_configs, actual_interfaces, not_assignable_interfaces);
  EXPECT_TRUE(isInterfaceApplicableToSystem);

  Status status = validator_.ValidateIPConfigs(ip_configs, true);
  EXPECT_EQ(StatusCode::INVALID_PARAMETER, status.Get());
}

TEST_F(AIPValidator, FailedToValidateAnIPConfigWhichNetmaskContainsOnlyOnes) {

  Interfaces actual_interfaces = { "ethX1" };
  Interfaces not_assignable_interfaces = { };
  IPConfigs ip_configs = { { "ethX1", IPSource::STATIC, "192.168.1.1",
      "255.255.255.255", "192.168.1.1" } };
  bool isInterfaceApplicableToSystem = validator_.IsInterfaceApplicableToSystem(
      ip_configs, actual_interfaces, not_assignable_interfaces);
  EXPECT_TRUE(isInterfaceApplicableToSystem);

  Status status = validator_.ValidateIPConfigs(ip_configs, true);
  EXPECT_EQ(StatusCode::INVALID_PARAMETER, status.Get());
}

TEST_F(AIPValidator, FailedToValidateAnIPConfigWhichSubnetmaskContainsZeros) {

  Interfaces actual_interfaces = { "ethX1" };
  Interfaces not_assignable_interfaces = { };
  IPConfigs ip_configs = { { "ethX1", IPSource::STATIC, "192.168.1.1",
      "255.111.255.0", "192.168.1.255" } };
  bool isInterfaceApplicableToSystem = validator_.IsInterfaceApplicableToSystem(
      ip_configs, actual_interfaces, not_assignable_interfaces);
  EXPECT_TRUE(isInterfaceApplicableToSystem);

  Status status = validator_.ValidateIPConfigs(ip_configs, true);
  EXPECT_EQ(StatusCode::INVALID_PARAMETER, status.Get());
}

TEST_F(AIPValidator, FailedToValidateAnIPConfigWhichContainsAnInvalidBroadcast) {

  Interfaces actual_interfaces = { "ethX1" };
  Interfaces not_assignable_interfaces = { };
  IPConfigs ip_configs = { { "ethX1", IPSource::STATIC, "192.168.1.1",
      "255.255.255.0", "42" } };
  bool isInterfaceApplicableToSystem = validator_.IsInterfaceApplicableToSystem(
      ip_configs, actual_interfaces, not_assignable_interfaces);
  EXPECT_TRUE(isInterfaceApplicableToSystem);

  Status status = validator_.ValidateIPConfigs(ip_configs, true);
  EXPECT_EQ(StatusCode::INVALID_PARAMETER, status.Get());
}

TEST_F(AIPValidator, FailedToValidateAnIPConfigWhichBroadcastDoesNotMatchTheIPAddress) {

  Interfaces actual_interfaces = { "ethX1" };
  Interfaces not_assignable_interfaces = { };
  IPConfigs ip_configs = { { "ethX1", IPSource::STATIC, "192.168.1.1",
      "255.255.0.0", "172.168.255.255" } };
  bool isInterfaceApplicableToSystem = validator_.IsInterfaceApplicableToSystem(
      ip_configs, actual_interfaces, not_assignable_interfaces);
  EXPECT_TRUE(isInterfaceApplicableToSystem);

  Status status = validator_.ValidateIPConfigs(ip_configs, true);
  EXPECT_EQ(StatusCode::INVALID_PARAMETER, status.Get());
}

TEST_F(AIPValidator, FailedToValidateAnIPConfigWhichBroadcastDoesNotMatchTheNetmask) {

  Interfaces actual_interfaces = { "ethX1" };
  Interfaces not_assignable_interfaces = { };
  IPConfigs ip_configs = { { "ethX1", IPSource::STATIC, "192.168.1.1",
      "255.255.255.0", "192.168.255.255" } };
  bool isInterfaceApplicableToSystem = validator_.IsInterfaceApplicableToSystem(
      ip_configs, actual_interfaces, not_assignable_interfaces);
  EXPECT_TRUE(isInterfaceApplicableToSystem);

  Status status = validator_.ValidateIPConfigs(ip_configs, true);
  EXPECT_EQ(StatusCode::INVALID_PARAMETER, status.Get());
}

TEST_F(AIPValidator, FailedToValidateIPConfigsWithSeveralIpAddresses) {

  Interfaces actual_interfaces = { "ethX1", "ethX2", "ethX3" };
  Interfaces not_assignable_interfaces = { };
  IPConfigs ip_configs = { { "ethX1", IPSource::STATIC, "192.168.1.1",
      "255.255.255.0", "192.168.1.255" }, { "ethX2", IPSource::STATIC,
      "192.168.2.1", "255.255.0.0", "192.168.255.255" }, { "ethX3",
      IPSource::STATIC, "192.168.1.1", "255.0.0.0", "192.255.255.255" } };
  bool isInterfaceApplicableToSystem = validator_.IsInterfaceApplicableToSystem(
      ip_configs, actual_interfaces, not_assignable_interfaces);
  EXPECT_TRUE(isInterfaceApplicableToSystem);

  Status status = validator_.ValidateIPConfigs(ip_configs, true);
  EXPECT_EQ(StatusCode::INVALID_PARAMETER, status.Get());
}

TEST_F(AIPValidator, FailedToValidateIPConfigsWithSeveralNetworks) {

  Interfaces actual_interfaces = { "ethX1", "ethX2", "ethX3" };
  Interfaces not_assignable_interfaces = { };
  IPConfigs ip_configs = { { "ethX1", IPSource::STATIC, "192.168.1.1",
      "255.255.255.0", "192.168.1.255" }, { "ethX2", IPSource::STATIC,
      "192.168.1.2", "255.255.255.0", "192.168.1.255" }, { "ethX3",
      IPSource::STATIC, "192.168.2.1", "255.255.255.0", "192.168.2.255" } };
  bool isInterfaceApplicableToSystem = validator_.IsInterfaceApplicableToSystem(
      ip_configs, actual_interfaces, not_assignable_interfaces);
  EXPECT_TRUE(isInterfaceApplicableToSystem);

  Status status = validator_.ValidateIPConfigs(ip_configs, true);
  EXPECT_EQ(StatusCode::INVALID_PARAMETER, status.Get());
}

TEST_F(AIPValidator, FailedToValidateAnIPConfigWhichIPIsAlreadyAssignedOnTheSystem) {

  IPConfig actual_ip_config = { "ethX1", IPSource::STATIC, "192.168.1.1",
      "255.255.255.0", "192.168.1.255" };
  EXPECT_CALL(mock_ip_controller_, GetIPConfig(Eq("ethX1"),_)).WillOnce(
      DoAll(SetArgReferee<1>(actual_ip_config), Return(Status())));

  Interfaces actual_interfaces = { "ethX1", "ethX2" };
  Interfaces not_assignable_interfaces = { };
  IPConfigs ip_configs = { { "ethX2", IPSource::STATIC, "192.168.1.1",
      "255.255.0.0", "192.168.255.255" } };

  bool isInterfaceApplicableToSystem = validator_.IsInterfaceApplicableToSystem(
      ip_configs, actual_interfaces, not_assignable_interfaces);
  EXPECT_TRUE(isInterfaceApplicableToSystem);

  Status status = validator_.ValidateIPConfigIsApplicableToSystem(
      ip_configs, actual_interfaces);
  EXPECT_EQ(StatusCode::INVALID_PARAMETER, status.Get());
}

TEST_F(AIPValidator, FailedToValidateAnIPConfigWhichNetworkAlreadyExistsOnTheSystem) {

  IPConfig actual_ip_config = { "ethX1", IPSource::STATIC, "192.168.1.2",
      "255.255.255.0", "192.168.1.255" };
  EXPECT_CALL(mock_ip_controller_, GetIPConfig(Eq("ethX1"),_)).WillOnce(
      DoAll(SetArgReferee<1>(actual_ip_config), Return(Status())));

  Interfaces actual_interfaces = { "ethX1", "ethX2" };
  Interfaces not_assignable_interfaces = { };
  IPConfigs ip_configs = { { "ethX2", IPSource::STATIC, "192.168.1.1",
      "255.255.255.0", "192.168.1.255" } };

  bool isInterfaceApplicableToSystem = validator_.IsInterfaceApplicableToSystem(
      ip_configs, actual_interfaces, not_assignable_interfaces);
  EXPECT_TRUE(isInterfaceApplicableToSystem);

  Status status = validator_.ValidateIPConfigIsApplicableToSystem(
      ip_configs, actual_interfaces);
  EXPECT_EQ(StatusCode::INVALID_PARAMETER, status.Get());
}

TEST_F(AIPValidator, FailedToValidateMultipleIPConfigsWhichIPAndNetworkAlreadyExistsOnTheSystem) {

  //ip already exist ethX1 == ethX4
  IPConfig actual_ip_config_1 = { "ethX1", IPSource::STATIC, "192.168.1.2",
      "255.255.255.0", "192.168.1.255" };
  EXPECT_CALL(mock_ip_controller_, GetIPConfig(Eq("ethX1"),_)).WillOnce(
      DoAll(SetArgReferee<1>(actual_ip_config_1), Return(Status())));

  //network already exist ethX2 == ethX5
  IPConfig actual_ip_config_2 = { "ethX2", IPSource::STATIC, "192.168.2.2",
      "255.255.255.0", "192.168.2.255" };
  EXPECT_CALL(mock_ip_controller_, GetIPConfig(Eq("ethX2"),_)).WillOnce(
      DoAll(SetArgReferee<1>(actual_ip_config_2), Return(Status())));

  //ip and network already exist ethX3 == ethX6
  IPConfig actual_ip_config_3 = { "ethX3", IPSource::STATIC, "192.168.3.2",
      "255.255.255.0", "192.168.3.255" };
  EXPECT_CALL(mock_ip_controller_, GetIPConfig(Eq("ethX3"),_)).WillOnce(
      DoAll(SetArgReferee<1>(actual_ip_config_3), Return(Status())));

  Interfaces actual_interfaces = { "ethX1", "ethX2", "ethX3", "ethX4", "ethX5",
      "ethX6" };
  Interfaces not_assignable_interfaces = { };
  IPConfigs ip_configs = { { "ethX4", IPSource::STATIC, "192.168.1.2",
      "255.255.0.0", "192.168.255.255" }, { "ethX5", IPSource::STATIC,
      "192.168.2.1", "255.255.255.0", "192.168.2.255" }, { "ethX6",
      IPSource::STATIC, "192.168.3.2", "255.255.255.0", "192.168.3.255" } };

  bool isInterfaceApplicableToSystem = validator_.IsInterfaceApplicableToSystem(
      ip_configs, actual_interfaces, not_assignable_interfaces);
  EXPECT_TRUE(isInterfaceApplicableToSystem);

  Status status = validator_.ValidateIPConfigIsApplicableToSystem(
      ip_configs, actual_interfaces);
  EXPECT_EQ(StatusCode::INVALID_PARAMETER, status.Get());
}

TEST_F(AIPValidator, DoesNotValidateIPParameterOfDHCPBootpAndNoneIPSources) {

  Interfaces actual_interfaces = { "ethX1", "ethX2", "ethX3" };
  Interfaces not_assignable_interfaces = { };
  IPConfigs ip_configs = { { "ethX1", IPSource::NONE, "0.0.0.0", "0.0.0.0",
      "0.0.0.0" }, { "ethX2", IPSource::DHCP, "0.0.0.0", "0.0.0.0", "0.0.0.0" },
      { "ethX3", IPSource::BOOTP, "0.0.0.0", "0.0.0.0", "0.0.0.0" } };
  bool isInterfaceApplicableToSystem = validator_.IsInterfaceApplicableToSystem(
      ip_configs, actual_interfaces, not_assignable_interfaces);

  EXPECT_TRUE(isInterfaceApplicableToSystem);
}

} /* namespace netconfd */
