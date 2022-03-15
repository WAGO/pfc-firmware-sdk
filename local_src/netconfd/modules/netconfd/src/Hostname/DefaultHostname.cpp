// SPDX-License-Identifier: GPL-2.0-or-later

#include "DefaultHostname.hpp"

#include "Logger.hpp"
#include "FileEditor.hpp"
#include "StringUtils.hpp"

namespace netconf {


::std::string GetDefaultHostname(const ::std::string& board_variant, const ::std::string& base_mac) {

  ::std::string hostname{};
  try {

    ::std::string last_three_mac_octets = base_mac;
    last_three_mac_octets.erase(0, 8);

    hostname = board_variant + "-" + last_three_mac_octets;

    RemoveCharFromString(hostname, '\n');
    RemoveCharFromString(hostname, ':');

    StringToUpper(hostname);

  } catch (...) {
    LogError("Could not determine the default hostname.");
  }

  return hostname;
}

::std::string GetDefaultHostname() {

  FileEditor reader;

  ::std::string board_variant;
  reader.Read("/sys/class/wago/system/board_variant", board_variant);
  ::std::string base_mac;
  reader.Read("/sys/class/net/eth0/address", base_mac);

  return GetDefaultHostname(board_variant, base_mac);
}

} /* namespace netconf */
