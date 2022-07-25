//------------------------------------------------------------------------------
// Copyright (c) WAGO GmbH & Co. KG
//
// PROPRIETARY RIGHTS are involved in the subject matter of this material. All
// manufacturing, reproduction, use and sales rights pertaining to this
// subject matter are governed by the license agreement. The recipient of this
// software implicitly accepts the terms of the license.
//------------------------------------------------------------------------------

#pragma once

#include <vector>

#include "ConfigLine.hpp"
#include "SnmpConfig.hpp"

namespace wago::snmp_config_lib {

void ExtractBasicParamters(const std::vector<ConfigLine> &lines, SnmpConfig &snmp_config);
void ExtractTrapV1V2cParameters(const std::vector<ConfigLine> &lines, SnmpConfig &snmp_config);
void ExtractCommunityParameters(const std::vector<ConfigLine> &lines, SnmpConfig &snmp_config);
void ExtractUserParameters(const std::vector<ConfigLine> &lines, SnmpConfig &snmp_config);
void ExtractTrapV3Parameters(const std::vector<ConfigLine> &lines, SnmpConfig &snmp_config);

std::string SnmpBasicAndV1V2cParameterToString(const SnmpConfig &snmp_config);
std::string SnmpV3ParameterToString(const SnmpConfig &snmp_config);


::std::vector<ConfigLine> ExtractConfigLines(const ::std::string &config);

}
