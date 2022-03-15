// SPDX-License-Identifier: GPL-2.0-or-later
//------------------------------------------------------------------------------
///  \file     IPValidatorTest.cpp
///
///  \brief    <short description of the file contents>
///
///  \author   <author> : WAGO Kontakttechnik GmbH & Co. KG
//------------------------------------------------------------------------------

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <boost/asio/ip/address_v4.hpp>
#include <boost/asio/ip/network_v4.hpp>
#include <string>

#include "IPValidator.hpp"
#include "TypesTestHelper.hpp"

using namespace std::literals;
using namespace testing;

namespace netconf {

class AnIPValidator : public Test {
 public:
  IPValidator validator_;

  AnIPValidator() : validator_{} {
  }

  void SetUp() override {
  }

  void TearDown() override {
  }
};

TEST_F(AnIPValidator, ShouldValidateAnIPConfig) {
  IPConfigs ip_configs = {{"br0", IPSource::STATIC, "192.168.1.1", "255.255.255.0"}};
  Status status        = validator_.ValidateIPConfigs(ip_configs);

  EXPECT_TRUE(status.IsOk());
}

TEST_F(AnIPValidator, ShouldValidateAnIPConfigWithoutBroadcast) {
  IPConfigs ip_configs = {{"br0", IPSource::STATIC, "192.168.1.1", "255.255.255.0"}};
  Status status        = validator_.ValidateIPConfigs(ip_configs);

  EXPECT_TRUE(status.IsOk());
}

TEST_F(AnIPValidator, ShouldValidateTwoIPConfigs) {
  IPConfigs ip_configs = {{"br0", IPSource::STATIC, "192.168.1.1", "255.255.255.0"},
                          {"br1", IPSource::STATIC, "192.168.2.1", "255.255.255.0"}};
  Status status        = validator_.ValidateIPConfigs(ip_configs);

  EXPECT_TRUE(status.IsOk());
}

TEST_F(AnIPValidator, ShouldRejectZeroIpForStaticSource) {
  IPConfigs ip_configs = {{"br0", IPSource::STATIC, ZeroIP, ZeroIP},
                          {"br1", IPSource::STATIC, "192.168.2.1", "255.255.255.0"},
                          {"br2", IPSource::STATIC, ZeroIP, ZeroIP}};
  Status status        = validator_.ValidateIPConfigs(ip_configs);

  EXPECT_FALSE(status.IsOk());
}

TEST_F(AnIPValidator, ShouldAcceptNoneToResetIpParameters) {
  IPConfigs ip_configs = {{"br0", IPSource::NONE, ZeroIP, ZeroIP},
                          {"br1", IPSource::STATIC, "192.168.2.1", "255.255.255.0"},
                          {"br2", IPSource::NONE, ZeroIP, ZeroIP}};
  Status status        = validator_.ValidateIPConfigs(ip_configs);

  EXPECT_TRUE(status.IsOk());
}

TEST_F(AnIPValidator, FailedToValidateAnStaticIPConfigWhichContainsAnInterfaceThatNotExist) {
  IPConfigs ip_configs = {{"ethX1", IPSource::STATIC, "192.168.1.1", "255.255.255.0"}};
  Status status        = validator_.ValidateIPConfigs(ip_configs);

  EXPECT_TRUE(status.IsOk());
}

TEST_F(AnIPValidator, FailedToValidatesADHCPIPConfigWhichContainsAnInterfaceThatNotExist) {
  IPConfigs ip_configs = {{"ethX1", IPSource::DHCP, "192.168.1.1", "255.255.255.0"}};
  Status status        = validator_.ValidateIPConfigs(ip_configs);

  EXPECT_TRUE(status.IsOk());
}

TEST_F(AnIPValidator, FailedToValidatesABootpIPConfigWhichContainsAnInterfaceThatNotExist) {
  IPConfigs ip_configs = {{"ethX1", IPSource::BOOTP, "192.168.1.1", "255.255.255.0"}};
  Status status        = validator_.ValidateIPConfigs(ip_configs);

  EXPECT_TRUE(status.IsOk());
}

TEST_F(AnIPValidator, FailedToValidateAStaticIPConfigWhichContainsAnInterfaceThatIsNotAssignable) {
  IPConfigs ip_configs = {{"ethX1", IPSource::STATIC, "192.168.1.1", "255.255.255.0"}};
  Status status        = validator_.ValidateIPConfigs(ip_configs);

  EXPECT_TRUE(status.IsOk());
}

TEST_F(AnIPValidator, FailedToValidateADHCPIPConfigWhichContainsAnInterfaceThatIsNotAssignable) {
  IPConfigs ip_configs = {{"ethX1", IPSource::DHCP, "192.168.1.1", "255.255.255.0"}};
  Status status        = validator_.ValidateIPConfigs(ip_configs);

  EXPECT_TRUE(status.IsOk());
}

TEST_F(AnIPValidator, FailedToValidateABootpIPConfigWhichContainsAnInterfaceThatIsNotAssignable) {
  IPConfigs ip_configs = {{"ethX1", IPSource::BOOTP, "192.168.1.1", "255.255.255.0"}};
  Status status        = validator_.ValidateIPConfigs(ip_configs);

  EXPECT_TRUE(status.IsOk());
}

TEST_F(AnIPValidator, FailedToValidateAnIPConfigWhichContainsOneInterfaceSeveralTimes) {
  IPConfigs ip_configs = {{"ethX1", IPSource::STATIC, "192.168.2.1", "255.255.255.0"},
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

TEST_F(AnIPValidator, DisregardsConfigsOfSameInterfaceInOverlappingCheck) {
  IPConfigs current_ip_configs = {{"br0", IPSource::STATIC, "192.168.1.128", "255.255.255.128"},
                                  {"br1", IPSource::STATIC, "192.168.2.128", "255.255.255.128"}};

  IPConfigs new_ip_configs_totaloverlap = {{"br0", IPSource::STATIC, "192.168.1.128", "255.255.255.128"},
                                           {"br1", IPSource::STATIC, "192.168.2.128", "255.255.255.128"}};

  IPConfigs new_ip_configs_partialoverlap = {{"br0", IPSource::STATIC, "192.168.1.128", "255.255.255.128"},
                                             {"br2", IPSource::STATIC, "192.168.3.128", "255.255.255.128"}};

  Status status = validator_.ValidateCombinabilityOfIPConfigs(new_ip_configs_totaloverlap, current_ip_configs);
  EXPECT_EQ(StatusCode::OK, status.GetStatusCode());
  status = validator_.ValidateCombinabilityOfIPConfigs(new_ip_configs_partialoverlap, current_ip_configs);
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
  IPConfigs ip_configs = {{"ethX1", IPSource::STATIC, "192.168.1.1", "255.255.255.0"},
                          {
                              "ethX2",
                              IPSource::STATIC,
                              "192.168.2.1",
                              "255.255.0.0",
                          },
                          {"ethX3", IPSource::STATIC, "192.168.1.1", "255.0.0.0"}};

  Status status = validator_.ValidateIPConfigs(ip_configs);
  EXPECT_EQ(StatusCode::IP_DISTRIBUTED_MULTIPLE_TIMES, status.GetStatusCode());
}

TEST_F(AnIPValidator, FailedToValidateIPConfigsWithSeveralNetworks) {
  IPConfigs ip_configs = {{"ethX1", IPSource::STATIC, "192.168.1.1", "255.255.255.0"},
                          {"ethX2", IPSource::STATIC, "192.168.1.2", "255.255.255.0"},
                          {"ethX3", IPSource::STATIC, "192.168.2.1", "255.255.255.0"}};

  Status status = validator_.ValidateIPConfigs(ip_configs);
  EXPECT_EQ(StatusCode::NETWORK_CONFLICT, status.GetStatusCode());
}

TEST_F(AnIPValidator, FailedToValidateOverlappingNetmask) {
  IPConfigs new_ip_configs    = {{"ethX2", IPSource::STATIC, "192.168.1.1", "255.255.255.0"}};
  IPConfigs remain_ip_configs = {{"ethX1", IPSource::DHCP, "192.168.1.2", "255.255.0.0"}};

  Status status = validator_.ValidateCombinabilityOfIPConfigs(new_ip_configs, remain_ip_configs);
  EXPECT_EQ(StatusCode::NETWORK_CONFLICT, status.GetStatusCode());
}

TEST_F(AnIPValidator, FailedToValidateAnIPConfigWhichIPIsAlreadyAssignedOnTheSystem) {
  IPConfigs remain_ip_configs = {{"ethX1", IPSource::STATIC, "192.168.1.1", "255.255.0.0"}};
  IPConfigs new_ip_configs    = {{"ethX2", IPSource::STATIC, "192.168.1.1", "255.255.255.0"}};

  Status status = validator_.ValidateCombinabilityOfIPConfigs(new_ip_configs, remain_ip_configs);
  EXPECT_EQ(StatusCode::NETWORK_CONFLICT, status.GetStatusCode());
}

TEST_F(AnIPValidator, FailedToValidateAnIPConfigWhichNetworkAlreadyExistsOnTheSystem) {
  IPConfigs remain_ip_configs = {{"ethX1", IPSource::STATIC, "192.168.1.2", "255.255.0.0"}};
  IPConfigs new_ip_configs    = {{"ethX2", IPSource::STATIC, "192.168.1.1", "255.255.0.0"}};

  Status status = validator_.ValidateCombinabilityOfIPConfigs(new_ip_configs, remain_ip_configs);
  EXPECT_EQ(StatusCode::NETWORK_CONFLICT, status.GetStatusCode());
}

TEST_F(AnIPValidator, FailedToValidateMultipleIPConfigsWhichIPAndNetworkAlreadyExistsOnTheSystem) {
  IPConfig actual_ip_config_1 = {"ethX1", IPSource::STATIC, "192.168.1.2", "255.255.255.0"};
  IPConfig actual_ip_config_2 = {"ethX2", IPSource::STATIC, "192.168.2.2", "255.255.255.0"};
  IPConfig actual_ip_config_3 = {"ethX3", IPSource::STATIC, "192.168.3.2", "255.255.255.0"};
  IPConfigs actual_ip_configs{actual_ip_config_1, actual_ip_config_2, actual_ip_config_3};

  IPConfigs ip_configs = {{"ethX4", IPSource::STATIC, "192.168.1.2", "255.255.0.0"},
                          {"ethX5", IPSource::STATIC, "192.168.2.1", "255.255.255.0"},
                          {"ethX6", IPSource::STATIC, "192.168.3.2", "255.255.255.0"}};

  Status status = validator_.ValidateCombinabilityOfIPConfigs(ip_configs, actual_ip_configs);
  EXPECT_EQ(StatusCode::NETWORK_CONFLICT, status.GetStatusCode());
}

TEST_F(AnIPValidator, ShouldAcceptRFC3021Ips) {
  // RFC3021 defines that subnet mask 255.255.255.254 contains exactly 2 hosts without broadcast address.
  // This is used in Point-to-Point IP links like modems.
  // Further 255.255.255.255 is used to define exactly one IP address.
  auto netmask_30 = IPConfigs{{"wwan0", IPSource::STATIC, "10.12.88.128", "255.255.255.254"},
                              {"wwan1", IPSource::STATIC, "10.12.88.133", "255.255.255.254"},
                              {"wwan2", IPSource::STATIC, "10.12.88.130", "255.255.255.255"}};

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
  IPConfig ip_config_1   = {"ethX1", IPSource::STATIC, "192.168.10.10", "255.255.255.0"};
  IPConfigs ip_configs_1 = {ip_config_1};
  IPConfig ip_config_2   = {"ethX2", IPSource::STATIC, "192.168.20.10", "255.255.255.0"};
  IPConfigs ip_configs_2 = {ip_config_2};
  IPConfig ip_config_3   = {"ethX3", IPSource::STATIC, "192.168.30.10", "255.255.255.0"};
  IPConfigs ip_configs_3 = {ip_config_3};

  Status status = validator_.ValidateCombinabilityOfIPConfigs(ip_configs_2, ip_configs_1);
  EXPECT_EQ(StatusCode::OK, status.GetStatusCode());

  IPConfigs ip_configs_12 = {ip_config_1, ip_config_2};

  status = validator_.ValidateCombinabilityOfIPConfigs(ip_configs_3, ip_configs_12);
  EXPECT_EQ(StatusCode::OK, status.GetStatusCode());
}

using ValidateParams = std::tuple<IPConfigs, IPConfigs, InterfaceInformations, StatusCode>;
struct AnIPValidatorDigests : public AnIPValidator, public WithParamInterface<ValidateParams> {};

inline ValidateParams of(IPConfigs candidate, IPConfigs current, InterfaceInformations ii, StatusCode status) {
  return std::make_tuple(std::move(candidate), std::move(current), std::move(ii), std::move(status));
}

inline InterfaceInformation accessibleBridge(std::string interfacename) {
  return InterfaceInformation{interfacename, interfacename, DeviceType::Bridge};
}
inline InterfaceInformation inaccessibleBridge(std::string interfacename) {
  return InterfaceInformation{interfacename, interfacename, DeviceType::Bridge, true};
}

TEST_P(AnIPValidatorDigests, throughItsValidateMethod) {
  auto [candidate, current, interface_information, expected_result] = GetParam();

  auto result = IPValidator::Validate(candidate, current, interface_information);
  EXPECT_EQ(expected_result, result.GetStatusCode());
}

INSTANTIATE_TEST_CASE_P(
    Values, AnIPValidatorDigests,
    ::testing::Values(
        // #0 No configs can not conflict
        of({}, {}, {}, StatusCode::OK),

        // #1 Can not configure a non-existing bridge
        of({{"br0", IPSource::STATIC, "192.168.1.1", "255.255.255.0"}}, {}, {}, StatusCode::INTERFACE_NOT_EXISTING),

        // #2 Can not configure a read-only bridge
        of({{"br0", IPSource::STATIC, "192.168.1.1", "255.255.255.0"}}, {}, {inaccessibleBridge("br0")},
           StatusCode::IP_CONFIG_READONLY),

        // #3
        of({{"br0", IPSource::STATIC, "192.168.1.1", "255.255.255.0"}},
           {{"br0", IPSource::STATIC, "192.168.1.2", "255.255.255.0"}}, {accessibleBridge("br0")}, StatusCode::OK),

        /* #4 Conflicting networks with current IPs */
        of({{"br0", IPSource::STATIC, "192.168.1.1", "255.255.255.0"}},
           {{"br1", IPSource::STATIC, "192.168.1.2", "255.255.255.0"}},
           {accessibleBridge("br0"), accessibleBridge("br1")}, StatusCode::NETWORK_CONFLICT),

        // #5
        of({{"br0", IPSource::STATIC, "192.168.1.1", "255.255.255.0"}},
           {{"br1", IPSource::DHCP, "192.168.1.2", "255.255.255.0"}},
           {accessibleBridge("br0"), accessibleBridge("br1")}, StatusCode::NETWORK_CONFLICT),

        /* #6 DHCP in candidate is not considered in conflicts */
        of({{"br0", IPSource::DHCP, "192.168.1.1", "255.255.255.0"}},
           {{"br1", IPSource::STATIC, "192.168.1.2", "255.255.255.0"}},
           {accessibleBridge("br0"), accessibleBridge("br1")}, StatusCode::OK),

        /* #7 General syntax error */
        of({{"br0", IPSource::STATIC, "192.168.asdasd", "255.255.255.0"}},
           {{"br1", IPSource::STATIC, "192.168.1.2", "255.255.255.0"}}, {accessibleBridge("br0")},
           StatusCode::IP_INVALID),

        // #8
        of({{"br0", IPSource::STATIC, "192.168.1.1", "255bla"}},
           {{"br1", IPSource::STATIC, "192.168.1.2", "255.255.255.0"}}, {accessibleBridge("br0")},
           StatusCode::NETMASK_INVALID),

        /* #9 Conflict in candidate */
        of({{"br0", IPSource::STATIC, "192.168.1.1", "255.255.255.0"},
            {"br1", IPSource::STATIC, "192.168.1.10", "255.255.255.0"}},
           {{"br1", IPSource::STATIC, "192.168.2.2", "255.255.255.0"}},
           {accessibleBridge("br0"), accessibleBridge("br1")}, StatusCode::NETWORK_CONFLICT),

        /* #10 Switch current IPs should work */
        of({{"br0", IPSource::STATIC, "192.168.1.1", "255.255.255.0"},
            {"br1", IPSource::STATIC, "192.168.2.1", "255.255.255.0"}},
           {{"br0", IPSource::STATIC, "192.168.2.1", "255.255.255.0"},
            {"br1", IPSource::STATIC, "192.168.1.1", "255.255.255.0"}},
           {accessibleBridge("br0"), accessibleBridge("br1")}, StatusCode::OK),

        /* #11 override multiple IPs is OK*/
        of({{"br0", IPSource::STATIC, "192.168.1.2", "255.255.255.0"},
            {"br1", IPSource::STATIC, "192.169.1.2", "255.255.255.0"},
            {"br2", IPSource::STATIC, "192.170.1.2", "255.255.255.0"},
            {"br3", IPSource::STATIC, "192.171.1.2", "255.255.255.0"}},
           {{"br0", IPSource::STATIC, "192.168.1.1", "255.255.255.0"},
            {"br1", IPSource::STATIC, "192.168.2.1", "255.255.255.0"}},
           {accessibleBridge("br0"), accessibleBridge("br1"), accessibleBridge("br2"), accessibleBridge("br3")},
           StatusCode::OK),

        /* #12 complements netmasks when checking*/
        of({{"br0", IPSource::STATIC, "192.168.1.2", ""}, {"br1", IPSource::STATIC, "192.168.2.2", "0.0.0.0"}},
           {{"br0", IPSource::STATIC, "192.168.1.1", "255.255.255.0"},
            {"br1", IPSource::STATIC, "192.168.2.1", "255.255.255.0"}},
           {accessibleBridge("br0"), accessibleBridge("br1")}, StatusCode::OK),

        /* #13 empty/zero current are ignored */
        of({{"br1", IPSource::STATIC, "192.168.2.2", ""}},
           {{"br0", IPSource::STATIC, "192.168.1.1", "255.255.255.0"}, {"ethX1", IPSource::NONE, "0.0.0.0", "0.0.0.0"}},
           {accessibleBridge("br0"), accessibleBridge("br1")}, StatusCode::OK),

        /* #14 IP is moved to another bridge */
        of({{"br1", IPSource::STATIC, "192.168.1.1", "255.255.255.0"},
            {"br0", IPSource::STATIC, "192.168.0.1", "255.255.255.0"}},
           {{"br0", IPSource::STATIC, "192.168.1.1", "255.255.255.0"}, {"br1", IPSource::NONE, "0.0.0.0", "0.0.0.0"}},
           {accessibleBridge("br0"), accessibleBridge("br1")}, StatusCode::OK)

            ));

/*
 * {"br0":{"bcast":"123.1.1.255","ipaddr":"123.1.1.1","netmask":"255.255.255.0","source":"static"},"br1":{"bcast":"0.0.0.0","ipaddr":"0.0.0.0","netmask":"0.0.0.0","source":"none"},"ethX1":{"bcast":"0.0.0.0","ipaddr":"0.0.0.0","netmask":"0.0.0.0","source":"none"},"ethX11":{"bcast":"0.0.0.0","ipaddr":"0.0.0.0","netmask":"0.0.0.0","source":"none"},"ethX12":{"bcast":"0.0.0.0","ipaddr":"0.0.0.0","netmask":"0.0.0.0","source":"none"},"ethX2":{"bcast":"0.0.0.0","ipaddr":"0.0.0.0","netmask":"0.0.0.0","source":"none"}}
 */

}  // namespace netconf
