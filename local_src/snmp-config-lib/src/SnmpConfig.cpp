//------------------------------------------------------------------------------
// Copyright (c) WAGO GmbH & Co. KG
//
// PROPRIETARY RIGHTS are involved in the subject matter of this material. All
// manufacturing, reproduction, use and sales rights pertaining to this
// subject matter are governed by the license agreement. The recipient of this
// software implicitly accepts the terms of the license.
//------------------------------------------------------------------------------

#include "SnmpConfig.hpp"

#include "ConfigConversion.hpp"
#include "Constants.hpp"
#include "ControlSnmp.hpp"
#include "ConfigLine.hpp"
#include "Program.hpp"
#include "File.hpp"
#include "EventFolder.hpp"
#include "Status.hpp"
#include "SnmpParameterValidation.hpp"
#include "SnmpUserConf.hpp"

namespace wago::snmp_config_lib {

namespace {

void FromConfigLines(const std::vector<ConfigLine> &lines, SnmpConfig &sc) {
  ExtractBasicParamters(lines, sc);
  ExtractCommunityParameters(lines, sc);
  ExtractTrapV1V2cParameters(lines, sc);
  ExtractUserParameters(lines, sc);
  ExtractTrapV3Parameters(lines, sc);
}

void ReadConfigFile(const ::std::string &&path, SnmpConfig &snmp_config) {
  auto data = wago::util::ReadFile(path);
  std::vector<ConfigLine> config_lines = ExtractConfigLines(data);
  FromConfigLines(config_lines, snmp_config);
}

void ReadConfigFiles(SnmpConfig &snmp_config) {
  ReadConfigFile(Constants::GetSnmpConfPath(), snmp_config);
  ReadConfigFile(Constants::GetSnmpV3ConfPath(), snmp_config);
}

void WriteConfigFiles(const SnmpConfig &config) {
  SnmpConfig actual_config;
  ReadConfigFile(Constants::GetSnmpConfPath(), actual_config);
  UpdateSnmpUserConfFile(Constants::GetSnmpUserConfPath(), config, actual_config);

  wago::util::WriteFile(Constants::GetSnmpConfPath(), SnmpBasicAndV1V2cParameterToString(config));
  wago::util::WriteFile(Constants::GetSnmpV3ConfPath(), SnmpV3ParameterToString(config));

}

}  //namespace

Community::Community(::std::string name, Access access)
    : name_ { ::std::move(name) },
      access_ { access } {
}

bool Community::operator ==(const Community &rhs) const {
  return (name_ == rhs.name_ && access_ == rhs.access_);
}

bool Community::operator <(const Community &rhs) const {
  return name_ < rhs.name_;
}

TrapReceiverV1V2c::TrapReceiverV1V2c(::std::string host, ::std::string community, VersionV1V2c version)
    : host_ { ::std::move(host) },
      community_ { ::std::move(community) },
      version_ { version } {

}

bool TrapReceiverV1V2c::operator ==(const TrapReceiverV1V2c &rhs) const {
  return (host_ == rhs.host_ && community_ == rhs.community_ && version_ == rhs.version_);
}

bool TrapReceiverV1V2c::operator <(const TrapReceiverV1V2c &rhs) const {
  const auto lhs_key = community_ + host_;
  const auto rhs_key = rhs.community_ + rhs.host_;
  return lhs_key < rhs_key;
}

TrapReceiverV3::TrapReceiverV3(::std::string name, SecurityLevel security_level,
                               AuthentificationType authentification_type, ::std::string authentification_key,
                               Privacy privacy, ::std::string privacy_key, ::std::string host)
    : name_ { ::std::move(name) },
      security_level_ { security_level },
      authentification_type_ { authentification_type },
      authentification_key_ { ::std::move(authentification_key) },
      privacy_ { privacy },
      privacy_key_ { ::std::move(privacy_key) },
      host_ { ::std::move(host) } {

  //The standard dictates that the private key is the same as the authentication key if the privacy key is not explicitly specified
  if (privacy != Privacy::None && privacy_key_.empty()) {
    privacy_key_ = authentification_key_;
  }

}

bool TrapReceiverV3::operator ==(const TrapReceiverV3 &rhs) const {
  return (name_ == rhs.name_ && host_ == rhs.host_ && security_level_ == rhs.security_level_
      && authentification_key_ == rhs.authentification_key_ && authentification_type_ == rhs.authentification_type_
      && privacy_ == rhs.privacy_ && privacy_key_ == rhs.privacy_key_);
}

bool TrapReceiverV3::operator <(const TrapReceiverV3 &rhs) const {
  const auto lhs_key = name_ + host_;
  const auto rhs_key = name_ + rhs.host_;
  return lhs_key < rhs_key;
}

User::User(::std::string &name)
    : name_ { name } {
}

User::User(::std::string name, Access access, SecurityLevel security_level, AuthentificationType authentification_type,
           ::std::string authentification_key, Privacy privacy, ::std::string privacy_key)
    : name_ { ::std::move(name) },
      access_ { access },
      security_level_ { security_level },
      authentification_type_ { authentification_type },
      authentification_key_ { ::std::move(authentification_key) },
      privacy_ { privacy },
      privacy_key_ { ::std::move(privacy_key) } {
}

bool User::operator ==(const User &rhs) const {
  return (name_ == rhs.name_ && access_ == rhs.access_ && security_level_ == rhs.security_level_
      && authentification_key_ == rhs.authentification_key_ && authentification_type_ == rhs.authentification_type_
      && privacy_ == rhs.privacy_ && privacy_key_ == rhs.privacy_key_);
}

bool User::operator <(const User &rhs) const {
  return name_ < rhs.name_;
}

bool SnmpConfig::operator ==(const SnmpConfig &rhs) const {
  return (snmp_enable_ == rhs.snmp_enable_ && sys_name_ == rhs.sys_name_ && sys_contact_ == rhs.sys_contact_
      && sys_descr_ == rhs.sys_descr_ && sys_location_ == rhs.sys_location_ && sys_objectID_ == rhs.sys_objectID_
      && sys_service_ == rhs.sys_service_ && trap_receivers_V1V2c_ == rhs.trap_receivers_V1V2c_
      && trap_receivers_V3_ == rhs.trap_receivers_V3_ && communities_ == rhs.communities_ && user_ == rhs.user_);
}

SnmpConfig ReadSnmpConfig() {
  SnmpConfig snmp_config;

  ReadConfigFiles(snmp_config);

  snmp_config.snmp_enable_ = IsSnmpRunning();

  return snmp_config;
}

Statuses WriteSnmpConfig(const SnmpConfig &config) {
  Statuses s = Validate(config);

  if (!s.IsOk()) {
    return s;
  }

  WriteConfigFiles(config);

  if (config.snmp_enable_) {
    auto state = StartDaemons();
    if (state == AlreadyRunning) {
      InformDaemonAboutChangedConfig();
    }
  } else {
    StopDaemons();
  }

  TriggerEventFolder();

  return s;
}

}
