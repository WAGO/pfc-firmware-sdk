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

  NetDev()
      :
      if_index_ { 0 },
      ip_config_ro_{true},
      onlinkchange_ { nullptr } {
  }

  NetDev(uint if_index, ::std::string name, DeviceType kind)
      :
      NetDev(if_index, name, name, kind) {
  }

  NetDev(uint if_index, ::std::string name, ::std::string label, DeviceType kind, bool ip_config_ro = false)
      :
      if_index_ { if_index },
      name_ { ::std::move(name) },
      label_ { ::std::move(label) },
      mac_{""},
      kind_ { kind },
      ip_config_ro_{ip_config_ro},
      onlinkchange_ { nullptr } {
  }

  ::std::string GetName() const {
    return name_;
  }

  ::std::string GetLabel() const {
    return label_;
  }

  uint GetIndex() const {
    return if_index_;
  }

  DeviceType GetKind() const {
    return kind_;
  }

  MacAddress GetMac() const {
    return mac_;
  }

  void SetMac(MacAddress mac) {
    mac_ = mac;
  }

  bool IsIpConfigReadonly() const {
    return ip_config_ro_;
  }
  
  void SetIfFlags(::std::uint32_t flags);

  static void RemoveChildren(NetDevPtr parent) {
    for (auto child : parent->children_) {
      child->parent_.reset();
    }
    parent->children_.clear();
  }

  static void RemoveParent(NetDevPtr child) {
    RemoveRelation(child, child->parent_.lock());
  }

  static void RemoveRelation(NetDevPtr child, NetDevPtr parent) {
    if (not child || not parent) {
      return;
    }

    auto childs_parent = child->parent_.lock();
    if (childs_parent == parent) {
      child->parent_.reset();
    }
    parent->children_.remove(child);
  }

  static void SetRelation(NetDevPtr child, NetDevPtr parent) {
    if (not child || not parent) {
      return;
    }

    auto childs_parent = child->parent_.lock();
    if (childs_parent == parent) {
      return;
    }

    if (childs_parent) {
      childs_parent->children_.remove(child);
    }

    child->parent_ = parent;
    if (parent) {
      parent->children_.push_back(child);
    }

  }

  const ::std::list<NetDevPtr>& GetChildren() {
    return children_;
  }

  NetDevPtr GetParent() {
    return parent_.lock();
  }

  void SetLinkChangeHandler(::std::function<void(NetDev&, eth::InterfaceLinkState)> callback) {
    onlinkchange_ = callback;
  }

  void ClearLinkChangeHandler() {
    onlinkchange_ = nullptr;
  }

  eth::InterfaceLinkState GetLinkState() {
    return link_state_;
  }

 private:

  const uint if_index_;
  ::std::string name_;
  ::std::string label_;
  MacAddress mac_;
  DeviceType kind_;
  bool ip_config_ro_;
  ::std::weak_ptr<NetDev> parent_;
  ::std::list<NetDevPtr> children_;

  ::std::uint32_t flags_ = 0;
  eth::InterfaceLinkState link_state_ = eth::InterfaceLinkState::Down;

  ::std::function<void(NetDev&, eth::InterfaceLinkState)> onlinkchange_;
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


