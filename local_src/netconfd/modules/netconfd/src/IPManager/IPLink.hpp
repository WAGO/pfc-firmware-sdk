// SPDX-License-Identifier: GPL-2.0-or-later
#pragma once

#include <functional>
#include <list>

#include "Types.hpp"
#include "NetworkInterfaceConstants.hpp"

namespace netconf {

class IPLink {
 public:

  IPLink(uint32_t index,const ::std::string& itf_name, ::std::string address = ZeroIP,
         ::std::string netmask = ZeroIP, eth::InterfaceLinkState state = eth::InterfaceLinkState::Down);
  ~IPLink() = default;

  void SetIPConfig(const IPConfig& new_ip_config);
  void SetLinkState(eth::InterfaceLinkState state);
  void SetAddress(const ::std::string &address, const ::std::string &netmask);

  IPConfig GetIPConfig() const;
  IPConfig GetCurrentIPConfig() const;
  uint32_t GetIfIndex() const;
  IPSource GetSource() const;
  eth::InterfaceLinkState GetLinkState() const;
  ::std::string GetName() const;
  ::std::string GetAddress() const;

 private:
  uint32_t index_;
  ::std::string name_;
  ::std::string address_;
  ::std::string netmask_;
  eth::InterfaceLinkState link_state_;
  IPConfig ip_config_;
};

using IPLinks = ::std::list<::std::shared_ptr<IPLink>>;
using IPLinkPtr = ::std::shared_ptr<IPLink>;

}  // namespace netconf
