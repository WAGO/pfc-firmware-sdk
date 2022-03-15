// SPDX-License-Identifier: LGPL-3.0-or-later

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
#include <gsl/gsl>


extern "C" char *if_indextoname(unsigned int __ifindex, char *__ifname);

namespace netconf {

using eth::DeviceState;
using eth::Autoneg;
using eth::MediaType;
using eth::InterfaceLinkState;
using gsl::span;
using namespace std::literals;

EthernetInterface::EthernetInterface(::std::string name)

    : mac_ { 0 },
      name_(std::move(name)),
      ifreq_ { },
      socket_ { AF_INET, SOCK_DGRAM, IPPROTO_IP },
      mtu_ { },
      ethtool_{socket_, name_} ,
      ethtool_settings_r_{ethtool_.Create()},
      ethtool_settings_w_{ethtool_.Create()}
      {

  strncpy(ifreq_.ifr_name, name_.c_str(), name_.size());  // NOLINT: must access union members in legacy data structures.

  if (ioctl(socket_, SIOCGIFINDEX, &ifreq_) < 0) {
    throw ::std::system_error(errno, ::std::system_category(),
                              "EthernetInterface "s + name_ + " : ioctl() SIOCGIFINDEX failed"s);
  }
  if_index_ = static_cast<uint32_t>(ifreq_.ifr_ifindex);  // NOLINT: must access union members in legacy data structures.
  InitializeData();
}

void EthernetInterface::InitializeData() {

  if (ioctl(socket_, SIOCGIFMTU, &ifreq_) < 0) {
    throw ::std::system_error(errno, ::std::system_category(), "EthernetInterface: ioctl() SIOCGIFHWADDR failed");
  }
  mtu_ = static_cast<std::size_t>(ifreq_.ifr_mtu);  // NOLINT: must access union members in legacy data structures.
  if (ioctl(socket_, SIOCGIFHWADDR, &ifreq_) < 0) {
    throw ::std::system_error(errno, ::std::system_category(), "EthernetInterface: ioctl() SIOCGIFHWADDR failed");
  }
  mac_[0] = static_cast<uint8_t>(ifreq_.ifr_hwaddr.sa_data[0]);  // NOLINT: must access union members in legacy data structures.
  mac_[1] = static_cast<uint8_t>(ifreq_.ifr_hwaddr.sa_data[1]);  // NOLINT: must access union members in legacy data structures.
  mac_[2] = static_cast<uint8_t>(ifreq_.ifr_hwaddr.sa_data[2]);  // NOLINT: must access union members in legacy data structures.
  mac_[3] = static_cast<uint8_t>(ifreq_.ifr_hwaddr.sa_data[3]);  // NOLINT: must access union members in legacy data structures.
  mac_[4] = static_cast<uint8_t>(ifreq_.ifr_hwaddr.sa_data[4]);  // NOLINT: must access union members in legacy data structures.
  mac_[5] = static_cast<uint8_t>(ifreq_.ifr_hwaddr.sa_data[5]);  // NOLINT: must access union members in legacy data structures.

  ethtool_.Update(ethtool_settings_r_);
  ethtool_settings_w_ = ethtool_settings_r_;
}

void EthernetInterface::UpdateConfig() {
  ethtool_.Update(ethtool_settings_r_);
}

::std::uint32_t EthernetInterface::GetSpeed() const {
  return ethtool_settings_r_.GetSpeed();
}

eth::Duplex EthernetInterface::GetDuplex() const {
  return ethtool_settings_r_.GetDuplex();
}

MacAddress EthernetInterface::GetMac() const {
  return MacAddress(::gsl::make_span(mac_));
}

bool EthernetInterface::GetAutonegSupport() const {
  return TestLinkModesBits(ethtool_settings_r_.GetSupported(), ETHTOOL_LINK_MODE_Autoneg_BIT);
}

bool EthernetInterface::GetAutonegEnabled() const {
  return ethtool_settings_r_.IsAutonegEnabled();
}

MediaType EthernetInterface::GetMediaType() const {
  return ethtool_settings_r_.GetMediaType();
}

gsl::span<const uint32_t> EthernetInterface::GetSupportedLinkModes() const {
  return ethtool_settings_r_.GetSupported();
}

[[gnu::pure]]
::std::uint32_t EthernetInterface::GetMTU() const {
  return mtu_;
}

void EthernetInterface::SetIfFlags(int16_t if_flags) {
  ifreq_.ifr_flags = if_flags;  // NOLINT: must access union members in legacy data structures.
  if (ioctl(socket_, SIOCSIFFLAGS, &ifreq_) < 0) {
    throw ::std::system_error(errno, ::std::system_category());
  }
}

int16_t EthernetInterface::GetIfFlags() const {
  if (ioctl(socket_, SIOCGIFFLAGS, &ifreq_) < 0) {
    throw ::std::system_error(errno, ::std::system_category(), "EthernetInterface: ioctl() SIOCGIFINDEX failed");
  }
  return ifreq_.ifr_flags;  // NOLINT: must access union members in legacy data structures.
}


DeviceState EthernetInterface::GetState() const {
  return ethtool_settings_r_.GetIfState();
}

InterfaceLinkState EthernetInterface::GetLinkState() const {
  return ethtool_settings_r_.GetIfLinkState();
}

void EthernetInterface::Commit() {
  ethtool_.Commit(ethtool_settings_w_);
}

void EthernetInterface::SetAutoneg(eth::Autoneg autoneg) {
  ethtool_settings_w_.SetAutonegEnabled(autoneg == eth::Autoneg::On);
}

void EthernetInterface::SetSpeed(::std::int32_t speed) {
  if (speed > 0) {
    ethtool_settings_w_.SetSpeed(speed);
  }
}

void EthernetInterface::SetDuplex(eth::Duplex duplex) {
  ethtool_settings_w_.SetDuplex(duplex);
}

void EthernetInterface::SetState(DeviceState state) {
  ethtool_settings_w_.SetIfState(state);
}

[[gnu::pure]]
::std::uint32_t EthernetInterface::GetInterfaceIndex() const {
  return if_index_;
}

::std::string EthernetInterface::IndexToName(::std::uint32_t ifindex) {
  char nameBuf[IFNAMSIZ + 1] = { 0 };
  if_indextoname(ifindex, static_cast<char*>(nameBuf));
  return ::std::string { static_cast<char*>(nameBuf) };
}

}  // namespace pfcspecific
