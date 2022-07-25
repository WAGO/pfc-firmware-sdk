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
#include <vector>

namespace wago::snmp_config_lib
{
  /**
   * represents a line in snmpd.conf
   * e.g. rwcommunity public localhost
   *      |           `--> value(s)
   *      `---------------> parameter
   */
  struct ConfigLine {
    ::std::string parameter_;
    ::std::string values_;
  };

}
