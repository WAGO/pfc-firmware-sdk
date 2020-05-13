// SPDX-License-Identifier: GPL-2.0-or-later

#include "NetDevManager.hpp"

#include <boost/algorithm/string/predicate.hpp>
#include <functional>
#include <net/if.h>

#include "Helper.hpp"

namespace netconfd {

using std::string;
using std::make_shared;
using namespace std::literals;

struct KindMatches {
  explicit KindMatches(NetDev::Kind kind)
      :
      kind_ { kind } {
  }

  bool operator()(const NetDevPtr &netdev) const {
    return netdev->GetKind() == kind_;
  }

  NetDev::Kind kind_;
};

NetDev::Kind NetDevManager::DetermineNetDevKind(const ::std::string &name) {
  if (boost::starts_with(name, "eth")) {
    return NetDev::Kind::Ethernet;
  }
  if (boost::starts_with(name, "br")) {
    return NetDev::Kind::Bridge;
  }
  if (boost::starts_with(name, "wwan")) {
    return NetDev::Kind::Wwan;
  }
  if (boost::starts_with(name, "vpn")) {
    return NetDev::Kind::Virtual;
  }
  if (boost::starts_with(name, "lo")) {
    return NetDev::Kind::Loopback;
  }

  return NetDev::Kind::Other;
}

NetDevManager::NetDevManager(::std::shared_ptr<IInterfaceMonitor> interface_monitor, IBridgeController &bridge_controller)
    :
    interface_monitor_ { interface_monitor },
    netdev_construction_ { nullptr },
    bridge_controller_ { bridge_controller } {

  for (auto &interface_name : bridge_controller.GetInterfaces()) {

    auto kind = DetermineNetDevKind(interface_name);

    auto interface_label = interface_name;
    if (kind == NetDev::Kind::Ethernet) {
      interface_label.erase(0, "eth"s.size());
    }

    if (kind == NetDev::Kind::Ethernet || kind == NetDev::Kind::Wwan || kind == NetDev::Kind::Bridge) {
      CreateNetdev(interface_name, interface_label, kind);
    }
  }

  interface_monitor_->RegisterEventHandler(*this);
}

::std::shared_ptr<NetDev> NetDevManager::FindIf(
    const ::std::function<bool(::std::shared_ptr<const NetDev>)> &predicate) {

  auto it = ::std::find_if(net_devs_.begin(), net_devs_.end(), predicate);
  return it != net_devs_.end() ? *it : ::std::shared_ptr<NetDev> { nullptr };
}

NetDevPtr NetDevManager::CreateNetdev(::std::string name, ::std::string label, NetDev::Kind kind) {
  auto netdev = GetByName(name);

  if (not netdev) {
    auto if_index = if_nametoindex(name.c_str());

    netdev = make_shared<NetDev>(if_index, name, label, kind);
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
    auto &bridge_itf_labels = config.at(bridge_netdev->GetName());
    for (auto &netdev_itf : netdev_itfs) {
      auto label = netdev_itf->GetLabel();
      auto not_found = ::std::find(bridge_itf_labels.begin(), bridge_itf_labels.end(), label)
          == bridge_itf_labels.end();
      if (not_found) {
        NetDev::RemoveParent(netdev_itf);
      }
    }
  }
}

void NetDevManager::CreateBridgesAndRelateToInterfaces(const BridgeConfig &config) {
  for (auto &cfg : config) {
    auto &bridge_name = ::std::get<0>(cfg);
    auto &interfaces = ::std::get<1>(cfg);
    auto bridge_netdev = CreateNetdev(bridge_name, bridge_name, NetDev::Kind::Bridge);

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
}

NetDevPtr NetDevManager::GetByName(::std::string name) {
  auto pred = [&name](const auto &net_dev) {
    return net_dev->GetName() == name;
  };
  return FindIf(pred);
}

NetDevPtr NetDevManager::GetByLabel(::std::string name) {
  auto pred = [&name](const auto &net_dev) {
    return net_dev->GetLabel() == name;
  };
  return FindIf(pred);
}

NetDevPtr NetDevManager::GetByIfIndex(::std::uint32_t if_index) {
  auto pred = [=](const auto &net_dev) {
    return net_dev->GetIndex() == if_index;
  };
  return FindIf(pred);
}

NetDevs NetDevManager::GetNetDevs() {
  return net_devs_;
}

NetDevs NetDevManager::GetEthernetNetDevs() {
  NetDevs ethernet_net_devs;

  ::std::copy_if(net_devs_.begin(), net_devs_.end(), std::back_inserter(ethernet_net_devs),
                 KindMatches(NetDev::Kind::Ethernet));

  return ethernet_net_devs;
}

NetDevs NetDevManager::GetBridgeNetDevs() {
  NetDevs bridge_net_devs;

  ::std::copy_if(net_devs_.begin(), net_devs_.end(), std::back_inserter(bridge_net_devs),
                 KindMatches(NetDev::Kind::Bridge));

  return bridge_net_devs;
}

//TODO(Team Pnd): not nice but how to solve? Own Kind? used by InterfaceConfigManager to describe the ports
NetDevs NetDevManager::GetPortNetDevs() {
  NetDevs port_net_devs;

  ::std::copy_if(net_devs_.begin(), net_devs_.end(), std::back_inserter(port_net_devs), [](NetDevPtr netdev) {
    return boost::starts_with(netdev->GetLabel(), "X");
  });

  return port_net_devs;
}

bool NetDevManager::Exists(const NetDevs &netdevs,
                           const ::std::function<bool(const ::std::shared_ptr<const NetDev>&)> &predicate) {
  auto it = ::std::find_if(netdevs.begin(), netdevs.end(), predicate);
  return (it != netdevs.end());
}

bool NetDevManager::ExistsByName(::std::string name, const NetDevs &netdevs) {
  auto names_are_equal = [name](const auto &net_dev) {
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

void NetDevManager::RegisterForNetDevConstructionEvents(INetDevConstruction &netdev_construction) {
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

}  // namespace netconfd
