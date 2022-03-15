// SPDX-License-Identifier: LGPL-3.0-or-later
#pragma once

#include <linux/if.h>
#include <mutex>
#include <gsl/gsl>

#include "Socket.hpp"
#include "NetworkInterfaceConstants.hpp"
#include "IEthernetInterface.hpp"
#include "EthTool.hpp"

namespace netconf {

class EthernetInterface : public IEthernetInterface {
 public:
  explicit EthernetInterface(::std::string name);
  virtual ~EthernetInterface() = default;
  EthernetInterface(const EthernetInterface& other) = default;
  EthernetInterface& operator=(const EthernetInterface& other) = default;
  EthernetInterface(EthernetInterface&& other) = default;
  EthernetInterface& operator=(EthernetInterface&& other) = default;

  void UpdateConfig() override;
  MacAddress GetMac() const override;
  bool GetAutonegSupport() const override;
  bool GetAutonegEnabled() const override;
  eth::MediaType GetMediaType() const override;
  eth::DeviceState GetState() const override;
  eth::InterfaceLinkState GetLinkState() const override;
  ::std::uint32_t GetInterfaceIndex() const override;
  ::std::uint32_t GetMTU() const override;
  gsl::span<const uint32_t> GetSupportedLinkModes() const override;

  ::std::uint32_t GetSpeed() const override;
  eth::Duplex GetDuplex() const override;

  void Commit() override;

  void SetAutoneg(eth::Autoneg autoneg) override;
  void SetState(eth::DeviceState state) override;
  void SetSpeed(::std::int32_t speed) override;
  void SetDuplex(eth::Duplex duplex) override;

  static ::std::string IndexToName(::std::uint32_t ifindex);

 private:
  void SetIfFlags(int16_t if_flags);
  int16_t GetIfFlags() const;
  void InitializeData();

  mutable ::std::mutex data_mutex_;
  uint8_t mac_[6];

  ::std::string name_;
  ::std::uint32_t if_index_;

  ::ifreq ifreq_;
  Socket socket_;
  ::std::size_t mtu_;

  EthTool ethtool_;
  EthToolSettings ethtool_settings_r_;
  EthToolSettings ethtool_settings_w_;

};

}
