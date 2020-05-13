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
#include "gsl.hpp"
#include <Socket.h>

namespace netconfd {

static Status GetIPParamter(int fd, uint16_t flag, const Interface &interface, ::std::string &value_str) {

  Status status(StatusCode::OK);

  ifreq ifr = { };
  strncpy(ifr.ifr_name, interface.c_str(), IFNAMSIZ);  //NOLINT: do not implicitly decay an array into a pointer is unavoidable at this point
  ifr.ifr_addr.sa_family = AF_INET;  //NOLINT: do not access members of unions is unavoidable at this point

  if (ioctl(fd, flag, &ifr) < 0) {
    if (EADDRNOTAVAIL == errno) {
      value_str = ZeroIP;
    } else {
      status.Prepend(StatusCode::SYSTEM_CALL_ERROR, "system call error: errno " + ::std::to_string(errno));
    }
    return status;
  }

  sockaddr_in *sock_value = reinterpret_cast<struct sockaddr_in*>(&ifr.ifr_addr);  //NOLINT: reinterpret_cast is unavoidable at this point
  uint32_t value = sock_value->sin_addr.s_addr;

  char inet_buffer[INET_ADDRSTRLEN];
  gsl::span<char> inet = { inet_buffer };
  if (nullptr == inet_ntop(AF_INET, &value, inet.data(), inet.size())) {
    status.Prepend(
        StatusCode::INVALID_PARAMETER,
        "convert error, could not convert ip parameter " + std::to_string(value)
            + " from binary to presentation format.");
    return status;
  }

  value_str = ::std::string(inet.data());

  return status;

}

static Status SetIPParameter(int fd, uint16_t flag, const Interface &interface, const ::std::string &value_str) {

  Status status(StatusCode::OK);

  uint32_t value;
  if (1 != inet_pton(AF_INET, value_str.c_str(), &value)) {
    status.Prepend(
        StatusCode::INVALID_PARAMETER,
        "convert error, could not convert ip parameter " + value_str + " from presentation to binary format.");
    return status;
  }

  ifreq if_req = { };
  ::gsl::span<char> if_req_name { if_req.ifr_name };  //NOLINT: do not access members of unions is unavoidable at this point

  strncpy(if_req_name.data(), interface.c_str(), if_req_name.size());
  if_req.ifr_addr.sa_family = AF_INET;    //NOLINT: do not access members of unions is unavoidable at this point

  struct sockaddr_in *sock_value = reinterpret_cast<struct sockaddr_in*>(&if_req.ifr_addr);  //NOLINT: do not access members of unions is unavoidable at this point
  sock_value->sin_addr.s_addr = value;
  if (ioctl(fd, flag, &if_req) < 0) {
    status.Prepend(StatusCode::SYSTEM_CALL_ERROR, "system call error.");
    return status;
  }

  return status;
}

Status IPController::GetIPConfig(const Interface &interface, IPConfig &config) const {

  Status status(StatusCode::OK);

  config.Clear();

  try {
    ::Socket sockfd { PF_INET, SOCK_DGRAM, IPPROTO_IP };

    ::std::string ip_value;
    status = GetIPParamter(sockfd, SIOCGIFADDR, interface, ip_value);
    if (status.NotOk()) {
      status.Prepend("Get IP address of interface " + interface + ": ");
      return status;
    }

    ::std::string netmask_value;
    status = GetIPParamter(sockfd, SIOCGIFNETMASK, interface, netmask_value);
    if (status.NotOk()) {
      status.Prepend("Get netmask of interface " + interface + ": ");
      return status;
    }

    ::std::string broadcast_value;
    status = GetIPParamter(sockfd, SIOCGIFBRDADDR, interface, broadcast_value);
    if (status.NotOk()) {
      status.Prepend("Get broadcast of interface " + interface + ": ");
      return status;
    }

    config.interface_ = interface;
    config.address_ = ip_value;
    config.netmask_ = netmask_value;
    config.broadcast_ = broadcast_value;

  } catch (std::exception &e) {
    status.Prepend(StatusCode::ERROR, "Get ip parameter of interface " + interface + ": " + e.what());
    config.Clear();
  }
  return status;
}

Status IPController::SetIPConfig(const IPConfig &config) const {

  Status status(StatusCode::OK);

  try {

    ::Socket fd { PF_INET, SOCK_DGRAM, IPPROTO_IP };

    status = SetIPParameter(fd, SIOCSIFADDR, config.interface_, config.address_);
    if (status.NotOk()) {
      status.Prepend("Set IP address of interface " + config.interface_ + ": ");
      return status;
    }

    if (ZeroIP != config.address_) {
      status = SetIPParameter(fd, SIOCSIFNETMASK, config.interface_, config.netmask_);
      if (status.NotOk()) {
        status.Prepend("Set IP netmask of interface " + config.interface_ + ": ");
        return status;
      }
    }

    if (not config.broadcast_.empty() && ZeroIP != config.broadcast_) {
      status = SetIPParameter(fd, SIOCSIFBRDADDR, config.interface_, config.broadcast_);
      if (status.NotOk()) {
        status.Prepend("Set IP broadcast address of interface " + config.interface_ + ": ");
        return status;
      }
    }

  } catch (std::exception &e) {
    status.Prepend(StatusCode::ERROR, "Set ip parameter of interface " + config.interface_ + ": " + e.what());
  }

  return status;
}

}
/* namespace netconfd */
