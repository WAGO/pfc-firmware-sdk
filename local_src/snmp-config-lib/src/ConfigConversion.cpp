//------------------------------------------------------------------------------
// Copyright (c) WAGO GmbH & Co. KG
//
// PROPRIETARY RIGHTS are involved in the subject matter of this material. All
// manufacturing, reproduction, use and sales rights pertaining to this
// subject matter are governed by the license agreement. The recipient of this
// software implicitly accepts the terms of the license.
//------------------------------------------------------------------------------

#include "ConfigConversion.hpp"

#include <regex>
#include <string>

namespace wago::snmp_config_lib {

namespace {

constexpr const char* AuthentificationTypeToString(AuthentificationType a) {
  switch (a) {
    case AuthentificationType::MD5:
      return "MD5";
    case AuthentificationType::SHA:
      return "SHA";
    default:
      return "";
  }
}

AuthentificationType StringToAuthentificationType(::std::string &a) {
  AuthentificationType t = AuthentificationType::None;
  if (a == "MD5") {
    t = AuthentificationType::MD5;
  } else if (a == "SHA") {
    t = AuthentificationType::SHA;
  }
  return t;

}

constexpr const char* SecurityLevelToStringShortForm(SecurityLevel s) {
  switch (s) {
    case SecurityLevel::NoAuthNoPriv:
      return "noauth";
    case SecurityLevel::AuthNoPriv:
      return "auth";
    case SecurityLevel::AuthPriv:
      return "priv";
    default:
      return "";
  }
}

constexpr const char* PrivacyToString(Privacy p) {
  switch (p) {
    case Privacy::DES:
      return "DES";
    case Privacy::AES:
      return "AES";
    default:
      return "";
  }
}

Privacy StringToPrivacy(::std::string &a) {
  Privacy p = Privacy::None;
  if (a == "DES") {
    p = Privacy::DES;
  } else if (a == "AES") {
    p = Privacy::AES;
  }
  return p;
}

constexpr const char* SecurityLevelToString(SecurityLevel s) {
  switch (s) {
    case SecurityLevel::NoAuthNoPriv:
      return "noAuthNoPriv";
    case SecurityLevel::AuthNoPriv:
      return "authNoPriv";
    case SecurityLevel::AuthPriv:
      return "authPriv";
    default:
      return "";
  }
}

::std::string BasicParametersToString(const SnmpConfig &s) {
  ::std::stringstream ss;
  ss << "sysServices " << s.sys_service_ << "\n";
  ss << "sysName " << s.sys_name_ << "\n";
  ss << "sysDescr " << s.sys_descr_ << "\n";
  ss << "sysLocation " << s.sys_location_ << "\n";
  ss << "sysContact " << s.sys_contact_ << "\n";
  ss << "sysObjectID " << s.sys_objectID_ << "\n";
  return ss.str();
}

::std::string TrapV1V2cParameterToString(const SnmpConfig &s) {
  ::std::stringstream ss;
  for (auto &trap : s.trap_receivers_V1V2c_) {
    if (trap.version_ == VersionV1V2c::V1) {
      ss << "trapsink " << trap.host_ << " " << trap.community_ << "\n";
    }
    if (trap.version_ == VersionV1V2c::V2c) {
      ss << "trap2sink " << trap.host_ << " " << trap.community_ << "\n";
    }
  }
  return ss.str();
}

::std::string CommunityParameterToString(const SnmpConfig &s) {
  ::std::stringstream ss;
  for (auto &community : s.communities_) {
    if (community.access_ == Access::ReadWrite) {
      ss << "rwcommunity " << community.name_ << "\n";
    }
    if (community.access_ == Access::ReadOnly) {
      ss << "rocommunity " << community.name_ << "\n";
    }
  }
  return ss.str();
}

::std::string UserParameterToString(const SnmpConfig &s) {
  ::std::stringstream ss;
  for (auto &user : s.user_) {

    // CreateUser
    ss << "createUser " << user.name_;
    switch (user.security_level_) {
      case SecurityLevel::AuthNoPriv:
        ss << " " << AuthentificationTypeToString(user.authentification_type_) << " " << user.authentification_key_;
        break;
      case SecurityLevel::AuthPriv:
        ss << " " << AuthentificationTypeToString(user.authentification_type_) << " " << user.authentification_key_;
        if (user.privacy_key_.empty()) {
          ss << " " << PrivacyToString(user.privacy_) << " " << user.authentification_key_;
        } else {
          ss << " " << PrivacyToString(user.privacy_) << " " << user.privacy_key_;
        }
        break;
      default:
        break;
    }
    ss << "\n";

    // set RW/RO user excess
    if (user.access_ == Access::ReadWrite) {
      ss << "rwuser " << user.name_ << " " << SecurityLevelToStringShortForm(user.security_level_) << "\n";
    }
    if (user.access_ == Access::ReadOnly) {
      ss << "rouser " << user.name_ << " " << SecurityLevelToStringShortForm(user.security_level_) << "\n";
    }

  }
  return ss.str();
}

::std::string TrapV3ParameterToString(const SnmpConfig &s) {
  ::std::stringstream ss;
  for (auto &trap : s.trap_receivers_V3_) {
    ss << "trapsess -v 3 -u " << trap.name_;

    switch (trap.security_level_) {
      case SecurityLevel::AuthNoPriv:
        ss << " -a " << AuthentificationTypeToString(trap.authentification_type_) << " -A "
           << trap.authentification_key_;
        break;
      case SecurityLevel::AuthPriv:
        ss << " -a " << AuthentificationTypeToString(trap.authentification_type_) << " -A "
           << trap.authentification_key_;
        if (trap.privacy_key_.empty()) {
          ss << " -x " << PrivacyToString(trap.privacy_) << " -X " << trap.authentification_key_;
        } else {
          ss << " -x " << PrivacyToString(trap.privacy_) << " -X " << trap.privacy_key_;
        }
        break;
      default:
        break;
    }

    ss << " -l " << SecurityLevelToString(trap.security_level_) << " " << trap.host_ << "\n";
  }
  return ss.str();
}

::std::vector<::std::string> SplitBySpace(const ::std::string &str) {

  ::std::vector<::std::string> units;
  ::std::string::const_iterator search_start(str.cbegin());

  ::std::regex exp { R"regex(\S+)regex" };
  ::std::smatch res;
  while (regex_search(search_start, str.cend(), res, exp)) {
    units.emplace_back(res[0]);
    search_start = res.suffix().first;
  }

  return units;
}

::std::string GetSpaceDelimitedParameterByIndex(const ::std::string &str, ::std::uint32_t index) {
  auto parameter = SplitBySpace(str);
  return (parameter.size() > index) ? parameter[index] : "";
}

void ExtractAnyTrapV1V2cParameters(const ConfigLine &l, SnmpConfig &s, const VersionV1V2c version) {
  if (SplitBySpace(l.values_).size() == 2) {
    auto host = GetSpaceDelimitedParameterByIndex(l.values_, 0);
    auto community = GetSpaceDelimitedParameterByIndex(l.values_, 1);
    s.trap_receivers_V1V2c_.emplace_back(TrapReceiverV1V2c { host, community, version });
  }
}

SecurityLevel StringToSecurityLevel(::std::string &s) {

  if (s == "authPriv") {
    return SecurityLevel::AuthPriv;
  }
  if (s == "authNoPriv") {
    return SecurityLevel::AuthNoPriv;
  }
  return SecurityLevel::NoAuthNoPriv;

}

::std::string GetValue(::std::string &&key, ::std::vector<::std::string> values) {
  auto it = ::std::find(values.cbegin(), values.cend(), key);
  if (it != values.end() && it++ != values.end()) {
    return *it;
  }
  return "";
}

void ExtractAnyTrapV3Parameters(const ConfigLine &l, SnmpConfig &s) {

  auto values = SplitBySpace(l.values_);

  ::std::string name = GetValue("-u", values);
  ::std::string value = GetValue("-l", values);
  SecurityLevel security_level = StringToSecurityLevel(value);
  value = GetValue("-a", values);
  AuthentificationType auth_type = StringToAuthentificationType(value);
  ::std::string auth_key = GetValue("-A", values);
  value = GetValue("-x", values);
  Privacy privacy = StringToPrivacy(value);
  ::std::string privacy_key = GetValue("-X", values);
  ::std::string host = values.back();

  s.trap_receivers_V3_.emplace_back(TrapReceiverV3 { name, security_level, auth_type, auth_key, privacy, privacy_key,
      host });

}

std::vector<User>::iterator GetOrCreateUserByName(::std::string &name, ::std::vector<User> &users) {
  auto it = ::std::find_if(users.begin(), users.end(), [&](User &u) {
    return u.name_ == name;
  });
  if (it != users.end()) {
    return it;
  }
  users.emplace_back(User { name });
  return ::std::prev(users.end());
}

void ExtractAnyUserCreateParameters(const ConfigLine &l, SnmpConfig &s) {

  auto values = SplitBySpace(l.values_);

  auto name = values[0];

  SecurityLevel security_level = SecurityLevel::NoAuthNoPriv;
  AuthentificationType auth_type = AuthentificationType::None;
  ::std::string auth_key;
  Privacy privacy = Privacy::None;
  ::std::string privacy_key;

  if (values.size() == 5) {
    security_level = SecurityLevel::AuthPriv;
  } else if (values.size() == 3) {
    security_level = SecurityLevel::AuthNoPriv;
  } else {
    security_level = SecurityLevel::NoAuthNoPriv;
  }

  if (security_level == SecurityLevel::AuthPriv || security_level == SecurityLevel::AuthNoPriv) {
    auth_type = StringToAuthentificationType(values[1]);
    auth_key = values[2];
  }

  if (security_level == SecurityLevel::AuthPriv) {
    privacy = StringToPrivacy(values[3]);
    privacy_key = values[4];
  }

  auto it = GetOrCreateUserByName(name, s.user_);
  (*it).authentification_type_ = auth_type;
  (*it).authentification_key_ = auth_key;
  (*it).privacy_ = privacy;
  (*it).privacy_key_ = privacy_key;
  (*it).security_level_ = security_level;

}

void SetAccessForUser(const ConfigLine &l, SnmpConfig &s, Access access) {
  auto values = SplitBySpace(l.values_);
  auto it = GetOrCreateUserByName(values[0], s.user_);
  (*it).access_ = access;
}

}  // namespace

template<typename T>
using extractor_fn = ::std::function<void(const ConfigLine&, T&)>;

template<typename T>
inline void FillByExtractor(T &to, const std::vector<ConfigLine> &from,
                            const ::std::map<std::string, extractor_fn<T>> &extractors) {
  for (auto &line : from) {
    if (extractors.count(line.parameter_) > 0) {
      extractors.at(line.parameter_)(line, to);
    }
  }
}

std::vector<ConfigLine> ExtractConfigLines(const ::std::string &config) {

  std::vector<ConfigLine> config_lines;
  std::regex config_regex { R"regex((\S+)\ *(.*))regex" };

  auto configs_begin = std::sregex_iterator(config.begin(), config.end(), config_regex);
  auto configs_end = std::sregex_iterator();
  for (auto i = configs_begin; i != configs_end; ++i) {
    std::smatch match = *i;
    ConfigLine cl { { match[1] }, { match[2] } };
    config_lines.push_back(std::move(cl));
  }
  return config_lines;
}

void ExtractBasicParamters(const std::vector<ConfigLine> &lines, SnmpConfig &snmp_config) {

  static const ::std::map<std::string, extractor_fn<SnmpConfig>> extractors { { "sysName", [](const ConfigLine &l,
                                                                                              SnmpConfig &s) {
    s.sys_name_ = l.values_;
  } }, { "sysDescr", [](const ConfigLine &l, SnmpConfig &s) {
    s.sys_descr_ = l.values_;
  } }, { "sysLocation", [](const ConfigLine &l, SnmpConfig &s) {
    s.sys_location_ = l.values_;
  } }, { "sysObjectID", [](const ConfigLine &l, SnmpConfig &s) {
    s.sys_objectID_ = l.values_;
  } }, { "sysServices", [](const ConfigLine &l, SnmpConfig &s) {
    s.sys_service_ = l.values_;
  } }, { "sysContact", [](const ConfigLine &l, SnmpConfig &s) {
    s.sys_contact_ = l.values_;
  } } };

  FillByExtractor(snmp_config, lines, extractors);
}

void ExtractTrapV1V2cParameters(const std::vector<ConfigLine> &lines, SnmpConfig &snmp_config) {

  static const ::std::map<std::string, extractor_fn<SnmpConfig>> extractors { { "trapsink", [](const ConfigLine &l,
                                                                                               SnmpConfig &s) {
    ExtractAnyTrapV1V2cParameters(l, s, VersionV1V2c::V1);
  } }, { "trap2sink", [](const ConfigLine &l, SnmpConfig &s) {
    ExtractAnyTrapV1V2cParameters(l, s, VersionV1V2c::V2c);
  } } };

  FillByExtractor(snmp_config, lines, extractors);
}

void ExtractCommunityParameters(const std::vector<ConfigLine> &lines, SnmpConfig &snmp_config) {

  static const ::std::map<std::string, extractor_fn<SnmpConfig>> extractors { { "rwcommunity", [](const ConfigLine &l,
                                                                                                  SnmpConfig &s) {
    s.communities_.emplace_back(Community { l.values_, Access::ReadWrite });
  } }, { "rocommunity", [](const ConfigLine &l, SnmpConfig &s) {
    s.communities_.emplace_back(Community { l.values_, Access::ReadOnly });
  } } };

  FillByExtractor(snmp_config, lines, extractors);
}

void ExtractUserParameters(const std::vector<ConfigLine> &lines, SnmpConfig &snmp_config) {
  static const ::std::map<std::string, extractor_fn<SnmpConfig>> extractors { { "createUser", [](const ConfigLine &l,
                                                                                                 SnmpConfig &s) {
    ExtractAnyUserCreateParameters(l, s);
  } }, { "rwuser", [](const ConfigLine &l, SnmpConfig &s) {
    SetAccessForUser(l, s, Access::ReadWrite);
  } }, { "rouser", [](const ConfigLine &l, SnmpConfig &s) {
    SetAccessForUser(l, s, Access::ReadOnly);
  } } };

  FillByExtractor(snmp_config, lines, extractors);
}

void ExtractTrapV3Parameters(const std::vector<ConfigLine> &lines, SnmpConfig &snmp_config) {
  static const ::std::map<std::string, extractor_fn<SnmpConfig>> extractors { { "trapsess", [](const ConfigLine &l,
                                                                                               SnmpConfig &s) {
    ExtractAnyTrapV3Parameters(l, s);
  } } };

  FillByExtractor(snmp_config, lines, extractors);
}

std::string SnmpBasicAndV1V2cParameterToString(const SnmpConfig &snmp_config) {

  std::string s;
  s += BasicParametersToString(snmp_config);
  s += CommunityParameterToString(snmp_config);
  s += TrapV1V2cParameterToString(snmp_config);

  return s;
}

std::string SnmpV3ParameterToString(const SnmpConfig &snmp_config) {

  std::string s;
  s += UserParameterToString(snmp_config);
  s += TrapV3ParameterToString(snmp_config);

  return s;
}

}  // namespace wago::snmp_config_lib
