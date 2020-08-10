// SPDX-License-Identifier: GPL-2.0-or-later

#include "MacDistributor.hpp"
#include "Logger.hpp"
#include "alphanum.hpp"
#include <boost/algorithm/string.hpp>
#include <gsl/gsl>

namespace netconf {

MacDistributor::MacDistributor(MacAddress mac_address, uint32_t mac_inc, IMacController &mac_controller)
    : base_mac_address_ { ::std::move(mac_address) },
      mac_inc_ { mac_inc },
      mac_controller_ { mac_controller } {

}

void MacDistributor::AssignMacs(NetDevs &net_devs) {

  auto sort_alphanum = [](const NetDevPtr &lhs, const NetDevPtr &rhs) {
    return doj::alphanum_comp(lhs->GetName(), rhs->GetName()) < 0;
  };
  ::std::sort(net_devs.begin(), net_devs.end(), sort_alphanum);

  mac_counter_ = 0;

  auto port_count = ::std::count_if(net_devs.begin(), net_devs.end(), TypeMatches(DeviceType::Port));

  if (IsMacAddressAssignmentFull(mac_inc_, port_count)) {
    AssignFullMacSupport(net_devs);
  } else if (IsMacAddressAssignmentMultiple(mac_inc_, port_count)) {
    AssignMultipleMacSupport(net_devs);
  } else {
    AssignSingleMacSupport(net_devs);
  }

  AssignEthernetMacs(net_devs);
}

static bool BridgeHasMoreThenOneAssignedPort(NetDevPtr net_dev) {
  if (net_dev) {
    auto children = net_dev->GetChildren();
    return (children.size() > 1);
  }
  return false;
}

static bool BridgeHasOneAssignedPort(NetDevPtr net_dev) {
  if (net_dev) {
    auto children = net_dev->GetChildren();
    return (children.size() == 1);
  }
  return false;
}

void MacDistributor::AssignMac(NetDevPtr net_dev, MacAddress mac) {
  mac_controller_.SetMac(mac, net_dev->GetName());
  net_dev->SetMac(mac);
}

void MacDistributor::AssignMac(NetDevPtr net_dev) {
  auto mac = base_mac_address_.Increment(mac_counter_);
  AssignMac(net_dev, mac);
}

void MacDistributor::AssignMacAndIncrement(NetDevPtr net_dev) {
  auto mac = base_mac_address_.Increment(mac_counter_);
  AssignMac(net_dev, mac);
  mac_counter_++;
}

void MacDistributor::AssignSingleMacSupport(NetDevs &net_devs) {

  //Assign mac's
  auto assign_macs = [&](NetDevPtr &net_dev) {
    if (net_dev->GetKind() == DeviceType::Bridge || net_dev->GetKind() == DeviceType::Port) {
      AssignMac(net_dev, base_mac_address_);
    }
  };

  ::std::for_each(net_devs.begin(), net_devs.end(), assign_macs);
}

void MacDistributor::AssignMultipleMacSupport(NetDevs &net_devs) {

  //Assign bridge mac's
  auto assign_mac_to_bridge = [&](NetDevPtr &net_dev) {
    if (net_dev->GetKind() == DeviceType::Bridge) {
      AssignMac(net_dev, base_mac_address_);
    }
  };

  ::std::for_each(net_devs.begin(), net_devs.end(), assign_mac_to_bridge);
  mac_counter_++;

  //Assign port mac's
  auto assign_mac_to_ports = [&](NetDevPtr &net_dev) {
    if (net_dev->GetKind() == DeviceType::Port) {
      AssignMacAndIncrement(net_dev);
    }
  };

  ::std::for_each(net_devs.begin(), net_devs.end(), assign_mac_to_ports);
}

static int DetermineBridgeMacIncrement(std::string name) {
  auto last_index = name.find_last_not_of("0123456789");
  if (last_index != ::std::string::npos) {
    try {
      return std::stoi(name.substr(last_index + 1));
    } catch (...) {
      /* NOP */
    }
  }
  return -1;
}

void MacDistributor::AssignFullMacSupport(NetDevs &net_devs) {

  ::std::list<int> mac_incs(mac_inc_);
  std::generate(mac_incs.begin(), mac_incs.end(), [n = 0]() mutable {
    return n++;
  });

  //Assign bridge mac's
  auto assign_mac_to_bridge = [&](const NetDevPtr &net_dev) {
    if (net_dev->GetKind() == DeviceType::Bridge) {
      auto inc_num = DetermineBridgeMacIncrement(net_dev->GetName());
      if ((inc_num >= 0) && (static_cast<uint32_t>(inc_num) < mac_inc_)) {
        AssignMac(net_dev, base_mac_address_.Increment(inc_num));
        mac_incs.erase(::std::find(mac_incs.begin(), mac_incs.end(), inc_num));
      } else {
        LogWarning("MAC increment couldn't be determined, taking base: " + net_dev->GetName());
        AssignMac(net_dev, base_mac_address_);
      }
    }
  };

  ::std::for_each(net_devs.begin(), net_devs.end(), assign_mac_to_bridge);

  //Assign port mac's

  auto assign_from_mac_incs = [&](const NetDevPtr &netdev) {
    if (!mac_incs.empty()) {
      auto mac = base_mac_address_.Increment(mac_incs.front());
      mac_incs.pop_front();
      AssignMac(netdev, mac);
    } else {
      LogWarning("No MAC increment left, taking base MAC: " + netdev->GetName());
      AssignMac(netdev, base_mac_address_);
    }
  };

  auto assign_mac_to_ports = [&](NetDevPtr &net_dev) {
    if (net_dev->GetKind() == DeviceType::Port) {
      auto bridge = net_dev->GetParent();
      if (BridgeHasMoreThenOneAssignedPort(bridge)) {
        assign_from_mac_incs(net_dev);
      }
      if (BridgeHasOneAssignedPort(bridge)) {
        AssignMac(net_dev, bridge->GetMac());
      }
      if (not bridge) {
        assign_from_mac_incs(net_dev);
      }
    }
  };

  ::std::for_each(net_devs.begin(), net_devs.end(), assign_mac_to_ports);
}

void MacDistributor::AssignEthernetMacs(NetDevs &net_devs) {

  for (auto net_dev : net_devs) {

    //assign mac for eth0
    if (net_dev->GetKind() == DeviceType::Ethernet) {
      AssignMac(net_dev, base_mac_address_);
    }
  }

}

bool MacDistributor::IsMacAddressAssignmentMultiple(uint32_t mac_count, uint32_t port_count) const {
  if (not IsMacAddressAssignmentFull(mac_count, port_count) && not (mac_count == 1)) {
    if (mac_count >= port_count) {
      return true;
    }
  }
  return false;
}

bool MacDistributor::IsMacAddressAssignmentFull(uint32_t mac_count, uint32_t port_count) const {
  auto max_bridges_with_exclusiv_mac = port_count / 2;
  auto required_macs = port_count + max_bridges_with_exclusiv_mac;

  if (mac_count >= required_macs) {
    return true;
  }
  return false;
}

}
