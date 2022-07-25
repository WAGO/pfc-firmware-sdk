//------------------------------------------------------------------------------
// Copyright (c) WAGO GmbH & Co. KG
//
// PROPRIETARY RIGHTS are involved in the subject matter of this material. All
// manufacturing, reproduction, use and sales rights pertaining to this
// subject matter are governed by the license agreement. The recipient of this
// software implicitly accepts the terms of the license.
//------------------------------------------------------------------------------

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "ConfigConversion.hpp"

namespace wago::snmp_config_lib {

using ::testing::Contains;

TEST(ConfigConversionTest, ConvertSnmpConfigToString ) {
  SnmpConfig snmp_config;
  snmp_config.sys_name_ = "textName";
  snmp_config.sys_contact_ = "textContact";
  snmp_config.sys_descr_ = "textDescr";
  snmp_config.sys_location_ = "textLocation";
  snmp_config.sys_objectID_ = "textObjectID";
  snmp_config.sys_service_ = "textService";
  snmp_config.communities_.emplace_back(Community { "testCommunityRW", Access::ReadWrite });
  snmp_config.communities_.emplace_back(Community { "testCommunityRO", Access::ReadOnly });
  snmp_config.trap_receivers_V1V2c_.emplace_back(
      TrapReceiverV1V2c { "testHostV1", "testCommunityV1", VersionV1V2c::V1 });
  snmp_config.trap_receivers_V1V2c_.emplace_back(TrapReceiverV1V2c { "testHostV2c", "testCommunityV2c",
      VersionV1V2c::V2c });

  ::std::string str_config = SnmpBasicAndV1V2cParameterToString(snmp_config);

  ::std::string expected_snmp_config =
      R"(sysServices textService
sysName textName
sysDescr textDescr
sysLocation textLocation
sysContact textContact
sysObjectID textObjectID
rwcommunity testCommunityRW
rocommunity testCommunityRO
trapsink testHostV1 testCommunityV1
trap2sink testHostV2c testCommunityV2c
)";

  EXPECT_EQ(expected_snmp_config, str_config);
}

TEST(ConfigConversionTest, ConvertEmptySnmpConfigToString ) {
  SnmpConfig snmp_config;

  ::std::string str_config = SnmpBasicAndV1V2cParameterToString(snmp_config);

  ::std::string expected_snmp_config = R"(sysServices 
sysName 
sysDescr 
sysLocation 
sysContact 
sysObjectID 
)";

  EXPECT_EQ(expected_snmp_config, str_config);
}

TEST(ConfigConversionTest, ConvertDifferentTrapV3ConfigurationsToString ) {
  SnmpConfig snmp_config;
  snmp_config.trap_receivers_V3_.emplace_back(TrapReceiverV3 { "name1", SecurityLevel::AuthPriv,
      AuthentificationType::MD5, "aaaaaaaa", Privacy::DES, "bbbbbbbb", "192.168.10.1" });
  snmp_config.trap_receivers_V3_.emplace_back(TrapReceiverV3 { "name2", SecurityLevel::AuthPriv,
        AuthentificationType::SHA, "aaaaaaaa", Privacy::AES, "", "192.168.10.2" });
  snmp_config.trap_receivers_V3_.emplace_back(TrapReceiverV3 { "name3", SecurityLevel::AuthNoPriv,
        AuthentificationType::MD5, "aaaaaaaa", Privacy::None, "", "192.168.10.3" });
  snmp_config.trap_receivers_V3_.emplace_back(TrapReceiverV3 { "name4", SecurityLevel::NoAuthNoPriv,
        AuthentificationType::None, "", Privacy::None, "", "192.168.10.4" });

  ::std::string str_config = SnmpV3ParameterToString(snmp_config);


  ::std::string expected_snmp_config =
      R"(trapsess -v 3 -u name1 -a MD5 -A aaaaaaaa -x DES -X bbbbbbbb -l authPriv 192.168.10.1
trapsess -v 3 -u name2 -a SHA -A aaaaaaaa -x AES -X aaaaaaaa -l authPriv 192.168.10.2
trapsess -v 3 -u name3 -a MD5 -A aaaaaaaa -l authNoPriv 192.168.10.3
trapsess -v 3 -u name4 -l noAuthNoPriv 192.168.10.4
)";

  EXPECT_EQ(expected_snmp_config, str_config);
}

