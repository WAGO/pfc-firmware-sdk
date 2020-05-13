// SPDX-License-Identifier: GPL-2.0-or-later
//------------------------------------------------------------------------------
///  \file     Types
///
///  \brief    <short description of the file contents>
///
///  \author   <author> : WAGO Kontakttechnik GmbH & Co. KG
//------------------------------------------------------------------------------
#include "Helper.hpp"
#include "TypesHelper.hpp"
#include <algorithm>
#include <boost/asio/ip/address.hpp>
#include <sstream>

namespace netconfd {

::std::string IPSourceToString(const IPSource value) {
  switch (value) {
    case IPSource::STATIC:
      return "static";
    case IPSource::DHCP:
      return "dhcp";
    case IPSource::BOOTP:
      return "bootp";
    case IPSource::TEMPORARY:
      return "temporary";
    case IPSource::NONE:
      return "none";
    default:
      return "unknown";
  }
}

IPSource StringToIPSource(const ::std::string &value) {
  if (value == "static") {
    return IPSource::STATIC;
  }
  if (value == "dhcp") {
    return IPSource::DHCP;
  }
  if (value == "bootp") {
    return IPSource::BOOTP;
  }
  if (value == "temporary") {
    return IPSource::TEMPORARY;
  }
  if (value == "none") {
    return IPSource::NONE;
  }
  return IPSource::UNKNOWN;
}

bool IsEqual(IPConfigs &lhs, IPConfigs &rhs) {
  if (lhs.size() != rhs.size()) {
    return false;
  }

  std::sort(lhs.begin(), lhs.end());
  std::sort(rhs.begin(), rhs.end());

  return std::equal(lhs.cbegin(), lhs.cend(), rhs.cbegin());
}

void AddIPConfig(const IPConfig &ip_config, IPConfigs &ip_configs) {

  auto it = ::std::find_if(ip_configs.begin(), ip_configs.end(), [&ip_config](const IPConfig &arg) {
    return arg.interface_ == ip_config.interface_;
  });
  if (it != ip_configs.cend()) {
    *it.base() = ip_config;
  } else {
    ip_configs.push_back(ip_config);
  }

}

Bridges GetBridgesOfBridgeConfig(const BridgeConfig &bridge_config) {
  Bridges bridges;
  for (auto &bridge : bridge_config) {
    bridges.push_back(bridge.first);
  }
  return bridges;
}

bool ConfigContainsBridgeWithSpecificInterfaces(const BridgeConfig &bridge_config, const Bridge &bridge,
                                                const Interfaces &interfaces) {

  bool contains = false;

  auto it = bridge_config.find(bridge);
  if (it != bridge_config.end()) {

    auto &bridge_interfaces = it->second;

    contains = true;
    for (auto &interface : interfaces) {
      if (IsNotIncluded(interface, bridge_interfaces)) {
        contains = false;
      }
    }

  }

  return contains;
}

/**
 * Removes all IP Configuration who's interface member is not found in Bridges selection.
 */
void IpConfigsIntersection(IPConfigs &ip_configs, const Bridges &selection) {

  auto new_end = std::remove_if(ip_configs.begin(), ip_configs.end(), [&](auto &ip_config) {
    return std::find(selection.begin(), selection.end(), ip_config.interface_) == selection.end();
  });

  ip_configs.erase(new_end, ip_configs.end());
}

bool IsEqual(const BridgeConfig &a, const BridgeConfig &b) {
  return std::is_permutation(
      a.begin(),
      a.end(),
      b.begin(),
      b.end(),
      [](auto &a_, auto &b_) {
        return (a_.first == b_.first)
            && std::is_permutation(a_.second.begin(), a_.second.end(), b_.second.begin(), b_.second.end());
      });
}

IPConfig& ComplementNetmask(IPConfig &ip_config) {
  if ((ip_config.netmask_.empty() || ip_config.netmask_ == ZeroIP)
      && (not ip_config.address_.empty() && ip_config.address_ != ZeroIP)) {
    try {
      auto ipv4 = boost::asio::ip::make_address_v4(ip_config.address_);
      ip_config.netmask_ = boost::asio::ip::address_v4::netmask(ipv4).to_string();
    } catch (...) {
      /* nothing to do here */
    }
  }
  return ip_config;
}

IPConfigs& ComplementNetmasks(IPConfigs &ip_configs) {

  std::for_each(ip_configs.begin(), ip_configs.end(), &ComplementNetmask);
  return ip_configs;
}

::std::string IPConfigToString(const IPConfig &config) {
  ::std::stringstream ss;
  ss << config.interface_ << "," << IPSourceToString(config.source_) << "," << config.address_ << ","
     << config.netmask_;
  return ss.str();
}

void RemoveUnnecessaryIPParameter(IPConfigs &ip_configs) {
  for (auto &config : ip_configs) {
    if (config.source_ == IPSource::DHCP || config.source_ == IPSource::BOOTP || config.source_ == IPSource::EXTERNAL
        || config.source_ == IPSource::NONE) {
      config.address_ = ZeroIP;
      config.netmask_ = ZeroIP;
      config.broadcast_ = ZeroIP;
    } else if (config.source_ == IPSource::STATIC && config.address_ == ZeroIP) {
      config.netmask_ = ZeroIP;
      config.broadcast_ = ZeroIP;
    }
  }
}




} /* namespace netconfd */
