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

namespace wago::util {

void WriteFile(const ::std::string &path, const ::std::string &data);
::std::string ReadFile(const ::std::string &path);

}  // namespace wago::util
