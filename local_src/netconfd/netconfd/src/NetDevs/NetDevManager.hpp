// SPDX-License-Identifier: GPL-2.0-or-later
#pragma once

#include <algorithm>
#include <functional>
#include <memory>
#include <vector>

#include "IInterfaceMonitor.hpp"
#include "IDeviceProperties.hpp"
#include "NetDev.hpp"
#include "INetDevConstruction.hpp"
#include "INetDevManager.hpp"
#include "IBridgeController.hpp"

namespace netconfd {

class NetDevManager : public INetDevManager, public IInterfaceEvent {

 public:

  NetDevManager(::std::shared_ptr<IInterfaceMonitor> interface_monitor, IBridgeController &bridge_controller);
  NetDevPtr GetByName(::std::string name) override;
  NetDevPtr GetByLabel(::std::string name) override;
  NetDevPtr GetByIfIndex(::std::uint32_t if_index) override;
  NetDevs GetEthernetNetDevs() override;
  NetDevs GetBridgeNetDevs() override;
  NetDevs GetPortNetDevs() override;
  NetDevs GetNetDevs() override;
  void ConfigureBridges(const BridgeConfig& config) override;
  void LinkChange(::std::uint32_t if_index, ::std::uint32_t flags) override;
  void RegisterForNetDevConstructionEvents(INetDevConstruction& netdev_construction) override;

  static bool ExistsByName(::std::string name, const NetDevs& netdevs);
  static bool DoesNotExistByName(::std::string name, const NetDevs& netdevs);
  static bool ExistsByLabel(::std::string label, const NetDevs& netdevs);
  static bool DoesNotExistByLabel(::std::string label, const NetDevs& netdevs);
  static NetDev::Kind DetermineNetDevKind(const ::std::string& name);

 private:

  void OnNetDevCreated(NetDevPtr netdev) const;
  void OnNetDevRemoved(NetDevPtr netdev) const;
  NetDevPtr CreateNetdev(::std::string name, ::std::string label, NetDev::Kind kind);
  static bool Exists(const NetDevs& netdevs, const ::std::function<bool(const ::std::shared_ptr<const NetDev>&)>& predicate);
  ::std::shared_ptr<NetDev> FindIf(const ::std::function<bool(::std::shared_ptr<const NetDev>)>& predicate);
  void RemoveObsoleteBridgeNetdevs(const BridgeConfig& config);
  void RemoveObsoleteInterfaceRelations(const BridgeConfig& config);
  void CreateBridgesAndRelateToInterfaces(const BridgeConfig& config);

  ::std::shared_ptr<IInterfaceMonitor> interface_monitor_;
  ::std::vector<::std::shared_ptr<NetDev>> net_devs_;
  INetDevConstruction* netdev_construction_;
  IBridgeController &bridge_controller_;
};


}
