// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include <cstdint>
#include "IPLink.hpp"
#include "Status.hpp"
#include "NetDev.hpp"

namespace netconf {


class IGratuitousArp {
 public:
  IGratuitousArp() = default;
  virtual ~IGratuitousArp() = default;

  IGratuitousArp(const IGratuitousArp&) = delete;
  IGratuitousArp& operator=(const IGratuitousArp&) = delete;
  IGratuitousArp(IGratuitousArp&&) = delete;
  IGratuitousArp& operator=(IGratuitousArp&&) = delete;

  virtual void SendGratuitousArpOnPort(NetDevPtr port_netdev, Address address) const = 0;
  virtual void SendGratuitousArpOnBridge(NetDevPtr bridge_netdev, Address address) const = 0;
  virtual void EnableGratuitousArp(IPLinkPtr ip_link) const = 0;

};

}
