// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include "IGratuitousArp.hpp"
#include "MacAddress.hpp"

namespace netconf {

class GratuitousArp : public IGratuitousArp{

 public:
  GratuitousArp(){};
  ~GratuitousArp() = default;

  GratuitousArp(const GratuitousArp &other) = default;
  GratuitousArp(GratuitousArp &&other) = default;
  GratuitousArp& operator=(const GratuitousArp &other) = default;
  GratuitousArp& operator=(GratuitousArp &&other) = default;

  void SendGratuitousArpOnPort(NetDevPtr port_netdev, Address address) const override;
  void SendGratuitousArpOnBridge(NetDevPtr bridge_netdev, Address address) const override;
  void EnableGratuitousArp(IPLinkPtr ip_link) const override;

 private:
  Status Send(const ::std::string& ip_address, const NetDevPtr& bridge_netdev, const NetDevPtr& port_netdev) const;

};

}
