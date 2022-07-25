// SPDX-License-Identifier: GPL-2.0-or-later

#include "IPController.hpp"

#include <arpa/inet.h>
#include <linux/if.h>
#include <netinet/in.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <unistd.h>

#include <cerrno>
#include <cstring>
#include <gsl/gsl>
#include <system_error>

#include "Logger.hpp"
#include "Socket.hpp"

namespace netconf {

static Status SetIPParameter(int fd, uint16_t flag, const Interface &interface, const ::std::string &value_str) {
  uint32_t value;
  if (1 != inet_pton(AF_INET, value_str.c_str(), &value)) {
    return Status { StatusCode::IPV4_FORMAT, value_str };
  }

  ifreq if_req = { };
  ::gsl::span<char> if_req_name { if_req.ifr_name };  // NOLINT: do not access members of unions is unavoidable at this point

  strncpy(if_req_name.data(), interface.c_str(), if_req_name.size());
  if_req.ifr_addr.sa_family = AF_INET;  // NOLINT: do not access members of unions is unavoidable at this point

  struct sockaddr_in *sock_value = reinterpret_cast<struct sockaddr_in*>(&if_req.ifr_addr);  // NOLINT: do not access members of unions is unavoidable at this point
  sock_value->sin_addr.s_addr = value;
  if (ioctl(fd, flag, &if_req) < 0) {
    return MAKE_SYSTEMCALL_ERROR("ioctl");
  }

  return Status::Ok();
}

Status IPController::SetIPConfig(const Interface &itf_name, const Address &address, const Netmask &netmask) const {
  Status status(StatusCode::OK);

  try {
    ::Socket fd { PF_INET, SOCK_DGRAM, IPPROTO_IP };

    status = SetIPParameter(fd, SIOCSIFADDR, itf_name, address);
    if (status.IsNotOk()) {
      auto user_status = Status { StatusCode::SET_IP, address, netmask, itf_name };
      LogError(user_status.ToString() + ": " + status.ToString());
      return user_status;
    }

    if (ZeroIP != address) {
      status = SetIPParameter(fd, SIOCSIFNETMASK, itf_name, netmask);
      if (status.IsNotOk()) {
        auto user_status = Status { StatusCode::SET_IP, address, netmask, itf_name };
        LogError(user_status.ToString() + ": " + status.ToString());
        return user_status;
      }
    }

  } catch (std::exception &e) {
    return Status { StatusCode::GENERIC_ERROR, e.what() };
  }

  return status;
}

Status IPController::Configure(const Interface &itf_name, const Address &address, const Netmask &netmask) const {
  return SetIPConfig(itf_name, address, netmask);
}

Status IPController::Configure(const IPConfig &config) const {
  return SetIPConfig(config.interface_, config.address_, config.netmask_);
}

void IPController::Flush(const Interface &itf_name) const {
  Configure(itf_name, ZeroIP, ZeroIP);
}

}  // namespace netconf
/* namespace netconf */
