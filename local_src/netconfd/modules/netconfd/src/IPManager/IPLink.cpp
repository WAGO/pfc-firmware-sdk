// SPDX-License-Identifier: GPL-2.0-or-later

#include "GratuitousArp.hpp"
#include "NetworkHelper.hpp"
#include "IPLink.hpp"
#include "IIPConfigure.hpp"
#include "NetworkInterfaceConstants.hpp"
#include "boost/algorithm/string.hpp"
#include "Logger.hpp"

namespace netconf {

IPLink::IPLink(uint32_t index,const ::std::string& itf_name, ::std::string address,
               ::std::string netmask, eth::InterfaceLinkState state)
    : index_ { index },
      name_ { itf_name },
      address_ { ::std::move(address) },
      netmask_ { ::std::move(netmask) },
      link_state_ { state } {

  ip_config_.interface_ = itf_name;
}

void IPLink::SetIPConfig(const IPConfig& ip_config) {
  ip_config_ = ip_config;
}

[[gnu::pure]]
uint32_t IPLink::GetIfIndex() const {
  return index_;
}

IPConfig IPLink::GetIPConfig() const {
  return ip_config_;
}

::std::string IPLink::GetName() const {
  return name_;
}

IPConfig IPLink::GetCurrentIPConfig() const {
  IPConfig config = ip_config_;
  config.address_ = address_;
  config.netmask_ = netmask_;
  return config;
}

::std::string IPLink::GetAddress() const {
  return address_;
}

IPSource IPLink::GetSource() const {
  return ip_config_.source_;
}

eth::InterfaceLinkState IPLink::GetLinkState() const {
  return link_state_;
}

void IPLink::SetAddress(const ::std::string &address, const ::std::string &netmask) {
  address_ = address;
  netmask_ = netmask;
}

void IPLink::SetLinkState(eth::InterfaceLinkState state) {
  link_state_ = state;
}

}
