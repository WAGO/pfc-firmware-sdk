// SPDX-License-Identifier: GPL-2.0-or-later

#include "MacDistributor.hpp"
#include <boost/algorithm/string.hpp>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <net/if.h>
#include <net/if_arp.h>
#include <cstring>
#include <unistd.h>
#include <cstdio>
#include <sstream>

#include "gsl.hpp"
#include "alphanum.hpp"
#include "Logger.hpp"

namespace netconfd {

MacDistributor::MacDistributor(IDevicePropertiesProvider& properties_provider)
    : properties_provider_ { properties_provider } {

  auto interfaces = properties_provider.GetOSInterfaces();

  ::std::sort(interfaces.begin(), interfaces.end(), ::doj::alphanum_less<::std::string>());

  for (size_t i = 1; i <= interfaces.size(); i++) {
    ::std::string mac = properties_provider.GetIncrementedMac(i);
    if (mac.empty()) {
      mac = properties_provider_.GetMac();
    }
    Status status = SetMac(mac, interfaces[i - 1]);
    if (status.NotOk()) {

      ::std::string interface = interfaces[i - 1];
      throw ::std::runtime_error(
          "MacDistributor: Failed to set MAC for interface " + interface);
    }
  }

}

Status MacDistributor::SetMac(Bridge const& interface) {
  ::std::string mac = properties_provider_.GetMac();
  return SetMac(mac, interface);
}

static Status MacStringToBinaryMac(const ::std::string& mac, gsl::span<char>& mac_span) {

  Status status;

  const uint32_t ether_addr_len = 6;

  ::std::vector<::std::string> mac_bytes;
  boost::split(mac_bytes, mac, boost::is_any_of(":"));

  if (mac_bytes.size() != ether_addr_len) {
    status.Append(StatusCode::ERROR,
                   "Convert string mac " + mac + " to binary failed (format error).");
  }

  if (status.Ok()) {
    for (uint32_t i = 0; i < ether_addr_len; i++) {
      char trash;
      uint32_t mac_byte;
      std::stringstream ss(mac_bytes.at(i));
      ss >> std::hex >> mac_byte >> trash;
      mac_span.at(i) = static_cast<char>(mac_byte);
    }
  }

  return status;
}

Status MacDistributor::SetMac(const ::std::string& mac, ::std::string const& interface) {

  Status status;

  if (interface.length() >= IFNAMSIZ) {
    status.Prepend(StatusCode::ERROR, "interface name length >= 16 " + interface);
  }

  struct ifreq ifr { };
  if (status.Ok()) {
    ifr.ifr_hwaddr.sa_family = ARPHRD_ETHER;  //NOLINT: do not access members of unions is unavoidable at this point
    strncpy(ifr.ifr_name, interface.c_str(), IFNAMSIZ);  //NOLINT: do not access members of unions is unavoidable at this point

    ::gsl::span<char> mac_buffer{ifr.ifr_hwaddr.sa_data}; //NOLINT: do not access members of unions is unavoidable at this point
    status = MacStringToBinaryMac(mac, mac_buffer);

    if (status.NotOk()) {
      status.Prepend(StatusCode::ERROR, "Try to set mac. ");
    }
  }

  if (status.Ok()) {
    int socket_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (socket_fd != -1) {

      if (ioctl(socket_fd, SIOCSIFHWADDR, &ifr) == -1) {
        status.Prepend(StatusCode::SYSTEM_CALL_ERROR,
                        "system call error SIOCSIFHWADDR failed");
      }
      close(socket_fd);
    }
  }

  if (status.NotOk()) {
    status.Prepend("Failed to set mac of " + interface + ", ");
  }

  return status;
}

} /* namespace netconfd */
