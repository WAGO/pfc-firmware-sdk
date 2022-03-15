// SPDX-License-Identifier: GPL-2.0-or-later
#pragma once

#include <algorithm>
#include <functional>
#include <list>
#include <memory>
#include <string>
#include <sstream>
#include <utility>
#include <vector>
#include "NetworkInterfaceConstants.hpp"
#include "IBridgeController.hpp"
#include "Logger.hpp"

namespace netconf {

class NetDev;

using NetDevPtr = ::std::shared_ptr<NetDev>;
using NetDevs = ::std::vector<NetDevPtr>;

template<typename ... Args>
NetDevPtr MakeNetDev(Args &&... args) {
  return ::std::make_shared<NetDev>(::std::forward<Args>(args)...);
}

class NetDev {

 public:

  NetDev() = delete;
  NetDev(uint if_index, ::std::string name, ::std::string label, DeviceType kind, bool ip_addressable,
         eth::InterfaceLinkState link_state);

  ::std::string GetName() const;
  ::std::string GetLabel() const;
  uint GetIndex() const;
  DeviceType GetKind() const;
  MacAddress GetMac() const;
  NetDevPtr GetParent();
  eth::InterfaceLinkState GetLinkState();
  const ::std::list<NetDevPtr>& GetChildren();

  void SetMac(MacAddress mac);
  void SetLinkState(eth::InterfaceLinkState state);

  bool IsIpAddressable() const;
  bool IsLinkStateUp();
  std::optional<bool> IsAnySiblingUp();

  static void SetRelation(NetDevPtr &child, NetDevPtr &parent);
  static void RemoveChildren(NetDevPtr &parent);
  static void RemoveParent(NetDevPtr &child);
  static void RemoveRelation(NetDevPtr &child, NetDevPtr &parent);

 private:

  const uint if_index_;
  ::std::string name_;
  ::std::string label_;
  DeviceType kind_;
  bool ip_addressable_;
  MacAddress mac_;
  eth::InterfaceLinkState link_state_;
  ::std::weak_ptr<NetDev> parent_;
  ::std::list<NetDevPtr> children_;

};

struct TypeMatches {
  explicit TypeMatches(DeviceType kind)
      : kind_ { kind } {
  }

  bool operator()(const NetDevPtr &netdev) const {
    return kind_ && netdev->GetKind();
  }

  DeviceType kind_;
};

}

