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

#include "TypesTestHelper.hpp"

using namespace std::literals;
using namespace testing;

namespace netconf {

  class AnIPValidator : public Test {
   public:
    IPValidator validator_;

    AnIPValidator()
        :
        validator_ { } {

    }

    void SetUp() override
    {
    }

    void TearDown() override
    {

    }

  };

  class AnIPValidatorP: public AnIPValidator, public testing::WithParamInterface<uint16_t>
  {
  };

  TEST_F(AnIPValidator, ShouldValidateAnIPConfig) {

    IPConfigs ip_configs = {{"br0", IPSource::STATIC, "192.168.1.1", "255.255.255.0"}};
    Status status = validator_.ValidateIPConfigs(ip_configs);

    EXPECT_TRUE(status.IsOk());
  }

  TEST_F(AnIPValidator, ShouldValidateAnIPConfigWithoutBroadcast) {

    IPConfigs ip_configs = {{"br0", IPSource::STATIC, "192.168.1.1", "255.255.255.0"}};
    Status status = validator_.ValidateIPConfigs(ip_configs);

    EXPECT_TRUE(status.IsOk());
  }

  TEST_F(AnIPValidator, ShouldValidateTwoIPConfigs) {

    IPConfigs ip_configs = {
        {"br0", IPSource::STATIC, "192.168.1.1", "255.255.255.0"},
        {"br1", IPSource::STATIC, "192.168.2.1", "255.255.255.0"}};
    Status status = validator_.ValidateIPConfigs(ip_configs);

    EXPECT_TRUE(status.IsOk());
  }

  TEST_F(AnIPValidator, ShouldRejectZeroIpForStaticSource) {

    IPConfigs ip_configs = {
        {"br0", IPSource::STATIC, ZeroIP, ZeroIP},
        {"br1", IPSource::STATIC, "192.168.2.1", "255.255.255.0"},
        {"br2", IPSource::STATIC, ZeroIP, ZeroIP}};
    Status status = validator_.ValidateIPConfigs(ip_configs);

    EXPECT_FALSE(status.IsOk());
  }

  TEST_F(AnIPValidator, ShouldAcceptNoneToResetIpParameters) {

    IPConfigs ip_configs = {
        {"br0", IPSource::NONE, ZeroIP, ZeroIP},
        {"br1", IPSource::STATIC, "192.168.2.1", "255.255.255.0"},
        {"br2", IPSource::NONE, ZeroIP, ZeroIP}};
    Status status = validator_.ValidateIPConfigs(ip_configs);

    EXPECT_TRUE(status.IsOk());
  }

  TEST_F(AnIPValidator, FailedToValidateAnStaticIPConfigWhichContainsAnInterfaceThatNotExist) {

    IPConfigs ip_configs = {{"ethX1", IPSource::STATIC, "192.168.1.1", "255.255.255.0"}};
    Status status = validator_.ValidateIPConfigs(ip_configs);

    EXPECT_TRUE(status.IsOk());
  }

  TEST_F(AnIPValidator, FailedToValidatesADHCPIPConfigWhichContainsAnInterfaceThatNotExist) {

    IPConfigs ip_configs = {{"ethX1", IPSource::DHCP, "192.168.1.1", "255.255.255.0"}};
    Status status = validator_.ValidateIPConfigs(ip_configs);

    EXPECT_TRUE(status.IsOk());
  }

  TEST_F(AnIPValidator, FailedToValidatesABootpIPConfigWhichContainsAnInterfaceThatNotExist) {

    IPConfigs ip_configs = {{"ethX1", IPSource::BOOTP, "192.168.1.1", "255.255.255.0"}};
    Status status = validator_.ValidateIPConfigs(ip_configs);

    EXPECT_TRUE(status.IsOk());
  }

  TEST_F(AnIPValidator, FailedToValidateAStaticIPConfigWhichContainsAnInterfaceThatIsNotAssignable) {

    IPConfigs ip_configs = {{"ethX1", IPSource::STATIC, "192.168.1.1", "255.255.255.0"}};
    Status status = validator_.ValidateIPConfigs(ip_configs);

    EXPECT_TRUE(status.IsOk());
  }

  TEST_F(AnIPValidator, FailedToValidateADHCPIPConfigWhichContainsAnInterfaceThatIsNotAssignable) {

    IPConfigs ip_configs = {{"ethX1", IPSource::DHCP, "192.168.1.1", "255.255.255.0"}};
    Status status = validator_.ValidateIPConfigs(ip_configs);

    EXPECT_TRUE(status.IsOk());
  }

  TEST_F(AnIPValidator, FailedToValidateABootpIPConfigWhichContainsAnInterfaceThatIsNotAssignable) {

    IPConfigs ip_configs = {{"ethX1", IPSource::BOOTP, "192.168.1.1", "255.255.255.0"}};
    Status status = validator_.ValidateIPConfigs(ip_configs);

    EXPECT_TRUE(status.IsOk());
  }

