// SPDX-License-Identifier: GPL-2.0-or-later
//------------------------------------------------------------------------------
///  \file     EthernetInterface.cpp
///
///  \author   u014487 : WAGO Kontakttechnik GmbH & Co. KG
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// include files
//------------------------------------------------------------------------------

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
//------------------------------------------------------------------------------
// function implementation
//------------------------------------------------------------------------------

extern "C" char *if_indextoname(unsigned int __ifindex, char *__ifname);

namespace netconfd {

using eth::DeviceState;
using eth::Duplex;
using eth::Autoneg;
using eth::TpMdiType;
using eth::MediaType;
using eth::EthernetMauType;
using eth::InterfaceLinkState;
using namespace std::literals;

using guard = ::std::lock_guard<::std::mutex>;

EthernetInterface::EthernetInterface(::std::uint32_t ifindex)
    : mac_ { 0 },
      if_index_ { ifindex },
      ifreq_ { },
      socket_ { AF_INET, SOCK_DGRAM, IPPROTO_IP },
      data_get_ { },
      data_set_ { },
      mtu_ { } {
  InitializeData();
}

EthernetInterface::EthernetInterface(EthernetInterface&& other) noexcept
    : mac_ { 0 },
      name_ { std::move(other.name_) },
      if_index_ { other.if_index_ },
      ifreq_ { other.ifreq_ },
      socket_ { std::move(other.socket_) },
      data_get_ { other.data_get_ },
      data_set_ { other.data_set_ },
      mtu_ { other.mtu_ } {
  std::move(std::begin(other.mac_), std::end(other.mac_), &mac_[0]);
  other.if_index_ = std::numeric_limits<std::uint32_t>::max();
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
  EthernetInterface::UpdateConfig();
  data_set_ = data_get_;
}

EthernetInterface::EthernetInterface(::std::string name)

