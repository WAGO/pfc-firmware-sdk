//------------------------------------------------------------------------------
// Copyright (c) WAGO GmbH & Co. KG
//
// PROPRIETARY RIGHTS are involved in the subject matter of this material. All
// manufacturing, reproduction, use and sales rights pertaining to this
// subject matter are governed by the license agreement. The recipient of this
// software implicitly accepts the terms of the license.
//------------------------------------------------------------------------------

#pragma once

#include <map>
#include <string>

namespace wago::util {


::std::map<::std::string, ::std::string> ParseKeyValuePairs(const ::std::string& content);

} /* namespace netconf */

