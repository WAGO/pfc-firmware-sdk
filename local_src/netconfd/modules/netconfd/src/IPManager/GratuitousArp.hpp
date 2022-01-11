// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include "IGratuitousArp.hpp"
#include "MacAddress.hpp"
#include "NetDev.hpp"

namespace netconf {

class GratuitousArp : public IGratuitousArp{

 public:
  GratuitousArp(){};
  ~GratuitousArp() = default;

  Status Send(::std::string ip_address, const NetDevPtr bridge_netdev, const NetDevPtr port_netdev) override;
};

}
