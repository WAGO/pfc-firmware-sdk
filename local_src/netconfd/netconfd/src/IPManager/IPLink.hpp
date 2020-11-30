// SPDX-License-Identifier: GPL-2.0-or-later
#pragma once

#include <functional>

#include "Error.hpp"
#include "IEventManager.hpp"
#include "IIPEvent.hpp"
#include "NetDev.hpp"
#include "Types.hpp"

namespace netconf {

class IPLink;
class IIPConfigure;

using IPLinks = ::std::list<::std::shared_ptr<IPLink>>;

class IPLink {
 public:

  IPLink(IIPConfigure &configurator, IEventManager &event_manager, NetDevPtr netdev, const ::std::string &init_address = ZeroIP, const ::std::string &init_netmask = ZeroIP);
  ~IPLink();

  Error SetIPConfig(const IPConfig new_ip_config);

  IPConfig GetIPConfig() const;
  IPConfig GetCurrentIPConfig() const;
  uint32_t GetIfIndex() const;
  ::std::string GetName() const;

  void Enable();
  void Disable();

  void OnAddressChange(IIPEvent::ChangeType change_type, const ::std::string &address, const ::std::string &netmask);

 private:
  IIPConfigure &ip_configure_;
  IEventManager &event_manager_;
  NetDevPtr netdev_;
  IPConfig ip_config_;
  ::std::string address_;
  ::std::string netmask_;
  bool enabled_;
  bool shall_trigger_event_folder_;

  bool IsDifferentFromCurrentIpAddress(const IPConfig &new_ip_config) const;
  Error Configure(const IPConfig &new_ip_config);
  void AcceptNewConfig(const IPConfig &new_ip_config);
  void NotifyChanges(const IPConfig &new_ip_config);
};

}  // namespace netconf
