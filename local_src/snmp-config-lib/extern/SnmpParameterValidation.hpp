#pragma once

#include <string>

#include "Status.hpp"
#include "SnmpConfig.hpp"

namespace wago::snmp_config_lib{

Statuses Validate(const SnmpConfig &config);

bool IsValidName(const ::std::string& value);
bool IsValidDescription(const ::std::string& value);
bool IsValidContact(const ::std::string& value);
bool IsValidLocation(const ::std::string& value);
bool IsValidObjectId(const ::std::string& value);
bool IsValidCommunityName(const ::std::string &value);
bool IsValidHost(const ::std::string &value);
bool IsValidUserName(const ::std::string &value);
bool IsValidAuthentificationKey(const ::std::string &value);
bool IsValidPrivacyKey(const ::std::string &value);

}
