//------------------------------------------------------------------------------
// Copyright (c) WAGO GmbH & Co. KG
//
// PROPRIETARY RIGHTS are involved in the subject matter of this material. All
// manufacturing, reproduction, use and sales rights pertaining to this
// subject matter are governed by the license agreement. The recipient of this
// software implicitly accepts the terms of the license.
//------------------------------------------------------------------------------

#pragma once

#include "SnmpConfig.hpp"
#include <string>


namespace wago::snmp_config_lib {

void UpdateSnmpUserConfFile(::std::string &&path, const SnmpConfig &new_config, const SnmpConfig &actual_config);

// function is only published in the header for test purposes
void UpdateSnmpUserConfData(::std::string &data, const SnmpConfig &new_config, const SnmpConfig &actual_config);

}
