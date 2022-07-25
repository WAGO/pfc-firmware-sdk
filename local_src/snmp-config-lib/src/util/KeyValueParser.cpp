//------------------------------------------------------------------------------
// Copyright (c) WAGO GmbH & Co. KG
//
// PROPRIETARY RIGHTS are involved in the subject matter of this material. All
// manufacturing, reproduction, use and sales rights pertaining to this
// subject matter are governed by the license agreement. The recipient of this
// software implicitly accepts the terms of the license.
//------------------------------------------------------------------------------

#include "KeyValueParser.hpp"

#include <boost/algorithm/string.hpp>
#include <exception>
#include <regex>
#include <sstream>

namespace wago::util {

::std::map<::std::string, ::std::string> ParseKeyValuePairs(const ::std::string& content) {
  std::regex reList{R"regex(([A-Z0-9_]+)=([a-zA-Z0-9.-]+))regex", std::regex::extended};

  ::std::map<::std::string, ::std::string> values;
   std::for_each(std::sregex_iterator(content.begin(), content.end(), reList), std::sregex_iterator(),
                 [&](std::smatch const &match) {
                   ::std::string key   = match[1];
                   ::std::string value = match[2];

                   values[key] = value;
                 });

   return values;

}
}
