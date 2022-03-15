// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include <map>
#include <string>

namespace netconf {

class HostConfFileParser {
 public:
  HostConfFileParser() = default;
  virtual ~HostConfFileParser() = default;

  HostConfFileParser(const HostConfFileParser&) = default;
  HostConfFileParser& operator=(const HostConfFileParser&) = delete;
  HostConfFileParser(const HostConfFileParser&&) = delete;
  HostConfFileParser& operator=(const HostConfFileParser&&) = delete;

  void Parse(const ::std::string &content);

  ::std::string GetHostname();
  ::std::string GetDomain();

 private:
  ::std::map<::std::string, ::std::string> values_;
};

} /* namespace netconf */