  TEST_F(AnIPValidator, FailedToValidateAnIPConfigWhichContainsOneInterfaceSeveralTimes) {

    IPConfigs ip_configs = {
        {"ethX1", IPSource::STATIC, "192.168.2.1", "255.255.255.0"},
        {"ethX1", IPSource::STATIC, "192.168.1.1", "255.255.255.0"}};

    Status status = validator_.ValidateIPConfigs(ip_configs);

    EXPECT_EQ(StatusCode::ENTRY_DUPLICATE, status.GetStatusCode()) << status.ToString();
  }

  TEST_F(AnIPValidator, AssigningNetworkAddressAsHostIsAccepted) {

    // This is an exception for the wwan0 interface, the modem assigns such addresses :-(

    IPConfigs ip_configs = {{"br0", IPSource::STATIC, "192.168.1.128", "255.255.255.128"}};

    Status status = validator_.ValidateIPConfigs(ip_configs);
    EXPECT_EQ(StatusCode::OK, status.GetStatusCode());
  }

  TEST_F(AnIPValidator, FailedToValidateAnIPConfigWhichContainsAnInvalidAddress) {

    IPConfigs ip_configs = {{"ethX1", IPSource::STATIC, "192.168.1.1111", "255.255.255.0"}};

    Status status = validator_.ValidateIPConfigs(ip_configs);
    EXPECT_EQ(StatusCode::IP_INVALID, status.GetStatusCode()) << status.ToString();
  }

  TEST_F(AnIPValidator, FailedToValidateAnIPConfigWhichAddressContainsOnlyOnes) {

    IPConfigs ip_configs = {{"ethX1", IPSource::STATIC, "255.255.255.255", "255.255.255.0"}};

    Status status = validator_.ValidateIPConfigs(ip_configs);
    EXPECT_EQ(StatusCode::IP_INVALID, status.GetStatusCode());
  }

  TEST_F(AnIPValidator, FailedToValidateAnIPConfigWhichAddressContainsZeros) {

    IPConfigs ip_configs = {{"ethX1", IPSource::STATIC, ZeroIP, "255.255.255.0"}};

    Status status = validator_.ValidateIPConfigs(ip_configs);
    EXPECT_EQ(StatusCode::IP_INVALID, status.GetStatusCode());
  }

  TEST_F(AnIPValidator, FailedToValidateAnIPConfigWhichContainsAnInvalidNetmask) {

    IPConfigs ip_configs = {{"ethX1", IPSource::STATIC, "192.168.1.1", "255.X.255.0"}};

    Status status = validator_.ValidateIPConfigs(ip_configs);
    EXPECT_EQ(StatusCode::NETMASK_INVALID, status.GetStatusCode());
  }

  TEST_F(AnIPValidator, FailedToValidateAnIPConfigWhichContainsZeroNetmask) {

    IPConfigs ip_configs = {{"ethX1", IPSource::STATIC, "192.168.1.1", ZeroIP}};

    Status status = validator_.ValidateIPConfigs(ip_configs);
    EXPECT_EQ(StatusCode::NETMASK_INVALID, status.GetStatusCode());
  }

  TEST_F(AnIPValidator, FailedToValidateAnIPConfigWhichSubnetmaskContainsZeros) {

    IPConfigs ip_configs = {{"ethX1", IPSource::STATIC, "192.168.1.1", "255.111.255.0"}};

    Status status = validator_.ValidateIPConfigs(ip_configs);
    EXPECT_EQ(StatusCode::NETMASK_INVALID, status.GetStatusCode());
  }


  TEST_F(AnIPValidator, FailedToValidateIPConfigsWithSeveralIpAddresses) {

    IPConfigs ip_configs = {
        {"ethX1", IPSource::STATIC, "192.168.1.1", "255.255.255.0"},
        {"ethX2", IPSource::STATIC, "192.168.2.1", "255.255.0.0", },
        {"ethX3", IPSource::STATIC, "192.168.1.1", "255.0.0.0"}};

    Status status = validator_.ValidateIPConfigs(ip_configs);
    EXPECT_EQ(StatusCode::IP_DISTRIBUTED_MULTIPLE_TIMES, status.GetStatusCode());
  }

  TEST_F(AnIPValidator, FailedToValidateIPConfigsWithSeveralNetworks) {

    IPConfigs ip_configs = {
        {"ethX1", IPSource::STATIC, "192.168.1.1", "255.255.255.0"},
        {"ethX2", IPSource::STATIC, "192.168.1.2", "255.255.255.0"},
        {"ethX3", IPSource::STATIC, "192.168.2.1", "255.255.255.0"}};

    Status status = validator_.ValidateIPConfigs(ip_configs);
    EXPECT_EQ(StatusCode::NETWORK_CONFLICT, status.GetStatusCode());
  }

  TEST_F(AnIPValidator, FailedToValidateOverlappingNetmask) {

    IPConfigs new_ip_configs = {{"ethX2", IPSource::STATIC, "192.168.1.1", "255.255.255.0"}};
    IPConfigs remain_ip_configs = {{"ethX1", IPSource::DHCP, "192.168.1.2", "255.255.0.0"}};

    Status status = validator_.ValidateCombinabilityOfIPConfigs(new_ip_configs, remain_ip_configs);
    EXPECT_EQ(StatusCode::NETWORK_CONFLICT, status.GetStatusCode());
  }

