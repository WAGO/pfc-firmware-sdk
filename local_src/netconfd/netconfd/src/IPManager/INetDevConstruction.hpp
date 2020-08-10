// SPDX-License-Identifier: GPL-2.0-or-later
#pragma once

#include "NetDev.hpp"

namespace netconf {

class INetDevConstruction {
 public:
  INetDevConstruction() = default;
  virtual ~INetDevConstruction() = default;

  INetDevConstruction(const INetDevConstruction&) = delete;
  INetDevConstruction& operator=(const INetDevConstruction&) = delete;
  INetDevConstruction(const INetDevConstruction&&) = delete;
  INetDevConstruction& operator=(const INetDevConstruction&&) = delete;

  virtual void OnNetDevCreated(NetDevPtr netdev) = 0;
  virtual void OnNetDevRemoved(NetDevPtr netdev) = 0;
};

}
