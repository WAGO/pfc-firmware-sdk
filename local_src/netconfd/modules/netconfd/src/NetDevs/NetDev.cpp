// SPDX-License-Identifier: GPL-2.0-or-later



#include "NetDev.hpp"

namespace netconf {

NetDev::NetDev(uint if_index, ::std::string name, ::std::string label, DeviceType kind, bool ip_addressable,
       eth::InterfaceLinkState link_state)
    : if_index_ { if_index },
      name_ { ::std::move(name) },
      label_ { ::std::move(label) },
      kind_ { kind },
      ip_addressable_ { ip_addressable },
      mac_ { "" },
      link_state_ { link_state } {
}

::std::string NetDev::GetName() const {
  return name_;
}

::std::string NetDev::GetLabel() const {
  return label_;
}

uint NetDev::GetIndex() const {
  return if_index_;
}

DeviceType NetDev::GetKind() const {
  return kind_;
}

MacAddress NetDev::GetMac() const {
  return mac_;
}

void NetDev::SetMac(MacAddress mac) {
  mac_ = mac;
}

bool NetDev::IsIpAddressable() const {
  return ip_addressable_;
}

const ::std::list<NetDevPtr>& NetDev::GetChildren() {
  return children_;
}

NetDevPtr NetDev::GetParent() {
  return parent_.lock();
}

void NetDev::SetLinkState(eth::InterfaceLinkState state) {
  link_state_ = state;
}

eth::InterfaceLinkState NetDev::GetLinkState() {
  return link_state_;
}

bool NetDev::IsLinkStateUp() {
  return link_state_ == eth::InterfaceLinkState::Up;
}

std::optional<bool> NetDev::IsAnySiblingUp() {
  if (parent_.lock()) {
    auto children = parent_.lock()->children_;

    for (const auto &child : children) {
      if (child->GetName() != name_) {
        if (child->IsLinkStateUp()) {
          return true;
        }
      }
    }
    return false;
  }
  return ::std::nullopt;

}

void NetDev::RemoveChildren(NetDevPtr& parent) {
  for (auto& child : parent->children_) {
    child->parent_.reset();
  }
  parent->children_.clear();
}

void NetDev::RemoveParent(NetDevPtr& child) {
  NetDevPtr parent = child->parent_.lock();
  RemoveRelation(child, parent);
}

void NetDev::RemoveRelation(NetDevPtr& child, NetDevPtr& parent) {
  if (not child || not parent) {
    return;
  }

  auto childs_parent = child->parent_.lock();
  if (childs_parent == parent) {
    child->parent_.reset();
  }
  parent->children_.remove(child);
}

void NetDev::SetRelation(NetDevPtr& child, NetDevPtr& parent) {
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

}  // namespace netconf
