// SPDX-License-Identifier: GPL-2.0-or-later

#include "HostConfFileParser.hpp"

#include <boost/algorithm/string.hpp>
#include <exception>
#include <regex>
#include <sstream>

#include "KeyValueParser.hpp"


namespace netconf {

void HostConfFileParser::Parse(const ::std::string &content) {
  values_ = ParseKeyValuePairs(content);
}

::std::string HostConfFileParser::GetHostname() {
  return values_["HOSTNAME"];
}

::std::string HostConfFileParser::GetDomain() {
  return values_["DNSDOMAIN"];
}

} /* namespace netconf */
