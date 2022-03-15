#pragma once

#include <map>
#include <string>

namespace netconf {


::std::map<::std::string, ::std::string> ParseKeyValuePairs(const ::std::string& content);

} /* namespace netconf */

