//------------------------------------------------------------------------------
// Copyright (c) WAGO Kontakttechnik GmbH & Co. KG
//
// PROPRIETARY RIGHTS are involved in the subject matter of this material. All
// manufacturing, reproduction, use and sales rights pertaining to this
// subject matter are governed by the license agreement. The recipient of this
// software implicitly accepts the terms of the license.
//------------------------------------------------------------------------------

#pragma once

#include <string>
#include <vector>

#include "Status.hpp"

namespace wago::snmp_config_lib {

enum class VersionV1V2c : uint16_t {
  V1 = 0,
  V2c = 1
};

enum class Access : uint16_t {
  ReadOnly = 0,
  ReadWrite = 1
};

enum class AuthentificationType : uint16_t {
  None = 0,
  MD5 = 1,
  SHA = 2
};

enum class Privacy : uint16_t {
  None = 0,
  DES = 1,
  AES = 2
};

enum class SecurityLevel : uint16_t {
  NoAuthNoPriv = 0,
  AuthNoPriv = 1,
  AuthPriv = 2
};

class Community {
 public:
  Community() = default;
  Community(::std::string name, Access access);

  bool operator ==(const Community &rhs) const;
  bool operator <(const Community &rhs) const;

  ::std::string name_;
  Access access_ = Access::ReadOnly;
};

class TrapReceiverV1V2c {
 public:
  TrapReceiverV1V2c() = default;
  TrapReceiverV1V2c(::std::string host, ::std::string community, VersionV1V2c version);
  bool operator ==(const TrapReceiverV1V2c &rhs) const;
  bool operator <(const TrapReceiverV1V2c &rhs) const;

  ::std::string host_;
  ::std::string community_;
  VersionV1V2c version_ = VersionV1V2c::V2c;
};

class User {
 public:
  User() = default;
  explicit User(::std::string& name);
  User(::std::string name, Access access, SecurityLevel security_level, AuthentificationType authentification_type,
       ::std::string authentification_key, Privacy privacy, ::std::string privacy_key);
  bool operator ==(const User &rhs) const;
  bool operator <(const User &rhs) const;

  ::std::string name_;
  Access access_ = Access::ReadWrite;
  SecurityLevel security_level_ = SecurityLevel::NoAuthNoPriv;
  AuthentificationType authentification_type_ = AuthentificationType::None;
  ::std::string authentification_key_;
  Privacy privacy_ = Privacy::None;
  ::std::string privacy_key_;
};

class TrapReceiverV3 {
 public:
  TrapReceiverV3() = default;
  TrapReceiverV3(::std::string name, SecurityLevel security_level, AuthentificationType authentification_type,
                 ::std::string authentification_key, Privacy privacy, ::std::string privacy_key, ::std::string host);
  bool operator ==(const TrapReceiverV3 &rhs) const;
  bool operator <(const TrapReceiverV3 &rhs) const;

  ::std::string name_;
  SecurityLevel security_level_ = SecurityLevel::NoAuthNoPriv;
  AuthentificationType authentification_type_ = AuthentificationType::None;
  ::std::string authentification_key_;
  Privacy privacy_ = Privacy::None;
  ::std::string privacy_key_;
  ::std::string host_;

};

class SnmpConfig {
 public:
  SnmpConfig() = default;
  virtual ~SnmpConfig() = default;
  SnmpConfig(const SnmpConfig &other) = default;
  SnmpConfig(SnmpConfig &&other) = default;
  SnmpConfig& operator=(const SnmpConfig &other) = default;
  SnmpConfig& operator=(SnmpConfig &&other) = default;
  bool operator ==(const SnmpConfig &rhs) const;

  bool snmp_enable_ = false;

  ::std::string sys_name_;
  ::std::string sys_descr_;
  ::std::string sys_service_;
  ::std::string sys_location_;
  ::std::string sys_objectID_;
  ::std::string sys_contact_;

  ::std::vector<Community> communities_;
  ::std::vector<TrapReceiverV1V2c> trap_receivers_V1V2c_;
  ::std::vector<User> user_;
  ::std::vector<TrapReceiverV3> trap_receivers_V3_;

};

SnmpConfig ReadSnmpConfig();

Statuses WriteSnmpConfig(const SnmpConfig &config);

}

