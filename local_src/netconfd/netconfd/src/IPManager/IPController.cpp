// SPDX-License-Identifier: GPL-2.0-or-later

#include "IPController.hpp"

#include <system_error>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <linux/if.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>
#include <netinet/in.h>
#include <cerrno>
#include "Logger.hpp"
#include <gsl/gsl>
#include <Socket.h>
#include <errno.h>

namespace netconf {



static Error SetIPParameter(int fd, uint16_t flag, const Interface &interface, const ::std::string &value_str) {

  uint32_t value;
  if (1 != inet_pton(AF_INET, value_str.c_str(), &value)) {
    return Error{ErrorCode::IPV4_FORMAT, value_str};
  }

  ifreq if_req = { };
  ::gsl::span<char> if_req_name { if_req.ifr_name };  //NOLINT: do not access members of unions is unavoidable at this point

  strncpy(if_req_name.data(), interface.c_str(), if_req_name.size());
  if_req.ifr_addr.sa_family = AF_INET;    //NOLINT: do not access members of unions is unavoidable at this point

  struct sockaddr_in *sock_value = reinterpret_cast<struct sockaddr_in*>(&if_req.ifr_addr);  //NOLINT: do not access members of unions is unavoidable at this point
  sock_value->sin_addr.s_addr = value;
  if (ioctl(fd, flag, &if_req) < 0) {

    return MakeSystemCallError();
  }

  return Error::Ok();
}

Error IPController::SetIPConfig(const IPConfig &config) const {

  Error error(ErrorCode::OK);

  try {

    ::Socket fd { PF_INET, SOCK_DGRAM, IPPROTO_IP };

    error = SetIPParameter(fd, SIOCSIFADDR, config.interface_, config.address_);
    if (error.IsNotOk()) {
      auto user_error = Error{ErrorCode::SET_IP, config.address_, config.netmask_, config.interface_};
      LogError(user_error.ToString() + ": " + error.ToString());
      return user_error;
    }

    if (ZeroIP != config.address_) {
      error = SetIPParameter(fd, SIOCSIFNETMASK, config.interface_, config.netmask_);
      if (error.IsNotOk()) {
        auto user_error = Error{ErrorCode::SET_IP, config.address_, config.netmask_, config.interface_};
        LogError(user_error.ToString() + ": " +error.ToString());
        return user_error;
      }
    }

  } catch (std::exception &e) {
    return MakeSystemCallError();
  }

  return error;
}

}
/* namespace netconf */
