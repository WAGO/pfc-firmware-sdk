// SPDX-License-Identifier: GPL-2.0-or-later

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "LeaseFileParser.hpp"

using namespace testing;

namespace netconf {

class DynamicIPLeaseFileParserTest : public Test {
 public:
  void SetUp() override {
  }

  void TearDown() override {
  }
};

TEST_F(DynamicIPLeaseFileParserTest, parseContent) {
  ::std::string content =
      R"(IPADDRESS=192.168.3.3
NETMASK=255.255.255.0
DEFAULT_GATEWAY_1=192.168.3.10
DEFAULT_GATEWAY_2=192.168.3.11
DHCPHOSTNAME=PFC200V3-425DE4
DHCPDOMAIN=local.xxx
DNS_SERVER_1=192.168.3.248
DNS_SERVER_2=192.168.3.249
NTP_SERVER_1=192.168.3.248
NTP_SERVER_2=192.168.3.249
)";

  LeaseFileParser parser;
  parser.Parse(content);

  EXPECT_EQ(parser.GetAddress(), "192.168.3.3");
  EXPECT_EQ(parser.GetNetmask(), "255.255.255.0");
  EXPECT_EQ(parser.GetDHCPHostname(), "PFC200V3-425DE4");
  EXPECT_EQ(parser.GetDHCPDomain(), "local.xxx");
}

TEST_F(DynamicIPLeaseFileParserTest, parseContentWithEmptyNetmask) {
  ::std::string content = R"(IPADDRESS=192.168.3.3
NETMASK=
DHCPHOSTNAME=HORST
DHCPDOMAIN=local.xxx
)";

  LeaseFileParser parser;
  parser.Parse(content);

  EXPECT_EQ(parser.GetAddress(), "192.168.3.3");
  EXPECT_EQ(parser.GetNetmask(), "");
  EXPECT_EQ(parser.GetDHCPHostname(), "HORST");
  EXPECT_EQ(parser.GetDHCPDomain(), "local.xxx");
}

TEST_F(DynamicIPLeaseFileParserTest, parseContentWithMissingValues) {
  ::std::string content =
      R"(DNS_SERVER_1=192.168.3.248
DNS_SERVER_2=192.168.3.249
NTP_SERVER_1=192.168.3.248
NTP_SERVER_2=192.168.3.249
)";

  LeaseFileParser parser;
  parser.Parse(content);

  EXPECT_EQ(parser.GetAddress(), "");
  EXPECT_EQ(parser.GetNetmask(), "");
  EXPECT_EQ(parser.GetDHCPHostname(), "");
  EXPECT_EQ(parser.GetDHCPDomain(), "");
}

TEST_F(DynamicIPLeaseFileParserTest, resetValuesBeforeParsing) {
  LeaseFileParser parser;

  ::std::string content = R"(IPADDRESS=192.168.3.3
NETMASK=255.255.255.0
DHCPHOSTNAME=HORST
DHCPDOMAIN=local.xxx
)";
  parser.Parse(content);

  EXPECT_EQ(parser.GetAddress(), "192.168.3.3");
  EXPECT_EQ(parser.GetNetmask(), "255.255.255.0");
  EXPECT_EQ(parser.GetDHCPHostname(), "HORST");
  EXPECT_EQ(parser.GetDHCPDomain(), "local.xxx");

  ::std::string content_missing_values = R"()";
  parser.Parse(content_missing_values);

  EXPECT_EQ(parser.GetAddress(), "");
  EXPECT_EQ(parser.GetNetmask(), "");
  EXPECT_EQ(parser.GetDHCPHostname(), "");
  EXPECT_EQ(parser.GetDHCPDomain(), "");
}

}  // namespace netconf
