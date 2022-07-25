#pragma once

#include <map>
#include <string>

namespace netconf {


::std::map<::std::string, ::std::string> ParseKeyValuePairs(const ::std::string& content);
::std::string GetValueByKey(const ::std::string &data, const ::std::string &key);



} /* namespace netconf */

