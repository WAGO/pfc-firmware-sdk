// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include <string>
#include "NetDev.hpp"

namespace netconf {

class IMacDistributor {
 public:
  IMacDistributor() = default;
  virtual ~IMacDistributor() = default;

  IMacDistributor(const IMacDistributor&) = default;
  IMacDistributor& operator=(const IMacDistributor&) = default;
  IMacDistributor(IMacDistributor&&) = default;
  IMacDistributor& operator=(IMacDistributor&&) = default;

  virtual void AssignMacs(NetDevs &net_devs) = 0;

};

}