TEST(ConfigConversionTest, ConvertDifferentV3UserConfigurationsToString ) {
  SnmpConfig snmp_config;
  snmp_config.user_.emplace_back(User { "name1", Access::ReadWrite, SecurityLevel::AuthPriv,
      AuthentificationType::MD5, "aaaaaaaa", Privacy::DES, "bbbbbbbb"});
  snmp_config.user_.emplace_back(User { "name2", Access::ReadOnly, SecurityLevel::AuthPriv,
      AuthentificationType::SHA, "aaaaaaaa", Privacy::AES, "aaaaaaaa"});
  snmp_config.user_.emplace_back(User { "name3", Access::ReadWrite, SecurityLevel::AuthNoPriv,
      AuthentificationType::MD5, "aaaaaaaa", Privacy::None, ""});
  snmp_config.user_.emplace_back(User { "name4", Access::ReadWrite, SecurityLevel::NoAuthNoPriv,
      AuthentificationType::None, "", Privacy::None, ""});

  ::std::string str_config = SnmpV3ParameterToString(snmp_config);

  ::std::string expected_snmp_config =
      R"(createUser name1 MD5 aaaaaaaa DES bbbbbbbb
rwuser name1 priv
createUser name2 SHA aaaaaaaa AES aaaaaaaa
rouser name2 priv
createUser name3 MD5 aaaaaaaa
rwuser name3 auth
createUser name4
rwuser name4 noauth
)";

  EXPECT_EQ(expected_snmp_config, str_config);
}

TEST(ConfigConversionTest, ExtractBasicParameterFromConfilLines) {

  ::std::string snmpd_dot_conf =
      R"(
authtrapenable 1
sysServices 79
trapsink 192.168.10.222 TrapCommunity
trapsink 192.168.4.4 trapv2
rwcommunity public
sysName A-Za-z0-9 !?:;.,{}[]()*~'_-@
sysDescr desc
sysObjectID 0.1.222.3.4.5
trap2sink 192.168.10.223 travV2Community
sysLocation An der Werkbank
sysContact test@abc.de)";

  auto config_lines = ExtractConfigLines(snmpd_dot_conf);

  SnmpConfig snmp_config;
  ExtractBasicParamters(config_lines, snmp_config);

  SnmpConfig expected_snmp_config;
  expected_snmp_config.sys_name_ = "A-Za-z0-9 !?:;.,{}[]()*~'_-@";
  expected_snmp_config.sys_contact_ = "test@abc.de";
  expected_snmp_config.sys_descr_ = "desc";
  expected_snmp_config.sys_location_ = "An der Werkbank";
  expected_snmp_config.sys_objectID_ = "0.1.222.3.4.5";
  expected_snmp_config.sys_service_ = "79";

  EXPECT_EQ(expected_snmp_config.sys_name_, snmp_config.sys_name_);
  EXPECT_EQ(expected_snmp_config.sys_contact_, snmp_config.sys_contact_);
  EXPECT_EQ(expected_snmp_config.sys_descr_, snmp_config.sys_descr_);
  EXPECT_EQ(expected_snmp_config.sys_location_, snmp_config.sys_location_);
  EXPECT_EQ(expected_snmp_config.sys_objectID_, snmp_config.sys_objectID_);
  EXPECT_EQ(expected_snmp_config.sys_service_, snmp_config.sys_service_);
}

