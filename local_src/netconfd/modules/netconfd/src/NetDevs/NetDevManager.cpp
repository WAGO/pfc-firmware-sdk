// SPDX-License-Identifier: GPL-2.0-or-later

#include "NetDevManager.hpp"

#include <boost/algorithm/string/predicate.hpp>
#include <functional>
#include <net/if.h>

#include "Helper.hpp"

namespace netconf {

using std::string;
using std::make_shared;
using namespace std::literals;

constexpr DeviceType DeviceTypesAllowingIP = DeviceType::Bridge | DeviceType::Wwan;

DeviceType NetDevManager::DetermineNetDevKind(const ::std::string &name) {
  if (boost::starts_with(name, "ethX")) {
    return DeviceType::Port;
  }
  if (boost::starts_with(name, "eth")) {
    return DeviceType::Ethernet;
  }
  if (boost::starts_with(name, "br")) {
    return DeviceType::Bridge;
  }
  if (boost::starts_with(name, "wwan")) {
    return DeviceType::Wwan;
  }
  if (boost::starts_with(name, "vpn")) {
    return DeviceType::Virtual;
  }
  if (boost::starts_with(name, "lo")) {
    return DeviceType::Loopback;
  }

  return DeviceType::Other;
}

void NetDevManager::InitializeNetDevs() {

  for (auto name : bridge_controller_.GetInterfaces()) {
    auto kind = DetermineNetDevKind(name);

    if (kind == DeviceType::Ethernet) {
      CreateNetdev(name, name, kind);
    }
    if (kind == DeviceType::Port) {
      auto label = name;
      label.erase(0, "eth"s.size());
      CreateNetdev(name, label, kind);
    }
    if (kind == DeviceType::Wwan) {
      CreateNetdev(name, name, kind);
    }
  }

}

BridgeConfig NetDevManager::GetActualBridgeConfig() {
  BridgeConfig config;
  auto bridges = bridge_controller_.GetBridges();
  for (auto bridge : bridges) {
    auto system_ports = bridge_controller_.GetBridgeInterfaces(bridge);

    ::std::vector<Interface> ports;
    std::transform(system_ports.begin(), system_ports.end(), std::back_inserter(ports), [](Interface itf) {
      itf.erase(0, "eth"s.size());
      return itf;
    });

    config.emplace(bridge, ports);
  }
  return config;
}

NetDevManager::NetDevManager(::std::shared_ptr<IInterfaceMonitor> interface_monitor,
                             IBridgeController &bridge_controller, IMacDistributor &mac_distributor)
    : interface_monitor_ { interface_monitor },
      netdev_construction_ { nullptr },
      bridge_controller_ { bridge_controller },
      mac_distributor_ { mac_distributor } {

  InitializeNetDevs();

  BridgeConfig bridge_config = GetActualBridgeConfig();
  ConfigureBridges(bridge_config);

  interface_monitor_->RegisterEventHandler(*this);
}

NetDevPtr NetDevManager::FindIf(const NetDevs &netdevs, NetDevPredicate predicate) {

  auto it = ::std::find_if(netdevs.begin(), netdevs.end(), predicate);
  return it != netdevs.end() ? *it : NetDevPtr { nullptr };
}

NetDevPtr NetDevManager::CreateNetdev(::std::string name, ::std::string label, DeviceType kind) {
  auto netdev = GetByName(name);

  if (not netdev) {
    auto if_index = if_nametoindex(name.c_str());

    netdev = make_shared < NetDev > (if_index, name, label, kind, not (DeviceTypesAllowingIP && kind));
    netdev->SetIfFlags(interface_monitor_->GetIffFlags(if_index));

    net_devs_.push_back(netdev);
    OnNetDevCreated(netdev);
  }

  return netdev;
}

void NetDevManager::RemoveObsoleteBridgeNetdevs(const BridgeConfig &config) {
  for (auto &bridge_netdev : GetBridgeNetDevs()) {
    if (config.count(bridge_netdev->GetName()) == 0) {
      OnNetDevRemoved(bridge_netdev);
      NetDev::RemoveChildren(bridge_netdev);

      auto it = ::std::find(net_devs_.begin(), net_devs_.end(), bridge_netdev);
      assert((it != net_devs_.end()));  //NOLINT(cppcoreguidelines-pro-bounds-array-to-pointer-decay)
      net_devs_.erase(it);
    }

  }
}

void NetDevManager::RemoveObsoleteInterfaceRelations(const BridgeConfig &config) {
  for (auto &bridge_netdev : GetBridgeNetDevs()) {
    auto netdev_itfs = bridge_netdev->GetChildren();
    auto &itf_labels = config.at(bridge_netdev->GetName());
    bool change_ift_relations = false;
    for (auto &netdev_itf : netdev_itfs) {
      auto label = netdev_itf->GetLabel();
      auto not_found = ::std::find(itf_labels.begin(), itf_labels.end(), label)
          == itf_labels.end();
      if (not_found) {
        NetDev::RemoveParent(netdev_itf);
        change_ift_relations = true;
      }
    }
    if(change_ift_relations){
      OnNetDevChangeInterfaceRelations(bridge_netdev);
    }
  }
}

NetDevs NetDevManager::GetNetdevs(const Interfaces &interfaces) const {

  NetDevs netdevs;

  auto find_and_add_to_netdevs = [&](auto &netdev) {
    if (IsIncluded(netdev->GetLabel(), interfaces)) {
      netdevs.push_back(netdev);
    }
  };

  ::std::for_each(net_devs_.begin(), net_devs_.end(), find_and_add_to_netdevs);

  return netdevs;
}

void NetDevManager::CreateBridgesAndRelateToInterfaces(const BridgeConfig &config) {
  for (auto &cfg : config) {
    auto &bridge_name = ::std::get < 0 > (cfg);
    auto &interfaces = ::std::get < 1 > (cfg);
    auto bridge_netdev = CreateNetdev(bridge_name, bridge_name, DeviceType::Bridge);

    auto establish_relation = [&](auto &netdev) {
      if (IsIncluded(netdev->GetLabel(), interfaces)) {
        NetDev::SetRelation(netdev, bridge_netdev);
      }
    };

    ::std::for_each(net_devs_.begin(), net_devs_.end(), establish_relation);
  }

}

void NetDevManager::ConfigureBridges(const BridgeConfig &config) {
  RemoveObsoleteBridgeNetdevs(config);
  RemoveObsoleteInterfaceRelations(config);
  CreateBridgesAndRelateToInterfaces(config);

  mac_distributor_.AssignMacs(net_devs_);
}

NetDevPtr NetDevManager::GetByName(::std::string name) {
  auto pred = [&name](const auto &net_dev) {
    return net_dev->GetName() == name;
  };
  return FindIf(net_devs_, pred);
}

NetDevPtr NetDevManager::GetByLabel(::std::string name) {
  auto pred = [&name](const auto &net_dev) {
    return net_dev->GetLabel() == name;
  };
  return FindIf(net_devs_, pred);
}

NetDevPtr NetDevManager::GetByIfIndex(::std::uint32_t if_index) {
  auto pred = [=](const auto &net_dev) {
    return net_dev->GetIndex() == if_index;
  };
  return FindIf(net_devs_, pred);
}

NetDevs NetDevManager::GetNetDevs() {
  return net_devs_;
}

NetDevs NetDevManager::GetNetDevs(DeviceType kind) {
  NetDevs net_devs;

  ::std::copy_if(net_devs_.begin(), net_devs_.end(), std::back_inserter(net_devs), TypeMatches(kind));

  return net_devs;
}

NetDevs NetDevManager::GetEthernetNetDevs() {
  NetDevs ethernet_net_devs;

  ::std::copy_if(net_devs_.begin(), net_devs_.end(), std::back_inserter(ethernet_net_devs),
                 TypeMatches(DeviceType::Ethernet));

  return ethernet_net_devs;
}

NetDevs NetDevManager::GetBridgeNetDevs() {
  NetDevs bridge_net_devs;

  ::std::copy_if(net_devs_.begin(), net_devs_.end(), std::back_inserter(bridge_net_devs),
                 TypeMatches(DeviceType::Bridge));

  return bridge_net_devs;
}

NetDevs NetDevManager::GetPortNetDevs() {
  NetDevs port_net_devs;

  ::std::copy_if(net_devs_.begin(), net_devs_.end(), std::back_inserter(port_net_devs), TypeMatches(DeviceType::Port));

  return port_net_devs;
}

bool NetDevManager::Exists(const NetDevs &netdevs, NetDevPredicate predicate) {
  auto it = ::std::find_if(netdevs.begin(), netdevs.end(), predicate);
  return (it != netdevs.end());
}

bool NetDevManager::ExistsByName(::std::string name, const NetDevs &netdevs) {
  auto names_are_equal = [&](const auto &net_dev) {
    return net_dev->GetName() == name;
  };
  return NetDevManager::Exists(netdevs, names_are_equal);
}

bool NetDevManager::DoesNotExistByName(::std::string name, const NetDevs &netdevs) {
  return !ExistsByName(::std::move(name), netdevs);
}

bool NetDevManager::ExistsByLabel(::std::string label, const NetDevs &netdevs) {
  auto labels_are_equal = [&label](const auto &net_dev) {
    return net_dev->GetLabel() == label;
  };
  return NetDevManager::Exists(netdevs, labels_are_equal);
}

bool NetDevManager::DoesNotExistByLabel(::std::string label, const NetDevs &netdevs) {
  return !ExistsByLabel(::std::move(label), netdevs);
}

void NetDevManager::LinkChange(::std::uint32_t if_index, ::std::uint32_t flags) {

  auto netdev = GetByIfIndex(if_index);
  if (netdev) {
    netdev->SetIfFlags(flags);
  }
}

void NetDevManager::RegisterForNetDevConstructionEvents(INetDevEvents &netdev_construction) {
  netdev_construction_ = &netdev_construction;
}

void NetDevManager::OnNetDevCreated(NetDevPtr netdev) const {
  if (netdev_construction_ != nullptr) {
    netdev_construction_->OnNetDevCreated(::std::move(netdev));
  }
}

void NetDevManager::OnNetDevRemoved(NetDevPtr netdev) const {
  if (netdev_construction_ != nullptr) {
    netdev_construction_->OnNetDevRemoved(::std::move(netdev));
  }
}

void NetDevManager::OnNetDevChangeInterfaceRelations(NetDevPtr netdev) const {
  if (netdev_construction_ != nullptr) {
    netdev_construction_->OnNetDevChangeInterfaceRelations(::std::move(netdev));
  }
}

}  // namespace netconf
