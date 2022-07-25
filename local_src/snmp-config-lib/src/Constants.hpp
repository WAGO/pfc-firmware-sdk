//------------------------------------------------------------------------------
// Copyright (c) WAGO GmbH & Co. KG
//
// PROPRIETARY RIGHTS are involved in the subject matter of this material. All
// manufacturing, reproduction, use and sales rights pertaining to this
// subject matter are governed by the license agreement. The recipient of this
// software implicitly accepts the terms of the license.
//------------------------------------------------------------------------------

#pragma once

#include <string>

namespace wago::snmp_config_lib {

struct LinuxDaemonFilePaths {
  ::std::string daemon;
  ::std::string init_d;
  ::std::string rc_d;
};

struct Constants {
    static LinuxDaemonFilePaths GetSnmpFilePaths();
    static LinuxDaemonFilePaths GetSnmpTrapFilePaths();
    static ::std::string GetSnmpConfPath();
    static ::std::string GetSnmpV3ConfPath();
    static ::std::string GetSnmpUserConfPath();

};



}
