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

#include <regex>

#include "SnmpParameterValidation.hpp"

namespace wago::snmp_config_lib {

TEST(SnmpParameterValidation, validValues) {

  ::std::string valid_value = "aB!?:;.,{}[]*~@123";
  EXPECT_TRUE(IsValidName(valid_value));
  EXPECT_TRUE(IsValidContact(valid_value));
  EXPECT_TRUE(IsValidDescription(valid_value));
  EXPECT_TRUE(IsValidLocation(valid_value));

  ::std::string empty_value;
  EXPECT_TRUE(IsValidName(empty_value));
  EXPECT_TRUE(IsValidContact(empty_value));
  EXPECT_TRUE(IsValidDescription(empty_value));
  EXPECT_TRUE(IsValidLocation(empty_value));
}

TEST(SnmpParameterValidation, invalidValues) {

  ::std::string too_long(256, 'a');
  EXPECT_FALSE(IsValidName(too_long));
  EXPECT_FALSE(IsValidContact(too_long));
  EXPECT_FALSE(IsValidDescription(too_long));
  EXPECT_FALSE(IsValidLocation(too_long));

  ::std::string invalid_char = "$";
  EXPECT_FALSE(IsValidName(invalid_char));
  EXPECT_FALSE(IsValidContact(invalid_char));
  EXPECT_FALSE(IsValidDescription(invalid_char));
  EXPECT_FALSE(IsValidLocation(invalid_char));

  ::std::string invalid_chars_umlauts = "äüö";
  EXPECT_FALSE(IsValidName(invalid_chars_umlauts));
  EXPECT_FALSE(IsValidContact(invalid_chars_umlauts));
  EXPECT_FALSE(IsValidDescription(invalid_chars_umlauts));
  EXPECT_FALSE(IsValidLocation(invalid_chars_umlauts));
}

TEST(SnmpParameterValidation, validObjectID) {

  ::std::string valid_value = ".1.2.333.4.5.6";
  EXPECT_TRUE(IsValidObjectId(valid_value));

  ::std::string empty_value;
  EXPECT_TRUE(IsValidObjectId(empty_value));
}

TEST(SnmpParameterValidation, invalidObjectID) {

  ::std::string invalid_value = ".a.b.c";
  EXPECT_FALSE(IsValidObjectId(invalid_value));

  ::std::string too_long(129, '1');
  EXPECT_FALSE(IsValidObjectId(too_long));

  ::std::string missing_dot_at_beginning("1.2.3");
  EXPECT_FALSE(IsValidObjectId(missing_dot_at_beginning));

  ::std::string dot_at_the_end(".1.2.3.");
  EXPECT_FALSE(IsValidObjectId(dot_at_the_end));

  ::std::string double_dot(".1..2");
  EXPECT_FALSE(IsValidObjectId(double_dot));
}

TEST(SnmpParameterValidation, validCommunityName) {

  ::std::string valid_value = "AA09!?:;:{}()[]@";
  EXPECT_TRUE(IsValidCommunityName(valid_value));

}

TEST(SnmpParameterValidation, invalidCommunityName) {

  ::std::string invalid_value = "$";
  EXPECT_FALSE(IsValidCommunityName(invalid_value));

  ::std::string too_long(33, '1');
  EXPECT_FALSE(IsValidCommunityName(too_long));

  ::std::string invalid_chars_umlauts = "äüö";
  EXPECT_FALSE(IsValidCommunityName(invalid_chars_umlauts));

  ::std::string empty_value;
  EXPECT_FALSE(IsValidCommunityName(empty_value));
}

TEST(SnmpParameterValidation, validHost) {

  ::std::string valid_value = "aB-.";
  EXPECT_TRUE(IsValidHost(valid_value));
}

TEST(SnmpParameterValidation, invalidHost) {

  ::std::string invalid_value = "()";
  EXPECT_FALSE(IsValidHost(invalid_value));

  ::std::string too_long(256, '1');
  EXPECT_FALSE(IsValidHost(too_long));

  ::std::string empty_value;
  EXPECT_FALSE(IsValidHost(empty_value));

  ::std::string invalid_chars_umlauts = "äüö";
  EXPECT_FALSE(IsValidHost(invalid_chars_umlauts));
}

TEST(SnmpParameterValidation, validUserAuthentificationPrivacyKey) {

  ::std::string short_value(8, 'A');
  EXPECT_TRUE(IsValidUserName(short_value));
  EXPECT_TRUE(IsValidAuthentificationKey(short_value));
  EXPECT_TRUE(IsValidPrivacyKey(short_value));

  ::std::string long_value(32, 'A');
  EXPECT_TRUE(IsValidUserName(long_value));
  EXPECT_TRUE(IsValidAuthentificationKey(long_value));
  EXPECT_TRUE(IsValidPrivacyKey(long_value));

  ::std::string valid_values("aB09!()*~'._-");
  EXPECT_TRUE(IsValidUserName(valid_values));
  EXPECT_TRUE(IsValidAuthentificationKey(valid_values));
  EXPECT_TRUE(IsValidPrivacyKey(valid_values));

  ::std::string valid_empty_value;
  EXPECT_TRUE(IsValidPrivacyKey(valid_empty_value));

}

TEST(SnmpParameterValidation, invalidUserAuthentificationPrivacyKey) {

  ::std::string to_short(7, 'b');
  EXPECT_FALSE(IsValidUserName(to_short));
  EXPECT_FALSE(IsValidAuthentificationKey(to_short));
  EXPECT_FALSE(IsValidPrivacyKey(to_short));

  ::std::string to_long(33, 'b');
  EXPECT_FALSE(IsValidUserName(to_long));
  EXPECT_FALSE(IsValidAuthentificationKey(to_long));
  EXPECT_FALSE(IsValidPrivacyKey(to_long));

  ::std::string invalid_value = "[]";
  EXPECT_FALSE(IsValidUserName(invalid_value));
  EXPECT_FALSE(IsValidAuthentificationKey(invalid_value));
  EXPECT_FALSE(IsValidPrivacyKey(invalid_value));

  ::std::string invalid_chars_umlauts = "äüö";
  EXPECT_FALSE(IsValidUserName(invalid_chars_umlauts));
  EXPECT_FALSE(IsValidAuthentificationKey(invalid_chars_umlauts));
  EXPECT_FALSE(IsValidPrivacyKey(invalid_chars_umlauts));
}

TEST(SnmpParameterValidation, validateSnmpConfig) {

  SnmpConfig config;
  config.sys_name_ = "$";  //invalid
  config.sys_location_ = "b";  //valid
  config.sys_objectID_ = "0";  //invalid
  config.communities_.emplace_back(Community { "$", Access::ReadOnly });  //invalid
  config.communities_.emplace_back(Community { "a", Access::ReadOnly });  //valid
  config.communities_.emplace_back(Community { "$$", Access::ReadWrite });  //invalid
  config.trap_receivers_V1V2c_.emplace_back(TrapReceiverV1V2c { "$", "a", VersionV1V2c::V1 });  //invalid
  config.trap_receivers_V1V2c_.emplace_back(TrapReceiverV1V2c { "b", "$$", VersionV1V2c::V2c });  //invalid
  config.trap_receivers_V1V2c_.emplace_back(TrapReceiverV1V2c { "b", "a", VersionV1V2c::V2c });  //valid

  auto result = Validate(config);

  EXPECT_FALSE(result.IsOk());
  EXPECT_EQ(6, result.GetStatuses().size());
}

TEST(SnmpParameterValidation, validateTrapV3ConfigAuthPriv) {
  SnmpConfig config;
  config.trap_receivers_V3_.emplace_back(TrapReceiverV3 { "name1234", SecurityLevel::AuthPriv,
      AuthentificationType::MD5, "aaaaaaaa", Privacy::AES, "bbbbbbbb", "192.168.1.1" });
  auto result = Validate(config);
  EXPECT_TRUE(result.IsOk());
}

TEST(SnmpParameterValidation, validateTrapV3ConfigAuthNoPriv) {
  SnmpConfig config;
  config.trap_receivers_V3_.emplace_back(TrapReceiverV3 { "name1234", SecurityLevel::AuthNoPriv,
      AuthentificationType::MD5, "aaaaaaaa", Privacy::None, "", "192.168.1.1" });
  auto result = Validate(config);
  EXPECT_TRUE(result.IsOk());
}

TEST(SnmpParameterValidation, validateTrapV3ConfigNoAuthNoPriv) {
  SnmpConfig config;
  config.trap_receivers_V3_.emplace_back(TrapReceiverV3 { "name1234", SecurityLevel::NoAuthNoPriv,
      AuthentificationType::None, "", Privacy::None, "", "192.168.1.1" });
  auto result = Validate(config);
  EXPECT_TRUE(result.IsOk());
}

TEST(SnmpParameterValidation, validateUserV3ConfigAuthPriv) {
  SnmpConfig config;
  config.user_.emplace_back(User { "name1234", Access::ReadWrite, SecurityLevel::AuthPriv,
      AuthentificationType::MD5, "aaaaaaaa", Privacy::AES, "bbbbbbbb" });
  auto result = Validate(config);
  EXPECT_TRUE(result.IsOk());
}

TEST(SnmpParameterValidation, validateUserV3ConfigAuthNoPriv) {
  SnmpConfig config;
  config.user_.emplace_back(User { "name1234", Access::ReadWrite, SecurityLevel::AuthNoPriv,
      AuthentificationType::MD5, "aaaaaaaa", Privacy::None, "" });
  auto result = Validate(config);
  EXPECT_TRUE(result.IsOk());
}

TEST(SnmpParameterValidation, validateUserV3ConfigNoAuthNoPriv) {
  SnmpConfig config;
  config.user_.emplace_back(User { "name1234", Access::ReadWrite, SecurityLevel::NoAuthNoPriv,
      AuthentificationType::None, "", Privacy::None, "" });
  auto result = Validate(config);
  EXPECT_TRUE(result.IsOk());
}

}  // wago::util
