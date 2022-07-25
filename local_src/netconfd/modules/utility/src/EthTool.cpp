// SPDX-License-Identifier: LGPL-3.0-or-later

#include "EthTool.hpp"

#include <tuple>
#include <cstdlib>
#include <cstring>

#include <linux/sockios.h>
#include <sys/ioctl.h>

namespace netconf {

// helper type for the visitor
template<class ... Ts>
struct overloaded : Ts... {
  using Ts::operator()...;
};
// explicit deduction guide (not needed as of C++20)
template <class... Ts>
overloaded(Ts...) -> overloaded<Ts...>;

struct SetRequestHasChangesVisitor {
  bool operator()(const ethtool_cmd &lhs, const ethtool_cmd &rhs) {
    return ::std::tie(lhs.autoneg, lhs.advertising, lhs.duplex, lhs.speed, lhs.eth_tp_mdix_ctrl)
        != ::std::tie(rhs.autoneg, rhs.advertising, rhs.duplex, rhs.speed, rhs.eth_tp_mdix_ctrl);
  }

  bool operator()(const EthToolLinkSettings &getdata, const EthToolLinkSettings &setdata) {
    /* r/w values*/
    if (setdata.s_.link_mode_masks_nwords < 0 || getdata.s_.link_mode_masks_nwords < 0) {
      return false;
    }

    auto nwords = static_cast<::std::uint8_t>(setdata.s_.link_mode_masks_nwords);
    return std::tie(getdata.s_.speed, getdata.s_.duplex, getdata.s_.autoneg)
        != std::tie(setdata.s_.speed, setdata.s_.duplex, setdata.s_.autoneg)
        || (getdata.s_.eth_tp_mdix != setdata.s_.eth_tp_mdix_ctrl)
        || (memcmp(&getdata.link_mode_masks_[nwords],
                   &setdata.link_mode_masks_[nwords],
                   nwords * 3 * sizeof(uint32_t)) != 0);
  }

