// SPDX-License-Identifier: GPL-2.0-or-later
#pragma once

#include "NetDev.hpp"

namespace netconf {

class INetDevEvents {
 public:
  INetDevEvents() = default;
  virtual ~INetDevEvents() = default;

  INetDevEvents(const INetDevEvents&) = delete;
  INetDevEvents& operator=(const INetDevEvents&) = delete;
  INetDevEvents(INetDevEvents&&) = delete;
  INetDevEvents& operator=(INetDevEvents&&) = delete;

  virtual void OnNetDevCreated(NetDevPtr netdev) = 0;
  virtual void OnNetDevRemoved(NetDevPtr netdev) = 0;
  virtual void OnNetDevChangeInterfaceRelations(NetDevPtr netdev) = 0;
};

}
