// SPDX-License-Identifier: GPL-2.0-or-later

#include "MacController.hpp"

#include <net/if.h>
#include <net/if_arp.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <unistd.h>

#include <boost/algorithm/string.hpp>
#include <boost/format.hpp>
#include <cstdio>
#include <cstring>
#include <sstream>

#include "Logger.hpp"
#include "Status.hpp"
#include "alphanum.hpp"

namespace netconf {

using namespace ::std::string_literals;

void MacController::SetMac(const MacAddress &mac, ::std::string const &interface) {
  Status status;

  if (interface.length() >= IFNAMSIZ) {
    status.Set(StatusCode::GENERIC_ERROR, "Interface name length >= 16 "s + interface);
  }

  struct ifreq ifr { };
  if (status.IsOk()) {
    ifr.ifr_hwaddr.sa_family = ARPHRD_ETHER;  // NOLINT: do not access members of unions is unavoidable at this point
    strncpy(ifr.ifr_name, interface.c_str(),  // NOLINT: do not access members of unions is unavoidable at this point
            IFNAMSIZ);                        // NOLINT: do not access members of unions is unavoidable at this point
    memcpy(ifr.ifr_hwaddr.sa_data, mac.data(), MacAddress::LENGTH);  // NOLINT: do not access members of unions is unavoidable at this point
  }

  if (status.IsOk()) {
    int socket_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (socket_fd != -1) {
      if (ioctl(socket_fd, SIOCSIFHWADDR, &ifr) == -1) {
        status = MakeSystemCallError("ioctl");
      }
      close(socket_fd);
    }
  }

  if (status.IsNotOk()) {
    auto message = boost::format("MacDistributor: Failed to set mac address for interface %s: %s") % interface
        % status.ToString();
    LogWarning(message.str());
  }
}

}  // namespace netconf
