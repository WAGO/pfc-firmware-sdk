/*
 * SnmpParameterValidation.cpp
 *
 *  Created on: 14.02.2022
 *      Author: u016903
 */

#include "SnmpParameterValidation.hpp"

#include <regex>

namespace wago::snmp_config_lib {

namespace {

bool AnyString(const ::std::string &value) {
  ::std::regex expr { "[A-Za-z0-9 !?:;\\.,{}\\[\\]()*~'_\\-@+]{0,255}" };  //NOLINT(modernize-raw-string-literal) regex doesn't work with raw string
  ::std::smatch what;
  return ::std::regex_match(value, what, expr);
}

bool SelectedCharsLength8_32(const ::std::string &value) {
  ::std::regex expr { "[A-Za-z0-9!()*~'._-]{8,32}" };  //NOLINT(modernize-raw-string-literal) regex doesn't work with raw string
  ::std::smatch what;
  return ::std::regex_match(value, what, expr);
}

Statuses ValidateBasicParameter(const SnmpConfig &config) {
  Statuses s;
  if (not IsValidName(config.sys_name_)) {
    s.AddErrorStatus(Status { StatusCode::WRONG_PARAMETER_PATTERN, "invalid name: " + config.sys_name_ });
  }
  if (not IsValidContact(config.sys_contact_)) {
    s.AddErrorStatus(Status { StatusCode::WRONG_PARAMETER_PATTERN, "invalid contact " + config.sys_contact_ });
  }
  if (not IsValidDescription(config.sys_descr_)) {
    s.AddErrorStatus(Status { StatusCode::WRONG_PARAMETER_PATTERN, "invalid description " + config.sys_descr_ });
  }
  if (not IsValidLocation(config.sys_location_)) {
    s.AddErrorStatus(Status { StatusCode::WRONG_PARAMETER_PATTERN, "invalid location: " + config.sys_location_ });
  }
  if (not IsValidObjectId(config.sys_objectID_)) {
    s.AddErrorStatus(Status { StatusCode::WRONG_PARAMETER_PATTERN, "invalid objectID: " + config.sys_objectID_ });
  }
  return s;
}

Statuses ValidateTrapV1V2cParameter(const SnmpConfig &config) {
  Statuses s;
  for (auto &trap : config.trap_receivers_V1V2c_) {
    if (not IsValidHost(trap.host_)) {
      s.AddErrorStatus(Status { StatusCode::WRONG_PARAMETER_PATTERN, "invalid host for trap receiver: " + trap.host_ });
    }
    if (not IsValidCommunityName(trap.community_)) {
      s.AddErrorStatus(
          Status { StatusCode::WRONG_PARAMETER_PATTERN, "invalid community name for trap receiver: " + trap.community_ });
    }
  }
  return s;
}

Statuses ValidateCommunityParameter(const SnmpConfig &config) {
  Statuses s;
  for (auto &community : config.communities_) {
    if (not IsValidCommunityName(community.name_)) {
      s.AddErrorStatus(Status { StatusCode::WRONG_PARAMETER_PATTERN, "invalid community name: " + community.name_ });
    }
  }
  return s;
}

Statuses ValidateTrapV3Parameter(const SnmpConfig &config) {
  Statuses s;
  for (auto &trap : config.trap_receivers_V3_) {
    if (not IsValidName(trap.name_)) {
      s.AddErrorStatus(Status { StatusCode::WRONG_PARAMETER_PATTERN, "invalid name for trap receiver: " + trap.name_ });
    }
    if (not IsValidHost(trap.host_)) {
      s.AddErrorStatus(Status { StatusCode::WRONG_PARAMETER_PATTERN, "invalid host for trap receiver: " + trap.host_ });
    }

    if (trap.security_level_ == SecurityLevel::AuthNoPriv || trap.security_level_ == SecurityLevel::AuthPriv) {
      if (not IsValidAuthentificationKey(trap.authentification_key_)) {
        s.AddErrorStatus(
            Status { StatusCode::WRONG_PARAMETER_PATTERN, "invalid authentification key for trap receiver: "
                + trap.authentification_key_ });
      }
    }

    if (trap.security_level_ == SecurityLevel::AuthPriv) {
      if (not IsValidPrivacyKey(trap.privacy_key_)) {
        s.AddErrorStatus(
            Status { StatusCode::WRONG_PARAMETER_PATTERN, "invalid privacy key for trap receiver: " + trap.privacy_key_ });
      }
    }
  }
  return s;
}

Statuses ValidateUserParameter(const SnmpConfig &config) {
  Statuses s;
  for (auto &user : config.user_) {
    if (not IsValidName(user.name_)) {
      s.AddErrorStatus(Status { StatusCode::WRONG_PARAMETER_PATTERN, "invalid name for user: " + user.name_ });
    }

    if (user.security_level_ == SecurityLevel::AuthPriv || user.security_level_ == SecurityLevel::AuthNoPriv) {
      if (not IsValidAuthentificationKey(user.authentification_key_)) {
        s.AddErrorStatus(
            Status { StatusCode::WRONG_PARAMETER_PATTERN, "invalid authentification key for user: "
                + user.authentification_key_ });
      }
    }

    if (user.security_level_ == SecurityLevel::AuthPriv) {
      if (not IsValidPrivacyKey(user.privacy_key_)) {
        s.AddErrorStatus(
            Status { StatusCode::WRONG_PARAMETER_PATTERN, "invalid privacy key for user: " + user.privacy_key_ });
      }
    }
  }
  return s;
}

}  //namespace

bool IsValidName(const ::std::string &value) {
  return AnyString(value);
}

bool IsValidDescription(const ::std::string &value) {
  return AnyString(value);
}

bool IsValidContact(const ::std::string &value) {
  return AnyString(value);
}

bool IsValidLocation(const ::std::string &value) {
  return AnyString(value);
}

bool IsValidObjectId(const ::std::string &value) {
  if (value.size() > 128) {
    return false;
  }
  ::std::regex expr { "(\\.\\d+)*" };
  ::std::smatch what;
  return ::std::regex_match(value, what, expr);
}

bool IsValidCommunityName(const ::std::string &value) {
  ::std::regex expr { "[A-Za-z0-9!?:;\\.,{}\\[\\]()*~'_\\-@+]{1,32}" };  //NOLINT(modernize-raw-string-literal) regex doesn't work with raw string
  ::std::smatch what;
  return ::std::regex_match(value, what, expr);
}

bool IsValidHost(const ::std::string &value) {
  ::std::regex expr { "[a-zA-Z0-9\\-\\.]{1,255}" };  //NOLINT(modernize-raw-string-literal) regex doesn't work with raw string
  ::std::smatch what;
  return ::std::regex_match(value, what, expr);
}

bool IsValidUserName(const ::std::string &value) {
  return SelectedCharsLength8_32(value);
}

bool IsValidAuthentificationKey(const ::std::string &value) {
  return SelectedCharsLength8_32(value);
}

bool IsValidPrivacyKey(const ::std::string &value) {
  ::std::regex expr { "^$|[A-Za-z0-9!()*~'._-]{8,32}" };  //NOLINT(modernize-raw-string-literal) regex doesn't work with raw string
  ::std::smatch what;
  return ::std::regex_match(value, what, expr);
}

Statuses Validate(const SnmpConfig &config) {
  Statuses s;
  s.AddErrorStatuses(ValidateBasicParameter(config));
  s.AddErrorStatuses(ValidateCommunityParameter(config));
  s.AddErrorStatuses(ValidateTrapV1V2cParameter(config));
  s.AddErrorStatuses(ValidateUserParameter(config));
  s.AddErrorStatuses(ValidateTrapV3Parameter(config));

  return s;
}

}  //wago::snmp_config_lib
