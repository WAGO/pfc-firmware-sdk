// SPDX-License-Identifier: GPL-2.0-or-later

#include "LeaseFileParser.hpp"

#include <boost/algorithm/string.hpp>
#include <exception>
#include <regex>
#include <sstream>

#include "KeyValueParser.hpp"


namespace netconf {

void LeaseFileParser::Parse(const ::std::string &content) {
  values_ = ParseKeyValuePairs(content);
}

Address LeaseFileParser::GetAddress() {
  return values_["IPADDRESS"];
}

Netmask LeaseFileParser::GetNetmask() {
  return values_["NETMASK"];
}

::std::string LeaseFileParser::GetDHCPHostname() {
  return values_["DHCPHOSTNAME"];
}

::std::string LeaseFileParser::GetDHCPDomain() {
  return values_["DHCPDOMAIN"];
}

} /* namespace netconf */
