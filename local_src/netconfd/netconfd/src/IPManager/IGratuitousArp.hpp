// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include <cstdint>
#include "Status.hpp"
#include "NetDev.hpp"

namespace netconf {

class IGratuitousArp {
 public:
  IGratuitousArp() = default;
  virtual ~IGratuitousArp() = default;

  IGratuitousArp(const IGratuitousArp&) = delete;
  IGratuitousArp& operator=(const IGratuitousArp&) = delete;
  IGratuitousArp(const IGratuitousArp&&) = delete;
  IGratuitousArp& operator=(const IGratuitousArp&&) = delete;

  virtual Status Send(::std::string ip_address, const NetDevPtr bridge_netdev, const NetDevPtr port_netdev) = 0;
};

}