TEST(ConfigConversionTest, ExtractCommunityParameterFromConfigLines) {
  ::std::string snmpd_dot_conf = R"(
  rwcommunity public
  sysDescr ...
  rocommunity private
  sysObjectID ...)";

  auto config_lines = ExtractConfigLines(snmpd_dot_conf);

  SnmpConfig snmp_config;
  ExtractCommunityParameters(config_lines, snmp_config);

  EXPECT_EQ(2, snmp_config.communities_.size());
  EXPECT_THAT(snmp_config.communities_, Contains(Community { "public", Access::ReadWrite }));
  EXPECT_THAT(snmp_config.communities_, Contains(Community { "private", Access::ReadOnly }));

}

TEST(ConfigConversionTest, ExtractTrapV1V2cParameterFromConfigLines) {
  ::std::string snmpd_dot_conf =
      R"(
sysDescr ...
trapsink HostV1 CommunityV1
sysObjectID ...
trap2sink HostV2 CommunityV2)";

  auto config_lines = ExtractConfigLines(snmpd_dot_conf);

  SnmpConfig snmp_config;
  ExtractTrapV1V2cParameters(config_lines, snmp_config);

  EXPECT_EQ(2, snmp_config.trap_receivers_V1V2c_.size());
  EXPECT_THAT(snmp_config.trap_receivers_V1V2c_,
              Contains(TrapReceiverV1V2c { "HostV1", "CommunityV1", VersionV1V2c::V1 }));
  EXPECT_THAT(snmp_config.trap_receivers_V1V2c_,
              Contains(TrapReceiverV1V2c { "HostV2", "CommunityV2", VersionV1V2c::V2c }));
}

TEST(ConfigConversionTest, ExtractTrapV1V2cParameterIgnoreTrapWithMissingValues) {
  ::std::string snmpd_dot_conf =
      R"(
sysDescr ...
trapsink CommunityV1
sysObjectID ...
trap2sink HostV2 CommunityV2
trap2sink HostV2)";

  auto config_lines = ExtractConfigLines(snmpd_dot_conf);

  SnmpConfig snmp_config;
  ExtractTrapV1V2cParameters(config_lines, snmp_config);

  EXPECT_EQ(1, snmp_config.trap_receivers_V1V2c_.size());
  EXPECT_THAT(snmp_config.trap_receivers_V1V2c_,
              Contains(TrapReceiverV1V2c { "HostV2", "CommunityV2", VersionV1V2c::V2c }));

}

TEST(ConfigConversionTest, ExtractBasicParameterFromConfilLinesAndIgnoreEmptyValues) {

  ::std::string snmpd_dot_conf =
      R"(
authtrapenable 1
sysServices 79
trapsink 192.168.10.222 TrapCommunity
trapsink 192.168.4.4 trapv2
rwcommunity public
sysName
sysDescr
sysObjectID   0.1.222.3.4.5
trap2sink 192.168.10.223 travV2Community
sysLocation An der Werkbank
sysContact test@abc.de)";

  auto config_lines = ExtractConfigLines(snmpd_dot_conf);

  SnmpConfig snmp_config;
  ExtractBasicParamters(config_lines, snmp_config);

  SnmpConfig expected_snmp_config;
  expected_snmp_config.sys_name_ = "";
  expected_snmp_config.sys_contact_ = "test@abc.de";
  expected_snmp_config.sys_descr_ = "";
  expected_snmp_config.sys_location_ = "An der Werkbank";
  expected_snmp_config.sys_objectID_ = "0.1.222.3.4.5";
  expected_snmp_config.sys_service_ = "79";

  EXPECT_EQ(expected_snmp_config.sys_name_, snmp_config.sys_name_);
  EXPECT_EQ(expected_snmp_config.sys_contact_, snmp_config.sys_contact_);
  EXPECT_EQ(expected_snmp_config.sys_descr_, snmp_config.sys_descr_);
  EXPECT_EQ(expected_snmp_config.sys_location_, snmp_config.sys_location_);
  EXPECT_EQ(expected_snmp_config.sys_objectID_, snmp_config.sys_objectID_);
  EXPECT_EQ(expected_snmp_config.sys_service_, snmp_config.sys_service_);
}

