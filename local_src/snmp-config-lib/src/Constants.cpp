//------------------------------------------------------------------------------
// Copyright (c) WAGO GmbH & Co. KG
//
// PROPRIETARY RIGHTS are involved in the subject matter of this material. All
// manufacturing, reproduction, use and sales rights pertaining to this
// subject matter are governed by the license agreement. The recipient of this
// software implicitly accepts the terms of the license.
//------------------------------------------------------------------------------

#include "Constants.hpp"

namespace wago::snmp_config_lib {

LinuxDaemonFilePaths Constants::GetSnmpFilePaths() {
  return LinuxDaemonFilePaths { "/usr/sbin/snmpd", "/etc/init.d/net-snmpd", "/etc/rc.d/S21_netsnmpd" };
}

LinuxDaemonFilePaths Constants::GetSnmpTrapFilePaths() {
  return LinuxDaemonFilePaths { "/usr/sbin/snmptrapd", "/etc/init.d/net-snmptrapd", "/etc/rc.d/S22_netsnmptrapd" };
}

::std::string Constants::GetSnmpConfPath() {
  return "/etc/snmp/snmpd.conf";
}

::std::string Constants::GetSnmpV3ConfPath() {
  return "/var/lib/net-snmp/snmpd.conf";
}

::std::string Constants::GetSnmpUserConfPath() {
  return "/etc/specific/snmp_user.conf";
}


}
