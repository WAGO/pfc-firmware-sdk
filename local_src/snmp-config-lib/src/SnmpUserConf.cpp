//------------------------------------------------------------------------------
// Copyright (c) WAGO GmbH & Co. KG
//
// PROPRIETARY RIGHTS are involved in the subject matter of this material. All
// manufacturing, reproduction, use and sales rights pertaining to this
// subject matter are governed by the license agreement. The recipient of this
// software implicitly accepts the terms of the license.
//------------------------------------------------------------------------------
#include "SnmpUserConf.hpp"

#include <sstream>

#include "KeyValueParser.hpp"
#include "File.hpp"

namespace wago::snmp_config_lib {

namespace {

::std::string SnmpUserConfToString(::std::map<::std::string, ::std::string> config) {
  ::std::stringstream ss;
  ss << "USE_USER_DEFINED_DEVICE_NAME=" << config["USE_USER_DEFINED_DEVICE_NAME"] << "\n";
  ss << "USE_USER_DEFINED_DESCRIPTION=" << config["USE_USER_DEFINED_DESCRIPTION"] << "\n";
  return ss.str();
}

}  //namespace

void UpdateSnmpUserConfData(::std::string &data, const SnmpConfig &new_config, const SnmpConfig &actual_config) {

  auto user_conf = wago::util::ParseKeyValuePairs(data);

  if(user_conf.empty()){
    user_conf["USE_USER_DEFINED_DEVICE_NAME"] = "no";
    user_conf["USE_USER_DEFINED_DESCRIPTION"] = "no";
  }

  if (new_config.sys_name_ != actual_config.sys_name_) {
    user_conf["USE_USER_DEFINED_DEVICE_NAME"] = "yes";
  }
  if (new_config.sys_descr_ != actual_config.sys_descr_) {
    user_conf["USE_USER_DEFINED_DESCRIPTION"] = "yes";
  }

  data = SnmpUserConfToString(user_conf);

}

void UpdateSnmpUserConfFile(::std::string &&path, const SnmpConfig &new_config, const SnmpConfig &actual_config) {

  auto data = wago::util::ReadFile(path);
  UpdateSnmpUserConfData(data, new_config, actual_config);
  ::wago::util::WriteFile(path, data);

}

}