TEST(ConfigConversionTest, ExtractTrapV3ParameterFromConfigLines) {

  ::std::string snmpd_dot_conf =
      R"(trapsess -v 3 -u name1 -a MD5 -A aaaaaaaa -x DES -X bbbbbbbb -l authPriv 192.168.10.1
trapsess -v 3 -u name2 -a SHA -A aaaaaaaa -x AES -X aaaaaaaa -l authPriv 192.168.10.2
trapsess -v 3 -u name3 -a MD5 -A aaaaaaaa -l authNoPriv 192.168.10.3
trapsess -v 3 -u name4 -l noAuthNoPriv 192.168.10.4
)";

  auto config_lines = ExtractConfigLines(snmpd_dot_conf);

  SnmpConfig snmp_config;
  ExtractTrapV3Parameters(config_lines, snmp_config);

  SnmpConfig expected_snmp_config;
  expected_snmp_config.trap_receivers_V3_.emplace_back(TrapReceiverV3 { "name1", SecurityLevel::AuthPriv,
      AuthentificationType::MD5, "aaaaaaaa", Privacy::DES, "bbbbbbbb", "192.168.10.1" });
  expected_snmp_config.trap_receivers_V3_.emplace_back(TrapReceiverV3 { "name2", SecurityLevel::AuthPriv,
        AuthentificationType::SHA, "aaaaaaaa", Privacy::AES, "", "192.168.10.2" });
  expected_snmp_config.trap_receivers_V3_.emplace_back(TrapReceiverV3 { "name3", SecurityLevel::AuthNoPriv,
        AuthentificationType::MD5, "aaaaaaaa", Privacy::None, "", "192.168.10.3" });
  expected_snmp_config.trap_receivers_V3_.emplace_back(TrapReceiverV3 { "name4", SecurityLevel::NoAuthNoPriv,
        AuthentificationType::None, "", Privacy::None, "", "192.168.10.4" });

  EXPECT_EQ(expected_snmp_config.trap_receivers_V3_, snmp_config.trap_receivers_V3_);
}

TEST(ConfigConversionTest, ExtractUserParameterFromConfigLines) {
  ::std::string snmpd_dot_conf =
      R"(createUser name1 MD5 aaaaaaaa DES bbbbbbbb
     rwuser name1 priv
     createUser name2 SHA aaaaaaaa AES aaaaaaaa
     rouser name2 priv
     createUser name3 MD5 aaaaaaaa
     rwuser name3 auth
     createUser name4
     rwuser name4 noauth
     )";

  auto config_lines = ExtractConfigLines(snmpd_dot_conf);

  SnmpConfig snmp_config;
  ExtractUserParameters(config_lines, snmp_config);

  SnmpConfig expected_snmp_config;
  expected_snmp_config.user_.emplace_back(User { "name1", Access::ReadWrite, SecurityLevel::AuthPriv,
      AuthentificationType::MD5, "aaaaaaaa", Privacy::DES, "bbbbbbbb"});
  expected_snmp_config.user_.emplace_back(User { "name2", Access::ReadOnly, SecurityLevel::AuthPriv,
      AuthentificationType::SHA, "aaaaaaaa", Privacy::AES, "aaaaaaaa"});
  expected_snmp_config.user_.emplace_back(User { "name3", Access::ReadWrite, SecurityLevel::AuthNoPriv,
      AuthentificationType::MD5, "aaaaaaaa", Privacy::None, ""});
  expected_snmp_config.user_.emplace_back(User { "name4", Access::ReadWrite, SecurityLevel::NoAuthNoPriv,
      AuthentificationType::None, "", Privacy::None, ""});

  EXPECT_EQ(expected_snmp_config.user_, snmp_config.user_);
}

}  // wago::util