    : mac_ { 0 },
      name_(std::move(name)),
      ifreq_ { },
      socket_ { AF_INET, SOCK_DGRAM, IPPROTO_IP },
      data_get_ { },
      data_set_ { },
      mtu_ { } {

  strncpy(ifreq_.ifr_name, name_.c_str(), name_.size());  // NOLINT: must access union members in legacy data structures.

  if (ioctl(socket_, SIOCGIFINDEX, &ifreq_) < 0) {
    throw ::std::system_error(errno, ::std::system_category(),
                              "EthernetInterface "s + name_ + " : ioctl() SIOCGIFINDEX failed"s);
  }
  if_index_ = static_cast<uint32_t>(ifreq_.ifr_ifindex);  // NOLINT: must access union members in legacy data structures.
  InitializeData();
}

void EthernetInterface::UpdateConfig() {
  Data new_data { };

  if (ioctl(socket_, SIOCGIFFLAGS, &ifreq_) < 0) {
    throw ::std::system_error(errno, ::std::system_category(), "EthernetInterface: ioctl() SIOCGIFINDEX failed");
  }
  new_data.if_state_ = static_cast<DeviceState>((ifreq_.ifr_flags & IFF_UP) == IFF_UP);  // NOLINT: must access union members in legacy data structures.

  struct ethtool_value eth_value { };
  eth_value.cmd = ETHTOOL_GLINK;
  ifreq_.ifr_data = reinterpret_cast<__caddr_t >(&eth_value);  // NOLINT: need reinterpret_cast for legacy API handling.
  if (ioctl(socket_, SIOCETHTOOL, &ifreq_) < 0) {
    throw ::std::system_error(errno, ::std::system_category(), "EthernetInterface: ioctl() ETHTOOL_GLINK failed");
  }
  new_data.if_link_state_ = static_cast<InterfaceLinkState>(eth_value.data);

  struct ethtool_cmd eth_cmd { };
  eth_cmd.cmd = ETHTOOL_GSET;
  ifreq_.ifr_data = reinterpret_cast<__caddr_t >(&eth_cmd);  // NOLINT: need reinterpret_cast for legacy API handling.
  if (ioctl(socket_, SIOCETHTOOL, &ifreq_) < 0) {
    throw ::std::system_error(errno, ::std::system_category(), "EthernetInterface: ioctl() ETHTOOL_GSET failed");
  }
  new_data.autoneg_enabled_ = eth_cmd.autoneg == AUTONEG_ENABLE;
  new_data.autoneg_supported_ = eth_cmd.supported;
  new_data.autoneg_advertising_ = eth_cmd.advertising;
  new_data.media_type_ = static_cast<MediaType>(eth_cmd.port);
  new_data.duplex_ = eth_cmd.duplex;
  new_data.speed_ = eth_cmd.speed;
  new_data.speed_ += static_cast<::std::uint32_t>(eth_cmd.speed_hi) << 16;
  new_data.mdix_state_ = static_cast<TpMdiType>(eth_cmd.eth_tp_mdix);

  {
    guard with(data_mutex_);
    data_get_ = new_data;
  }

}

EthernetMauType EthernetInterface::GetMauType() const {
  guard with(data_mutex_);
  auto mautype = EthernetMauType::TYPE_NONE;

  if (data_get_.speed_ == SPEED_10
      && (data_get_.media_type_ == MediaType::TP || data_get_.media_type_ == MediaType::AUI
          || data_get_.media_type_ == MediaType::MII) && data_get_.duplex_ == DUPLEX_UNKNOWN) {
    mautype = EthernetMauType::TYPE_10BASE_T;
  } else if (data_get_.speed_ == SPEED_10
      && (data_get_.media_type_ == MediaType::TP || data_get_.media_type_ == MediaType::AUI
          || data_get_.media_type_ == MediaType::MII) && data_get_.duplex_ == DUPLEX_HALF) {
    mautype = EthernetMauType::TYPE_10BASE_TXHD;
  } else if (data_get_.speed_ == SPEED_10
      && (data_get_.media_type_ == MediaType::TP || data_get_.media_type_ == MediaType::AUI
          || data_get_.media_type_ == MediaType::MII) && data_get_.duplex_ == DUPLEX_FULL) {
    mautype = EthernetMauType::TYPE_10BASE_TXFD;
  } else if (data_get_.speed_ == SPEED_10 && (data_get_.media_type_ == MediaType::FIBRE)
      && data_get_.duplex_ == DUPLEX_HALF) {
    mautype = EthernetMauType::TYPE_10BASE_FLHD;
  } else if (data_get_.speed_ == SPEED_10 && (data_get_.media_type_ == MediaType::FIBRE)
      && data_get_.duplex_ == DUPLEX_FULL) {
    mautype = EthernetMauType::TYPE_10BASE_FLFD;
  } else if (data_get_.speed_ == SPEED_100
      && (data_get_.media_type_ == MediaType::TP || data_get_.media_type_ == MediaType::MII)
      && data_get_.duplex_ == DUPLEX_HALF) {
    mautype = EthernetMauType::TYPE_100BASE_TXHD;
  } else if (data_get_.speed_ == SPEED_100
      && (data_get_.media_type_ == MediaType::TP || data_get_.media_type_ == MediaType::MII)
      && data_get_.duplex_ == DUPLEX_FULL) {
    mautype = EthernetMauType::TYPE_100BASE_TXFD;
  } else if (data_get_.speed_ == SPEED_100
      && data_get_.media_type_ == MediaType::FIBRE&& data_get_.duplex_ == DUPLEX_HALF) {
    mautype = EthernetMauType::TYPE_100BASE_FXHD;
  } else if (data_get_.speed_ == SPEED_100
      && data_get_.media_type_ == MediaType::FIBRE&& data_get_.duplex_ == DUPLEX_FULL) {
    mautype = EthernetMauType::TYPE_100BASE_FXFD;
  } else if (data_get_.speed_ == SPEED_1000 && data_get_.duplex_ == DUPLEX_HALF) {
    mautype = EthernetMauType::TYPE_1000BASE_XHD;
  } else if (data_get_.speed_ == SPEED_1000 && data_get_.duplex_ == DUPLEX_FULL) {
    mautype = EthernetMauType::TYPE_1000BASE_XFD;
  }

  return mautype;
}

const MacAddress EthernetInterface::GetMac() const {
  return MacAddress(mac_);
}

bool EthernetInterface::GetAutonegSupport() const {
  guard with(data_mutex_);
  return (data_get_.autoneg_supported_ & SUPPORTED_Autoneg) == SUPPORTED_Autoneg;
}

bool EthernetInterface::GetAutonegEnabled() const {
  guard with(data_mutex_);
  return data_get_.autoneg_enabled_;
}

::std::uint32_t EthernetInterface::GetAutonegCapabilities() const {
  guard with(data_mutex_);
  return data_get_.autoneg_advertising_;
}

MediaType EthernetInterface::GetMediaType() const {
  guard with(data_mutex_);
  return data_get_.media_type_;
}

[[gnu::pure]]
::std::uint32_t EthernetInterface::GetMTU() const {
  return mtu_;
}

constexpr void SetBit(uint16_t *value, uint16_t bit) {
  uint16_t bit_position = 0x0001U;

  bit_position = static_cast<uint16_t>(bit_position << bit);
  *value = (*value | bit_position);
}

constexpr bool IsSet(::std::uint32_t value, ::std::uint32_t flag) {
  return (value & flag) == flag;
}

constexpr auto GetAutonegAdvertisedCapability(::std::uint32_t advertised) {
  uint16_t advertised_capability = 0x0000U;

  if (IsSet(advertised, ADVERTISED_Autoneg)) {
    if (IsSet(advertised, ADVERTISED_10baseT_Half)) {
      SetBit(&advertised_capability, 14);
    }

    if (IsSet(advertised, ADVERTISED_10baseT_Full)) {
      SetBit(&advertised_capability, 13);
    }

    // 100BASE-T4 (Bit 12) Not Defined by ethtool

    if (IsSet(advertised, ADVERTISED_100baseT_Half)) {
      SetBit(&advertised_capability, 11);
    }

    if (IsSet(advertised, ADVERTISED_100baseT_Full)) {
      SetBit(&advertised_capability, 10);
    }

    // 100BASE-T2 Half-Duplex (Bit 9) Not Defined by ethtool

    // 100BASE-T2 Full-Duplex (Bit 8) Not Defined by ethtool

    if (IsSet(advertised, ADVERTISED_Pause) || IsSet(advertised, ADVERTISED_Asym_Pause)) {
      SetBit(&advertised_capability, 7);
    }

    if (IsSet(advertised, ADVERTISED_Asym_Pause)) {
      SetBit(&advertised_capability, 6);
    }

    if (IsSet(advertised, ADVERTISED_Pause)) {
      SetBit(&advertised_capability, 5);
    }

    if (IsSet(advertised, ADVERTISED_Pause) && IsSet(advertised, ADVERTISED_Asym_Pause)) {
      SetBit(&advertised_capability, 4);
    }

    // 100BASE-X Half-Duplex (Bit 3) Not Defined by ethtool

    // 1000BASE-X Full-Duplex (Bit 2) Not Defined by ethtool

    if (IsSet(advertised, ADVERTISED_1000baseT_Half)) {
      SetBit(&advertised_capability, 1);
    }

    if (IsSet(advertised, ADVERTISED_1000baseT_Full)) {
      SetBit(&advertised_capability, 0);
    }
  }

  return advertised_capability;
}

[[gnu::pure]]
::std::uint32_t EthernetInterface::GetAutonegCapabilitiesXdot3() const {
  return GetAutonegAdvertisedCapability(data_get_.autoneg_advertising_);
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
  guard with(data_mutex_);
  return data_get_.if_state_;
}

InterfaceLinkState EthernetInterface::GetLinkState() const {
  guard with(data_mutex_);
  return data_get_.if_link_state_;
}

void EthernetInterface::Commit() {
  struct ethtool_cmd eth_cmd { };

  int16_t if_flags = GetIfFlags();
  //
  // Get the current status
  //
  eth_cmd.cmd = ETHTOOL_GSET;
  ifreq_.ifr_data = reinterpret_cast<__caddr_t >(&eth_cmd);  // NOLINT: need reinterpret_cast for legacy API handling.
  if (ioctl(socket_, SIOCETHTOOL, &ifreq_) < 0) {
    throw ::std::system_error(errno, ::std::system_category(), "EthernetInterface: ioctl() ETHTOOL_GSET failed");
  }

  bool const port_powercycle_needed = static_cast<bool>(data_set_.if_state_)
      && (static_cast<bool>(eth_cmd.autoneg) != data_set_.autoneg_enabled_) && ((if_flags & IFF_UP) == IFF_UP);

  //
  // Apply new settings.
  //

  eth_cmd.cmd = ETHTOOL_SSET;

  eth_cmd.autoneg = data_set_.autoneg_enabled_ ? AUTONEG_ENABLE : AUTONEG_DISABLE;
  eth_cmd.advertising = data_set_.autoneg_advertising_;
  eth_cmd.duplex = data_set_.duplex_;
  eth_cmd.speed = data_set_.speed_ & 0xFFFF;
  eth_cmd.speed_hi = static_cast<uint16_t>((data_set_.speed_ >> 16) & 0xFFFF);
  eth_cmd.eth_tp_mdix_ctrl = static_cast<uint8_t>(data_set_.mdix_state_);

  int ioctl_result = ioctl(socket_, SIOCETHTOOL, &ifreq_);
  if (ioctl_result < 0) {
    throw ::std::system_error(errno, ::std::system_category(), "EthernetInterface: ioctl() ETHTOOL_SSET failed");
  }

  // Link Up/Down control over SIOCSIFFLAGS

  if_flags &= ~IFF_UP;
  if_flags = static_cast<int16_t>(if_flags | (static_cast<bool>(data_set_.if_state_) ? IFF_UP : 0));

  SetIfFlags(if_flags);

  if (port_powercycle_needed) {
    /* Port Down */
    SetIfFlags(if_flags & ~IFF_UP);
    /* Port Up */
    SetIfFlags(if_flags | IFF_UP);
  }

}

void EthernetInterface::FillDataFromMauType(EthernetMauType mau_type) {
  if (mau_type == EthernetMauType::TYPE_10BASE_T) {
    if (IsSet(data_set_.autoneg_supported_, SUPPORTED_10baseT_Half)
        || IsSet(data_set_.autoneg_supported_, SUPPORTED_10baseT_Full)) {
      data_set_.speed_ = SPEED_10;
      data_set_.duplex_ = DUPLEX_UNKNOWN;
    }
  } else if (mau_type == EthernetMauType::TYPE_10BASE_TXHD) {
    if (IsSet(data_set_.autoneg_supported_, SUPPORTED_10baseT_Half)) {
      data_set_.speed_ = SPEED_10;
      data_set_.duplex_ = DUPLEX_HALF;
    }
  } else if (mau_type == EthernetMauType::TYPE_10BASE_TXFD) {
    if (IsSet(data_set_.autoneg_supported_, SUPPORTED_10baseT_Full)) {
      data_set_.speed_ = SPEED_10;
      data_set_.duplex_ = DUPLEX_FULL;
    }
  } else if (mau_type == EthernetMauType::TYPE_100BASE_TXHD) {
    if (IsSet(data_set_.autoneg_supported_, SUPPORTED_100baseT_Half)) {
      data_set_.speed_ = SPEED_100;
      data_set_.duplex_ = DUPLEX_HALF;
    }
  } else if (mau_type == EthernetMauType::TYPE_100BASE_TXFD) {
    if (IsSet(data_set_.autoneg_supported_, SUPPORTED_100baseT_Full)) {
      data_set_.speed_ = SPEED_100;
      data_set_.duplex_ = DUPLEX_FULL;
    }
  } else if (mau_type == EthernetMauType::TYPE_1000BASE_XHD) {
    if (IsSet(data_set_.autoneg_supported_, SUPPORTED_1000baseT_Half)) {
      data_set_.speed_ = SPEED_1000;
      data_set_.duplex_ = DUPLEX_HALF;
    }
  } else if (mau_type == EthernetMauType::TYPE_1000BASE_XFD) {
    if (IsSet(data_set_.autoneg_supported_, SUPPORTED_1000baseT_Full)) {
      data_set_.speed_ = SPEED_1000;
      data_set_.duplex_ = DUPLEX_FULL;
    }
  } else if (mau_type == EthernetMauType::TYPE_10BASE_FLHD) {
    if (IsSet(data_set_.autoneg_supported_, SUPPORTED_FIBRE | SUPPORTED_10baseT_Half)) {
      data_set_.speed_ = SPEED_10;
      data_set_.duplex_ = DUPLEX_HALF;
    }
  } else if (mau_type == EthernetMauType::TYPE_10BASE_FLFD) {
    if (IsSet(data_set_.autoneg_supported_, SUPPORTED_FIBRE | SUPPORTED_10baseT_Full)) {
      data_set_.speed_ = SPEED_10;
      data_set_.duplex_ = DUPLEX_FULL;
    }
  } else if (mau_type == EthernetMauType::TYPE_100BASE_FXHD) {
    if (IsSet(data_set_.autoneg_supported_, SUPPORTED_FIBRE | SUPPORTED_100baseT_Half)) {
      data_set_.speed_ = SPEED_10;
      data_set_.duplex_ = DUPLEX_HALF;
    }
  } else if (mau_type == EthernetMauType::TYPE_100BASE_FXFD) {
    if (IsSet(data_set_.autoneg_supported_, SUPPORTED_FIBRE | SUPPORTED_100baseT_Full)) {
      data_set_.speed_ = SPEED_10;
      data_set_.duplex_ = DUPLEX_FULL;
    }
  }
}

void EthernetInterface::SetAutoneg(eth::Autoneg autoneg) {
  data_set_.autoneg_enabled_ = (autoneg == eth::Autoneg::On);
}

void EthernetInterface::SetSpeed(::std::int32_t speed) {
  if (speed > 0) {
    data_set_.speed_ = static_cast<::std::uint32_t>(speed);
  }
}

void EthernetInterface::SetDuplex(eth::Duplex duplex) {
  data_set_.duplex_ = static_cast<uint8_t>(duplex);
}

void EthernetInterface::SetState(DeviceState state) {
  data_set_.if_state_ = state;
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

//---- End of source file ------------------------------------------------------