  bool operator()([[maybe_unused]] const EthToolLinkSettings &getdata, [[maybe_unused]] const ethtool_cmd &setdata) {
    throw ::std::runtime_error("invalid settings comparison");
  }
  bool operator()([[maybe_unused]] const ethtool_cmd &getdata, [[maybe_unused]] const EthToolLinkSettings &setdata) {
    throw ::std::runtime_error("invalid settings comparison");
  }
};

gsl::span<const uint32_t> EthToolSettings::GetSupported() const {
  return std::visit(overloaded { [](const EthToolLinkSettings &arg) noexcept {
    return gsl::span<const uint32_t>(&arg.link_mode_masks_[0], static_cast<size_t>(arg.s_.link_mode_masks_nwords));
  },
  [](const ethtool_cmd &arg) noexcept {
    return gsl::span<const uint32_t>(&arg.supported, 1);
  } },
                    e_);
}

gsl::span<const uint32_t> EthToolSettings::GetAutonegAdvertising() const {
  return std::visit(
      overloaded { [](const EthToolLinkSettings &arg) noexcept {
        return gsl::span<const uint32_t>(&arg.link_mode_masks_[arg.s_.link_mode_masks_nwords],
                                         static_cast<size_t>(arg.s_.link_mode_masks_nwords));
      },
      [](const ethtool_cmd &arg) noexcept {
        return gsl::span<const uint32_t>(&arg.advertising, 1);
      } },
      e_);
}

void EthToolSettings::SetAutonegAdvertising(gsl::span<const uint32_t> autoneg_advertising) {
  ::std::visit(
      overloaded { [&](EthToolLinkSettings &arg) noexcept {
        ::std::memcpy(
            &arg.link_mode_masks_[arg.s_.link_mode_masks_nwords],
            autoneg_advertising.data(),
            ::std::min(static_cast<size_t>(arg.s_.link_mode_masks_nwords), autoneg_advertising.size())
                * sizeof(arg.link_mode_masks_[0]));
      },
      [&](ethtool_cmd &arg) noexcept {
        arg.advertising = *autoneg_advertising.data();
      } },
      e_);
}

eth::TpMdiType EthToolSettings::GetMdixState() const {
  return std::visit(overloaded { [](const EthToolLinkSettings &arg) noexcept {
    return static_cast<eth::TpMdiType>(arg.s_.eth_tp_mdix);
  },
  [](const ethtool_cmd &arg) noexcept {
    return static_cast<eth::TpMdiType>(arg.eth_tp_mdix);
  } },
                    e_);
}

void EthToolSettings::SetMdixState(eth::TpMdiType mdix_state) {
  std::visit(overloaded { [&](EthToolLinkSettings &arg) noexcept {
    arg.s_.eth_tp_mdix_ctrl = static_cast<decltype(arg.s_.eth_tp_mdix_ctrl)>(mdix_state);
  },
  [&](ethtool_cmd &arg) noexcept {
    arg.eth_tp_mdix_ctrl = static_cast<decltype(arg.eth_tp_mdix_ctrl)>(mdix_state);
  } },
             e_);
}

bool EthToolSettings::IsAutonegEnabled() const {
  return ::std::visit(overloaded { [](const EthToolLinkSettings &arg) noexcept {
    return arg.s_.autoneg == AUTONEG_ENABLE;
  },
  [](const ethtool_cmd &arg) noexcept {
    return arg.autoneg == AUTONEG_ENABLE;
  } },
                      e_);
}

void EthToolSettings::SetAutonegEnabled(bool autoneg_enabled) {
  std::visit(overloaded { [&](EthToolLinkSettings &arg) noexcept {
    arg.s_.autoneg = autoneg_enabled ? AUTONEG_ENABLE : AUTONEG_DISABLE;
  },
  [&](ethtool_cmd &arg) noexcept {
    arg.autoneg = autoneg_enabled ? AUTONEG_ENABLE : AUTONEG_DISABLE;
  } },
             e_);
}

eth::Duplex EthToolSettings::GetDuplex() const {
  return ::std::visit(overloaded { [](const EthToolLinkSettings &arg) noexcept {
    return static_cast<eth::Duplex>(arg.s_.duplex);
  },
  [](const ethtool_cmd &arg) noexcept {
    return static_cast<eth::Duplex>(arg.duplex);
  } },
                      e_);
}

void EthToolSettings::SetDuplex(eth::Duplex duplex) {
  std::visit(overloaded { [&](EthToolLinkSettings &arg) noexcept {
    arg.s_.duplex = static_cast<decltype(arg.s_.duplex)>(duplex);
  },
  [&](ethtool_cmd &arg) noexcept {
    arg.duplex = static_cast<decltype(arg.duplex)>(duplex);
  } },
             e_);
}

::std::uint32_t EthToolSettings::GetSpeed() const {
  return ::std::visit(overloaded { [](const EthToolLinkSettings &arg) noexcept {
    return arg.s_.speed;
  },
  [](const ethtool_cmd &arg) noexcept {
    return ethtool_cmd_speed(&arg);
  } },
                      e_);
}

void EthToolSettings::SetSpeed(int speed) {
  std::visit(overloaded { [&](EthToolLinkSettings &arg) noexcept {
    arg.s_.speed = static_cast<uint32_t>(speed);
  },
  [&](ethtool_cmd &arg) noexcept {
    ethtool_cmd_speed_set(&arg, static_cast<uint32_t>(speed));
  } },
             e_);
}

eth::InterfaceLinkState EthToolSettings::GetIfLinkState() const {
  return if_link_state_;
}

eth::DeviceState EthToolSettings::GetIfState() const {
  return if_state_;
}

void EthToolSettings::SetIfState(eth::DeviceState if_state) {
  if_state_ = if_state;
}

EthToolLinkSettings& EthToolSettings::GetLinkSettings() {
  return ::std::get<EthToolLinkSettings>(e_);
}

ethtool_cmd& EthToolSettings::GetEthToolCmd() {
  return ::std::get<ethtool_cmd>(e_);
}

eth::MediaType EthToolSettings::GetMediaType() const {
  return ::std::visit(overloaded { [](const EthToolLinkSettings &arg) noexcept {
    return static_cast<eth::MediaType>(arg.s_.port);
  },
  [](const ethtool_cmd &arg) noexcept {
    return static_cast<eth::MediaType>(arg.port);
  } },
                      e_);
}

EthTool::EthTool(Socket s, const std::string &ifname)
    : socket_ { std::move(s) },
      ifreq_ { } {
  strncpy(ifreq_.ifr_name, ifname.c_str(), IFNAMSIZ);  //NOLINT

  DetermineSettingsCommand();
}

void EthTool::DetermineSettingsCommand() {
  ethtool_link_settings ls { };
  ls.cmd = ETHTOOL_GLINKSETTINGS;
  auto res = ioctl(socket_, SIOCETHTOOL, &ls, sizeof(ls));
  if (res == 0) {
    link_mode_masks_nwords_ = abs(ls.link_mode_masks_nwords);
    supports_link_settings_ = true;
  } else if (res < 0) {
    supports_link_settings_ = false;
  }
}

EthToolSettings EthTool::Create() {
  return supports_link_settings_ ? EthToolSettings { EthToolLinkSettings { } } : EthToolSettings { ethtool_cmd { } };
}

void EthTool::IffFlagsGetIoctl() {
  auto res = ioctl(socket_, SIOCGIFFLAGS, &ifreq_);
  if (res < 0) {
    throw std::system_error { errno, std::system_category() };
  }
}

void EthTool::IffFlagsSetIoctl() {
  auto res = ioctl(socket_, SIOCSIFFLAGS, &ifreq_);
  if (res < 0) {
    throw std::system_error { errno, std::system_category() };
  }
}

void EthTool::EthToolIoctl() {
  auto res = ioctl(socket_, SIOCETHTOOL, &ifreq_);
  if (res < 0) {
    throw std::system_error { errno, std::system_category() };
  }
}

void EthTool::Update(EthToolSettings &s) {
  std::visit(overloaded { [&](EthToolLinkSettings &etsl) noexcept {
    etsl.s_.link_mode_masks_nwords = static_cast<__s8 >(link_mode_masks_nwords_);
    etsl.s_.cmd = ETHTOOL_GLINKSETTINGS;
    ifreq_.ifr_data = &etsl.s_;  //NOLINT(cppcoreguidelines-pro-type-union-access) must access union
  },
  [&](ethtool_cmd &cmd) noexcept {
    cmd.cmd = ETHTOOL_GSET;
    ifreq_.ifr_data = &cmd;  //NOLINT(cppcoreguidelines-pro-type-union-access) must access union
  } },
             s.e_);

  EthToolIoctl();

  struct ethtool_value eth_value { };
  eth_value.cmd = ETHTOOL_GLINK;
  ifreq_.ifr_data = reinterpret_cast<__caddr_t >(&eth_value);  // NOLINT: need reinterpret_cast for legacy API handling.
  EthToolIoctl();
  s.if_link_state_ = static_cast<eth::InterfaceLinkState>(eth_value.data);

  IffFlagsGetIoctl();
  s.if_state_ = static_cast<eth::DeviceState>((ifreq_.ifr_flags & IFF_UP) == IFF_UP);  //NOLINT(cppcoreguidelines-pro-type-union-access) must access union
}

EthToolSettings EthTool::ReadCurrent() {
  EthToolSettings s = Create();
  Update(s);
  return s;
}

bool EthTool::SetRequestHasChanges(const EthToolSettings &getdata, const EthToolSettings &setdata) {
  return ::std::visit(SetRequestHasChangesVisitor { }, getdata.e_, setdata.e_);
}

void EthTool::Commit(EthToolSettings &s) {

  IffFlagsGetIoctl();
  ifreq_.ifr_flags = static_cast<int16_t>((ifreq_.ifr_flags & ~IFF_UP)  //NOLINT(cppcoreguidelines-pro-type-union-access) must access union
  | ((s.if_state_ == eth::DeviceState::Up) ? IFF_UP : 0));
  IffFlagsSetIoctl();

  if (s.if_state_ == eth::DeviceState::Up) {
    auto current = ReadCurrent();

    std::visit(overloaded { [&](EthToolLinkSettings &etsl) noexcept {
      etsl.s_.link_mode_masks_nwords = static_cast<__s8 >(link_mode_masks_nwords_);
      etsl.s_.cmd = ETHTOOL_SLINKSETTINGS;
      ifreq_.ifr_data = &etsl.s_;  //NOLINT(cppcoreguidelines-pro-type-union-access) must access union
    },
    [&](ethtool_cmd &cmd) noexcept {
      cmd.cmd = ETHTOOL_SSET;
      ifreq_.ifr_data = &cmd;  //NOLINT(cppcoreguidelines-pro-type-union-access) must access union
    } },
               s.e_);

    if (SetRequestHasChanges(current, s)) {
      EthToolIoctl();
    }
  }
}

} /* namespace netconf */
