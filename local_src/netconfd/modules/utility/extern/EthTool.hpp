// SPDX-License-Identifier: LGPL-3.0-or-later
#pragma once

#include <linux/ethtool.h>
#include <linux/if.h>
#include <variant>
#include <gsl/gsl>
#include "NetworkInterfaceConstants.hpp"
#include "Socket.hpp"

namespace netconf {

using EthToolLinkBits = ::gsl::span<const uint32_t>;

template <typename... Indexes>
[[nodiscard]] constexpr bool TestLinkModesBits(EthToolLinkBits  ethtoolbits,Indexes... idx){
  constexpr size_t bitsperlement = sizeof(EthToolLinkBits::element_type)*8;
  if((((idx / bitsperlement) > ethtoolbits.size()) || ...))
  {
    return false;
  }
  else{
    return ((static_cast<EthToolLinkBits::element_type>(ethtoolbits[idx/bitsperlement] & (1<<(idx%bitsperlement))) == static_cast<EthToolLinkBits::element_type>(1<<(idx%bitsperlement))) && ...);
  }
}

namespace detail {
template<int MasksNWordsMax>
struct EthToolLinkSettings_ {
  struct ethtool_link_settings s_;
  std::uint32_t link_mode_masks_[3 * MasksNWordsMax];

  constexpr int maxwords() {
    return MasksNWordsMax;
  }
  constexpr size_t calcsize(int nwords) {
    return sizeof(s_) + nwords * 3 * sizeof(uint32_t);
  }
};
}
using EthToolLinkSettings = detail::EthToolLinkSettings_<4>;

/* Ensure there is no padding between s_ and link_mode_masks_ */
static_assert(sizeof(EthToolLinkSettings) == sizeof(struct ethtool_link_settings) + sizeof(EthToolLinkSettings::link_mode_masks_));

class EthToolSettings {
 public:
  explicit constexpr EthToolSettings(const EthToolLinkSettings &ls) noexcept
      : e_ { ls },
        if_state_ { eth::DeviceState::Down },
        if_link_state_ { eth::InterfaceLinkState::Down } {
  }
  explicit constexpr EthToolSettings(const ethtool_cmd &cmd) noexcept
      : e_ { cmd },
        if_state_ { eth::DeviceState::Down },
        if_link_state_ { eth::InterfaceLinkState::Down } {
  }
  explicit constexpr EthToolSettings(EthToolLinkSettings &&ls) noexcept
      : e_ { std::move(ls) },
        if_state_ { eth::DeviceState::Down },
        if_link_state_ { eth::InterfaceLinkState::Down } {
  }
  explicit constexpr EthToolSettings(ethtool_cmd &&cmd) noexcept
      : e_ { std::move(cmd) },
        if_state_ { eth::DeviceState::Down },
        if_link_state_ { eth::InterfaceLinkState::Down } {
  }

  gsl::span<const uint32_t> GetSupported() const;
  gsl::span<const uint32_t> GetAutonegAdvertising() const;
  void SetAutonegAdvertising(gsl::span<const uint32_t> autoneg_advertising);
  eth::TpMdiType GetMdixState() const;
  void SetMdixState(eth::TpMdiType mdix_state);
  bool IsAutonegEnabled() const;
  void SetAutonegEnabled(bool autoneg_enabled);
  eth::Duplex GetDuplex() const;
  void SetDuplex(eth::Duplex duplex);
  ::std::uint32_t GetSpeed() const;
  void SetSpeed(int speed);
  eth::InterfaceLinkState GetIfLinkState() const;
  eth::DeviceState GetIfState() const;
  void SetIfState(eth::DeviceState if_state);
  eth::MediaType GetMediaType() const;

  EthToolLinkSettings& GetLinkSettings();
  ethtool_cmd& GetEthToolCmd();
 private:
  friend class EthTool;
  ::std::variant<EthToolLinkSettings, ethtool_cmd> e_;

  eth::DeviceState if_state_;
  eth::InterfaceLinkState if_link_state_;
};

class EthTool {
 public:
  EthTool(Socket s, const std::string& ifname);
  virtual ~EthTool() = default;

  EthToolSettings Create();

  void Update(EthToolSettings &s);
  void Commit(EthToolSettings &s);

  bool SupportsLinkSettings() {
    return supports_link_settings_;
  }
 private:
  int link_mode_masks_nwords_ = 0;
  Socket socket_;
  bool supports_link_settings_ = false;
  ::ifreq ifreq_;

  EthToolSettings ReadCurrent();
  void DetermineSettingsCommand();
  void EthToolIoctl();
  void IffFlagsGetIoctl();
  void IffFlagsSetIoctl();
  bool SetRequestHasChanges(const EthToolSettings &getdata, const EthToolSettings &setdata);
};

} /* namespace netconf */

