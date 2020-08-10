// SPDX-License-Identifier: GPL-2.0-or-later

#include "EthernetInterface.hpp"

#include <gsl/gsl>
#include <utility>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <linux/sockios.h>
#include <linux/rtnetlink.h>
#include <linux/if_arp.h>
#include <ifaddrs.h>
#include <linux/ethtool.h>
#include <cstring>

extern "C" char* if_indextoname(unsigned int __ifindex, char *__ifname);

namespace netconf {
namespace api {

using namespace std::literals;

EthernetInterface::EthernetInterface(::std::uint32_t ifindex)
    : if_index_ { ifindex },
      ifreq_ { },
      socket_ { AF_INET, SOCK_DGRAM, IPPROTO_IP } {
  InitializeData();
}

EthernetInterface::EthernetInterface(EthernetInterface &&other) noexcept
    : mac_{std::move(other.mac_)},
        name_ { std::move(other.name_) },
      if_index_ { other.if_index_ },
      ifreq_ { other.ifreq_ },
      socket_ { std::move(other.socket_) } {
  other.if_index_ = std::numeric_limits < std::uint32_t > ::max();
}

void EthernetInterface::InitializeData() {

  if (ioctl(socket_, SIOCGIFHWADDR, &ifreq_) < 0) {
    throw ::std::system_error(errno, ::std::system_category(), "EthernetInterface: ioctl() SIOCGIFHWADDR failed");
  }
  mac_ = MacAddress{ifreq_.ifr_hwaddr.sa_data};
}

EthernetInterface::EthernetInterface(::std::string name)

    : name_(std::move(name)),
      ifreq_ { },
      socket_ { AF_INET, SOCK_DGRAM, IPPROTO_IP } {

  strncpy(ifreq_.ifr_name, name_.c_str(), name_.size());  // NOLINT: must access union members in legacy data structures.

  if (ioctl(socket_, SIOCGIFINDEX, &ifreq_) < 0) {
    throw ::std::system_error(errno, ::std::system_category(),
                              "EthernetInterface "s + name_ + " : ioctl() SIOCGIFINDEX failed"s);
  }
  if_index_ = static_cast<uint32_t>(ifreq_.ifr_ifindex);  // NOLINT: must access union members in legacy data structures.
  InitializeData();
}

const MacAddress EthernetInterface::GetMac() const {
  return mac_;
}

}  // namespace api
}  // namespace pfcspecific
