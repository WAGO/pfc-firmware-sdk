/*
 * MacController.cpp
 *
 *  Created on: 19.05.2020
 *      Author: u016903
 */

#include "MacController.hpp"

#include <boost/algorithm/string.hpp>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <net/if.h>
#include <net/if_arp.h>
#include <cstring>
#include <unistd.h>
#include <cstdio>
#include <sstream>

#include "alphanum.hpp"
#include "Logger.hpp"
#include "Status.hpp"

namespace netconf {

void MacController::SetMac(const MacAddress &mac, ::std::string const &interface) {

  Status status;

  if (interface.length() >= IFNAMSIZ) {
    status.Prepend(StatusCode::ERROR, "Interface name length >= 16 " + interface);
  }

  struct ifreq ifr { };
  if (status.Ok()) {
    ifr.ifr_hwaddr.sa_family = ARPHRD_ETHER;  //NOLINT: do not access members of unions is unavoidable at this point
    strncpy(ifr.ifr_name, interface.c_str(), IFNAMSIZ);  //NOLINT: do not access members of unions is unavoidable at this point

    memcpy(ifr.ifr_hwaddr.sa_data, mac.data(), MacAddress::LENGTH);

    if (status.NotOk()) {
      status.Prepend(StatusCode::ERROR, "Try to set mac. ");
    }
  }

  if (status.Ok()) {
    int socket_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (socket_fd != -1) {

      if (ioctl(socket_fd, SIOCSIFHWADDR, &ifr) == -1) {
        status.Prepend(StatusCode::SYSTEM_CALL_ERROR, "System call error SIOCSIFHWADDR failed");
      }
      close(socket_fd);
    }
  }

  if (status.NotOk()) {
    throw ::std::runtime_error("MacDistributor: Failed to set MAC for interface " + interface + ". ");
  }

}

}
