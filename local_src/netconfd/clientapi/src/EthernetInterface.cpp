// SPDX-License-Identifier: GPL-2.0-or-later

#include "EthernetInterface.hpp"

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

using namespace std::literals;

EthernetInterface::EthernetInterface(::std::uint32_t ifindex)
    : mac_ { 0 },
      if_index_ { ifindex },
      ifreq_ { },
      socket_ { AF_INET, SOCK_DGRAM, IPPROTO_IP } {
  InitializeData();
}

EthernetInterface::EthernetInterface(EthernetInterface &&other) noexcept
    : mac_ { 0 },
      name_ { std::move(other.name_) },
      if_index_ { other.if_index_ },
      ifreq_ { other.ifreq_ },
      socket_ { std::move(other.socket_) } {
  std::move(std::begin(other.mac_), std::end(other.mac_), &mac_[0]);
  other.if_index_ = std::numeric_limits < std::uint32_t > ::max();
}

void EthernetInterface::InitializeData() {

  if (ioctl(socket_, SIOCGIFHWADDR, &ifreq_) < 0) {
    throw ::std::system_error(errno, ::std::system_category(), "EthernetInterface: ioctl() SIOCGIFHWADDR failed");
  }
  mac_[0] = static_cast<uint8_t>(ifreq_.ifr_hwaddr.sa_data[0]);  // NOLINT: must access union members in legacy data structures.
  mac_[1] = static_cast<uint8_t>(ifreq_.ifr_hwaddr.sa_data[1]);  // NOLINT: must access union members in legacy data structures.
  mac_[2] = static_cast<uint8_t>(ifreq_.ifr_hwaddr.sa_data[2]);  // NOLINT: must access union members in legacy data structures.
  mac_[3] = static_cast<uint8_t>(ifreq_.ifr_hwaddr.sa_data[3]);  // NOLINT: must access union members in legacy data structures.
  mac_[4] = static_cast<uint8_t>(ifreq_.ifr_hwaddr.sa_data[4]);  // NOLINT: must access union members in legacy data structures.
  mac_[5] = static_cast<uint8_t>(ifreq_.ifr_hwaddr.sa_data[5]);  // NOLINT: must access union members in legacy data structures.
}

EthernetInterface::EthernetInterface(::std::string name)

    : mac_ { 0 },
      name_(std::move(name)),
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
  return MacAddress(mac_);
}

}  // namespace pfcspecific
