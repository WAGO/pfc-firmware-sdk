// SPDX-License-Identifier: LGPL-3.0-or-later
#pragma once

#include <Socket.h>
#include <linux/if.h>
#include <mutex>
#include <gsl/gsl>

#include "NetworkInterfaceConstants.hpp"
#include "IEthernetInterface.hpp"


namespace netconf {



class EthernetInterface : public IEthernetInterface {
 public:
  explicit EthernetInterface(::std::string name);
  explicit EthernetInterface(::std::uint32_t ifindex);
  EthernetInterface(EthernetInterface&& other) noexcept;

  void UpdateConfig() override;
  eth::EthernetMauType GetMauType() const override;
  MacAddress GetMac() const override;
  bool GetAutonegSupport() const override;
  bool GetAutonegEnabled() const override;
  ::std::uint32_t GetAutonegCapabilities() const override;
  eth::MediaType GetMediaType() const override;
  eth::DeviceState GetState() const override;
  eth::InterfaceLinkState GetLinkState() const override;
  ::std::uint32_t GetInterfaceIndex() const override;
  ::std::uint32_t GetAutonegCapabilitiesXdot3() const override;
  ::std::uint32_t GetMTU() const override;

  void Commit() override;

  void SetAutoneg(eth::Autoneg autoneg) override;
  void SetState(eth::DeviceState state) override;
  void SetSpeed(::std::int32_t speed) override;
  void SetDuplex(eth::Duplex duplex) override;

  static ::std::string IndexToName(::std::uint32_t ifindex);

 private:
  void SetIfFlags(int16_t if_flags);
  int16_t GetIfFlags() const;
  void FillDataFromMauType(eth::EthernetMauType mau_type);
  void InitializeData();

  struct Data {
    bool autoneg_enabled_;
    ::std::uint32_t autoneg_supported_;
    ::std::uint32_t autoneg_advertising_;
    eth::MediaType media_type_;
    ::std::uint8_t duplex_;
    ::std::uint32_t speed_;
    eth::TpMdiType mdix_state_;
    eth::DeviceState if_state_;
    eth::InterfaceLinkState if_link_state_;
  };

  mutable ::std::mutex data_mutex_;
  uint8_t mac_[6];

  ::std::string name_;
  ::std::uint32_t if_index_;

  ::ifreq ifreq_;
  Socket socket_;
  Data data_get_;
  Data data_set_;
  ::std::size_t mtu_;

};

}