  TEST_F(AnIPValidator, FailedToValidateAnIPConfigWhichIPIsAlreadyAssignedOnTheSystem) {

    IPConfigs remain_ip_configs = {{"ethX1", IPSource::STATIC, "192.168.1.1", "255.255.0.0"}};
    IPConfigs new_ip_configs = {{"ethX2", IPSource::STATIC, "192.168.1.1", "255.255.255.0"}};

    Status status = validator_.ValidateCombinabilityOfIPConfigs(new_ip_configs, remain_ip_configs);
    EXPECT_EQ(StatusCode::NETWORK_CONFLICT, status.GetStatusCode());
  }

  TEST_F(AnIPValidator, FailedToValidateAnIPConfigWhichNetworkAlreadyExistsOnTheSystem) {

    IPConfigs remain_ip_configs = {{"ethX1", IPSource::STATIC, "192.168.1.2", "255.255.0.0"}};
    IPConfigs new_ip_configs = {{"ethX2", IPSource::STATIC, "192.168.1.1", "255.255.0.0"}};

    Status status = validator_.ValidateCombinabilityOfIPConfigs(new_ip_configs, remain_ip_configs);
    EXPECT_EQ(StatusCode::NETWORK_CONFLICT, status.GetStatusCode());
  }

  TEST_F(AnIPValidator, FailedToValidateMultipleIPConfigsWhichIPAndNetworkAlreadyExistsOnTheSystem) {

    IPConfig actual_ip_config_1 = {"ethX1", IPSource::STATIC, "192.168.1.2", "255.255.255.0"};
    IPConfig actual_ip_config_2 = {"ethX2", IPSource::STATIC, "192.168.2.2", "255.255.255.0"};
    IPConfig actual_ip_config_3 = {"ethX3", IPSource::STATIC, "192.168.3.2", "255.255.255.0"};
    IPConfigs actual_ip_configs{actual_ip_config_1, actual_ip_config_2, actual_ip_config_3};

    IPConfigs ip_configs = {
        {"ethX4", IPSource::STATIC, "192.168.1.2", "255.255.0.0"},
        {"ethX5", IPSource::STATIC, "192.168.2.1", "255.255.255.0"},
        {"ethX6", IPSource::STATIC, "192.168.3.2", "255.255.255.0"}};

    Status status = validator_.ValidateCombinabilityOfIPConfigs(ip_configs, actual_ip_configs);
    EXPECT_EQ(StatusCode::NETWORK_CONFLICT, status.GetStatusCode());
  }

  TEST_F(AnIPValidator, ShouldAcceptRFC3021Ips) {
    // RFC3021 defines that subnet mask 255.255.255.254 contains exactly 2 hosts without broadcast address.
    // This is used in Point-to-Point IP links like modems.
    // Further 255.255.255.255 is used to define exactly one IP address.
    auto netmask_30 = IPConfigs{
      {"wwan0", IPSource::TEMPORARY, "10.12.88.128", "255.255.255.254"},
      {"wwan1", IPSource::TEMPORARY, "10.12.88.133", "255.255.255.254"},
      {"wwan2", IPSource::TEMPORARY, "10.12.88.130", "255.255.255.255"}};

    auto status = validator_.ValidateIPConfigs(netmask_30);

    EXPECT_EQ(StatusCode::OK, status.GetStatusCode()) << status.ToString();
  }

  TEST_F(AnIPValidator, validateOverlappingNetworksAsInvalidInCaseOfStaticIP) {

    IPConfig ip_config_1 = {"ethX1", IPSource::STATIC, "192.168.1.2", "255.255.255.0"};
    IPConfig ip_config_2 = {"ethX2", IPSource::STATIC, "192.168.2.2", "255.255.0.0"};
    IPConfigs ip_configs{ip_config_1, ip_config_2};

    auto status = validator_.ValidateIPConfigs(ip_configs);
    EXPECT_EQ(StatusCode::NETWORK_CONFLICT, status.GetStatusCode()) << status.ToString();
  }

  TEST_F(AnIPValidator, shouldValidateMultipleIPConfigs) {

    IPConfig ip_config_1 = {"ethX1", IPSource::STATIC, "192.168.10.10", "255.255.255.0"};
    IPConfigs ip_configs_1 = {ip_config_1};
    IPConfig ip_config_2 = {"ethX2", IPSource::STATIC, "192.168.20.10", "255.255.255.0"};
    IPConfigs ip_configs_2 = {ip_config_2};
    IPConfig ip_config_3 = {"ethX3", IPSource::STATIC, "192.168.30.10", "255.255.255.0"};
    IPConfigs ip_configs_3 = {ip_config_3};

    Status status = validator_.ValidateCombinabilityOfIPConfigs(ip_configs_2, ip_configs_1);
    EXPECT_EQ(StatusCode::OK, status.GetStatusCode());

    IPConfigs ip_configs_12 = {ip_config_1, ip_config_2};

    status = validator_.ValidateCombinabilityOfIPConfigs(ip_configs_3, ip_configs_12);
    EXPECT_EQ(StatusCode::OK, status.GetStatusCode());
  }
}
