// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include <memory>
#include <vector>

#include "NetDev.hpp"

namespace netconf {

class INetDevEvents;

class INetDevManager {
 public:
  INetDevManager() = default;
  virtual ~INetDevManager() = default;

  INetDevManager(const INetDevManager&) = delete;
  INetDevManager& operator=(const INetDevManager&) = delete;
  INetDevManager(const INetDevManager&&) = delete;
  INetDevManager& operator=(const INetDevManager&&) = delete;

  virtual void RegisterForNetDevConstructionEvents(INetDevEvents& netdev_construction) = 0;
  virtual NetDevPtr GetByName(::std::string name) = 0;
  virtual NetDevPtr GetByLabel(::std::string name) = 0;
  virtual NetDevPtr GetByIfIndex(::std::uint32_t if_index) = 0;
  virtual NetDevs GetEthernetNetDevs()= 0;
  virtual NetDevs GetBridgeNetDevs() = 0;
  virtual NetDevs GetPortNetDevs() = 0;
  virtual NetDevs GetNetDevs()= 0;
  virtual NetDevs GetNetDevs(DeviceType kind)= 0;
  virtual void ConfigureBridges(const BridgeConfig& config) = 0;

};

} /* namespace netconf */
