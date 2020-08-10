// SPDX-License-Identifier: GPL-2.0-or-later

#include "NetworkHelper.hpp"
#include "IPLink.hpp"
#include "IIPConfigure.hpp"
#include "NetworkInterfaceConstants.hpp"

namespace netconf {

IPLink::IPLink(IIPConfigure &configurator, IEventManager &event_manager, NetDevPtr netdev,
               const ::std::string &init_address, const ::std::string &init_netmask)
    :
    ip_configure_ { configurator },
    event_manager_ { event_manager },
    netdev_ { ::std::move(netdev) },
    address_ { init_address },
    netmask_ { init_netmask } {
  auto on_link_change = [this](NetDev& /*netdev*/, eth::InterfaceLinkState state) {
    if (state == eth::InterfaceLinkState::Up) {
      this->Enable();
    } else {
      this->Disable();
    }
  };
  shall_trigger_event_folder_ = !netdev_->IsIpConfigReadonly();
  netdev_->SetLinkChangeHandler(on_link_change);
  enabled_ = netdev_->GetLinkState() == eth::InterfaceLinkState::Up;
  ip_config_.interface_ = netdev_->GetName();
}

IPLink::~IPLink() {
  netdev_->ClearLinkChangeHandler();

  ip_config_.source_ = IPSource::NONE;
  ip_configure_.Configure(ip_config_);
}

[[gnu::pure]]
bool IPLink::IsDifferentFromCurrentIpAddress(const IPConfig &new_ip_config) const {
  if (address_ == ZeroIP) {
    /* Special case when testing zero IP, empty string is also considered to be zero IP */
    return (new_ip_config.address_ != ZeroIP) && !new_ip_config.address_.empty();
  }
  return (new_ip_config.address_ != address_) || (new_ip_config.netmask_ != netmask_);
}

Status IPLink::Configure(const IPConfig &new_ip_config) {
  Status status;
  if (address_ != ZeroIP) {
    if (IPConfig::SourceChangesToAnyOf(ip_config_, new_ip_config, IPSource::BOOTP, IPSource::DHCP,
                                       IPSource::EXTERNAL)) {
      auto flush_ip = IPConfig(new_ip_config.interface_, IPSource::NONE);
      (void) (ip_configure_.Configure(flush_ip));
      address_ = ZeroIP;
      netmask_ = ZeroIP;
    }
  }
  status = ip_configure_.Configure(new_ip_config);

  if (IPConfig::SourceIsAnyOf(new_ip_config, IPSource::STATIC, IPSource::TEMPORARY) && status.Ok()) {
    /* Manually set current IP, because during system boot no netlink events are send tu us.
     * This will circumvent this issue
     */
    address_ = new_ip_config.address_;
    netmask_ = new_ip_config.netmask_;
  }

  return status;
}

void IPLink::AcceptNewConfig(const IPConfig &new_ip_config) {
  if (new_ip_config.source_ != IPSource::TEMPORARY) {
    ip_config_ = new_ip_config;
  }
}

void IPLink::NotifyChanges(const IPConfig &new_ip_config) {
  auto notify_change = false;

  if (new_ip_config.source_ == IPSource::TEMPORARY) {
    /* IPSource::TEMPORARY is used by DHCP client,
     * notify needed since DHCP server might have send different DNS server addresses, etc. */
    notify_change = true;
  } else if (IPConfig::SourceChangesToAnyOf(ip_config_, new_ip_config, IPSource::BOOTP, IPSource::DHCP,
                                            IPSource::EXTERNAL)) {
    /* Changes to a source where the IP is given through a service always triggers event folder */
    notify_change = true;
  } else if ((new_ip_config.source_ == ip_config_.source_)
      && (IPConfig::SourceIsAnyOf(new_ip_config, IPSource::BOOTP, IPSource::DHCP))) {
    /* When the source is still a client we don't have to trigger right now,
     * the service will do this when setting the IP with TEMPORARY */
    notify_change = false;
  } else {
    notify_change = ((new_ip_config.source_ != ip_config_.source_) || IsDifferentFromCurrentIpAddress(new_ip_config));
  }

  if (notify_change) {
    event_manager_.NotifyNetworkChanges(EventType::USER, EventLayer::EVENT_FOLDER);
  }
}

Status IPLink::SetIPConfig(const IPConfig new_ip_config) {
  Status status = Configure(new_ip_config);

  if(status.Ok()) {
    NotifyChanges(new_ip_config);

    AcceptNewConfig(new_ip_config);
  }

  return status;
}

[[gnu::pure]]
uint32_t IPLink::GetIfIndex() const {
  return netdev_->GetIndex();
}

IPConfig IPLink::GetIPConfig() const {
  return ip_config_;
}

::std::string IPLink::GetName() const {
  return netdev_->GetName();
}

IPConfig IPLink::GetCurrentIPConfig() const {
  IPConfig config = ip_config_;
  config.address_ = address_;
  config.netmask_ = netmask_;
  return config;
}

void IPLink::OnAddressChange(IIPEvent::ChangeType change_type, const ::std::string &address,
                             const ::std::string &netmask) {
  // NETLINK reports address changes twice. The IP that is deleted (del) and the new one (add)

  if (change_type == IIPEvent::ChangeType::Delete) {
    address_ = ZeroIP;
    netmask_ = ZeroIP;
  } else if (address_ != address || netmask_ != netmask) {
    // NETLINK add ip
    address_ = address;
    netmask_ = netmask;
  }
  if (shall_trigger_event_folder_) {
    event_manager_.NotifyNetworkChanges(EventType::SYSTEM, EventLayer::IP_CHANGE_FILES, ip_config_.interface_);
  }
}

void IPLink::Enable() {
  if (enabled_) {
    return;
  }
  enabled_ = true;

  if (ip_config_.source_ != IPSource::EXTERNAL) {
    ip_configure_.Configure(ip_config_);
  }

  if (shall_trigger_event_folder_) {
    event_manager_.NotifyNetworkChanges(EventType::SYSTEM, EventLayer::EVENT_FOLDER);
  }
}

static bool ShouldFlush(const IPConfig &config) {
  return !IPConfig::SourceIsAnyOf(config, IPSource::EXTERNAL, IPSource::STATIC);
}

void IPLink::Disable() {
  if (not enabled_) {
    return;
  }
  enabled_ = false;

  if (ShouldFlush(ip_config_)) {
    IPConfig ip_config = ip_config_;
    ip_config.source_ = IPSource::NONE;
    ip_configure_.Configure(ip_config);
  }

  if (shall_trigger_event_folder_) {
    event_manager_.NotifyNetworkChanges(EventType::SYSTEM, EventLayer::EVENT_FOLDER);
  }
}

}
