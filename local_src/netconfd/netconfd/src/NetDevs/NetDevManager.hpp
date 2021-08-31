// SPDX-License-Identifier: GPL-2.0-or-later
#pragma once

#include <algorithm>
#include <functional>
#include <memory>
#include <vector>

#include "../IPManager/INetDevEvents.hpp"
#include "IInterfaceMonitor.hpp"
#include "IDeviceProperties.hpp"
#include "NetDev.hpp"
#include "INetDevManager.hpp"
#include "IBridgeController.hpp"
#include "IMacDistributor.hpp"

namespace netconf {

class NetDevManager : public INetDevManager, public IInterfaceEvent {
  using NetDevPredicate = ::std::function<bool(const NetDevPtr&)>;

 public:

  NetDevManager(::std::shared_ptr<IInterfaceMonitor> interface_monitor,
                IBridgeController &bridge_controller,
                IMacDistributor& mac_distributor);
  NetDevPtr GetByName(::std::string name) override;
  NetDevPtr GetByLabel(::std::string name) override;
  NetDevPtr GetByIfIndex(::std::uint32_t if_index) override;
  NetDevs GetEthernetNetDevs() override;
  NetDevs GetBridgeNetDevs() override;
  NetDevs GetPortNetDevs() override;
  NetDevs GetNetDevs() override;
  NetDevs GetNetDevs(DeviceType kind) override;
  void ConfigureBridges(const BridgeConfig& config) override;
  void LinkChange(::std::uint32_t if_index, ::std::uint32_t flags) override;
  void RegisterForNetDevConstructionEvents(INetDevEvents& netdev_construction) override;

  static bool ExistsByName(::std::string name, const NetDevs& netdevs);
  static bool DoesNotExistByName(::std::string name, const NetDevs& netdevs);
  static bool ExistsByLabel(::std::string label, const NetDevs& netdevs);
  static bool DoesNotExistByLabel(::std::string label, const NetDevs& netdevs);
  static DeviceType DetermineNetDevKind(const ::std::string& name);

 private:

  void InitializeNetDevs();
  void CreateBridgeNetDevs();
  void CreateOtherNetDevs();

  void OnNetDevCreated(NetDevPtr netdev) const;
  void OnNetDevRemoved(NetDevPtr netdev) const;
  void OnNetDevChangeInterfaceRelations(NetDevPtr netdev) const;
  NetDevPtr CreateNetdev(::std::string name, ::std::string label, DeviceType kind);
  static bool Exists(const NetDevs& netdevs, NetDevPredicate predicate);
  static NetDevPtr FindIf(const NetDevs& netdevs, NetDevPredicate predicate);
  void RemoveObsoleteBridgeNetdevs(const BridgeConfig& config);
  void RemoveObsoleteInterfaceRelations(const BridgeConfig& config);
  void CreateBridgesAndRelateToInterfaces(const BridgeConfig& config);
  NetDevs GetNetdevs(const Interfaces &interfaces) const;
  BridgeConfig GetActualBridgeConfig();

  ::std::shared_ptr<IInterfaceMonitor> interface_monitor_;
  ::std::vector<::std::shared_ptr<NetDev>> net_devs_;
  INetDevEvents* netdev_construction_;

  IBridgeController &bridge_controller_;
  IMacDistributor& mac_distributor_;
};


}
