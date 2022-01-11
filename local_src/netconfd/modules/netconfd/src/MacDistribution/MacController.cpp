// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * MacController.cpp
 *
 *  Created on: 19.05.2020
 *      Author: u016903
 */

#include "MacController.hpp"

#include <boost/algorithm/string.hpp>
#include <boost/format.hpp>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <net/if.h>
#include <net/if_arp.h>
#include <Status.hpp>
#include <cstring>
#include <unistd.h>
#include <cstdio>
#include <sstream>

#include "alphanum.hpp"
#include "Logger.hpp"

namespace netconf {

using namespace ::std::string_literals;

void MacController::SetMac(const MacAddress &mac, ::std::string const &interface) {

  Status status;

  if (interface.length() >= IFNAMSIZ) {
    status.Set(StatusCode::GENERIC_ERROR, "Interface name length >= 16 "s + interface);
  }

  struct ifreq ifr { };
  if (status.IsOk()) {
    ifr.ifr_hwaddr.sa_family = ARPHRD_ETHER;  //NOLINT: do not access members of unions is unavoidable at this point
    strncpy(ifr.ifr_name, interface.c_str(), IFNAMSIZ);  //NOLINT: do not access members of unions is unavoidable at this point
    memcpy(ifr.ifr_hwaddr.sa_data, mac.data(), MacAddress::LENGTH);
  }

  if (status.IsOk()) {
    int socket_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (socket_fd != -1) {
      if (ioctl(socket_fd, SIOCSIFHWADDR, &ifr) == -1) {
        status = MakeSystemCallError();
      }
      close(socket_fd);
    }
  }

  if (status.IsNotOk()) {
    auto message = boost::format("MacDistributor: Failed to set mac address for interface %s: %s")
      % interface % status.ToString();
    LogWarning(message.str());
  }
}

}
