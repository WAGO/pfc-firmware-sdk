// SPDX-License-Identifier: GPL-2.0-or-later

#include "BridgeConfig.hpp"
#include <nlohmann/json.hpp>
#include "JsonParse.hpp"

namespace netconf {

using nlohmann::json;

static void RemoveInterface(json &j, const ::std::string &interface_name) {
  for (auto &bridge : j) {
    auto it = ::std::find(bridge.begin(), bridge.end(), interface_name);
    if (it != bridge.end()) {
      bridge.erase(it);
      break;
    }
  }
}

class BridgeConfig::Impl {
 public:
  Impl() = default;
  explicit Impl(const ::std::string &jsonstr) {
    json_ = JsonToNJson(jsonstr);
  }
  nlohmann::json json_;
};

BridgeConfig::BridgeConfig()
    : impl_ { new Impl { } } {
}

BridgeConfig::BridgeConfig(const ::std::string &jsonstr)
    : impl_ { new Impl { jsonstr } } {
}

BridgeConfig::BridgeConfig(const BridgeConfig &other) noexcept
    : impl_ { new Impl { *other.impl_ } } {
}

BridgeConfig::BridgeConfig(BridgeConfig &&other) noexcept
    : impl_ { std::move(other.impl_) } {
  other.impl_ = std::make_unique<Impl>();
}

BridgeConfig& BridgeConfig::operator=(const BridgeConfig &other) {
  impl_ = std::make_unique < Impl > (*(other.impl_));
  return *this;
}

BridgeConfig& BridgeConfig::operator=(BridgeConfig &&other) noexcept
{
  impl_ = std::move(other.impl_);
  other.impl_ = std::make_unique<Impl>();
  return *this;
}

BridgeConfig::~BridgeConfig() = default;

void BridgeConfig::AddBridge(const ::std::string &bridge_name) {
  auto &j = impl_->json_;
  // find an entry
  if (not j.contains(bridge_name)) {
    j[bridge_name] = json::array();
  }
}

void BridgeConfig::DeleteBridge(const ::std::string &bridge_name) {
  impl_->json_.erase(bridge_name);
}

void BridgeConfig::AssignInterfaceToBridge(const ::std::string &interface_name, const ::std::string &bridge_name) {
  auto &j = impl_->json_;
  RemoveInterface(j, interface_name);

  if (j.contains(bridge_name)) {
    j[bridge_name].push_back(interface_name);
  }
}

void BridgeConfig::DeleteInterface(const ::std::string &interface_name) {
  auto &j = impl_->json_;
  RemoveInterface(j, interface_name);
}

::std::string BridgeConfig::GetBridgeInterfaces(const ::std::string &bridge_name) const {
  ::std::string interfaces;

  auto &j = impl_->json_;
  for (auto &bridge : j.items()) {
    if (bridge.key() == bridge_name) {
      auto &itfs = bridge.value();
      for (const auto &itf : itfs.items()) {
        interfaces.append(itf.value());
        interfaces.append(",");
      }
      interfaces.pop_back();
    }
  }

  return interfaces;
}

::std::string BridgeConfig::GetBridgeOfInterface(const ::std::string &interface_name) const {
  auto &j = impl_->json_;
  for (auto &bridge : j.items()) {
    auto &interface_array = bridge.value();
    auto it = ::std::find(interface_array.begin(), interface_array.end(), interface_name);
    if (it != interface_array.end()) {
      return bridge.key();
    }
  }
  return "";
}

bool BridgeConfig::BridgeIsEmpty(const ::std::string &bridge_name) const {
  if (impl_->json_.count(bridge_name) > 0) {
    return impl_->json_.at(bridge_name).empty();
  }
  return true;
}

::std::string BridgeConfig::ToJson() const {
  return impl_->json_.dump();
}

::std::string BridgeConfig::ToString() const {
  ::std::string config;
  for (const auto &bridge : impl_->json_.items()) {

    config.append(bridge.key());
    config.append("=");

    auto itfs = bridge.value();
    for (const auto &interface : itfs.items()) {
      config.append(interface.value());
      config.append(",");
    }
    config.pop_back();

    config.append(" ");
  }
  config.pop_back();

  return config;
}

} /* namespace netconf */
