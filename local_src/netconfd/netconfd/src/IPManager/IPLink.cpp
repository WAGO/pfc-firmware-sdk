// SPDX-License-Identifier: GPL-2.0-or-later

#include "GratuitousArp.hpp"
#include "NetworkHelper.hpp"
#include "IPLink.hpp"
#include "IIPConfigure.hpp"
#include "NetworkInterfaceConstants.hpp"
#include "boost/algorithm/string.hpp"
#include "Logger.hpp"

#include <chrono>
#include <thread>

#include <unistd.h>

namespace netconf {

IPLink::IPLink(IIPConfigure &configurator, IEventManager &event_manager, IGratuitousArp &gratuitous_arp,
               IIPLinks &ip_links, NetDevPtr netdev, const ::std::string &init_address,
               const ::std::string &init_netmask)
    : ip_configure_ { configurator },
      event_manager_ { event_manager },
      gratuitous_arp_ { gratuitous_arp },
      ip_links_ { ip_links },
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
  return (new_ip_config.address_ != ip_config_.address_) || (new_ip_config.netmask_ != ip_config_.netmask_);
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

  if (status.IsOk()) {
    if (IPConfig::SourceIsAnyOf(new_ip_config, IPSource::STATIC, IPSource::TEMPORARY)) {
      /* Manually set current IP, because during system boot no netlink events are send.
       * This will circumvent this issue
       */
      address_ = new_ip_config.address_;
      netmask_ = new_ip_config.netmask_;

      SendGratuitousArpOnBridge();

    } else if (IPConfig::SourceIsAnyOf(new_ip_config, IPSource::NONE)) {
      address_ = ZeroIP;
      netmask_ = ZeroIP;
    }
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

  /**
   * XXX: Avoid setting temporary IP on former 'static' and 'FixIP' config.
   * This happens when DHCP client is stopped by 'static' or 'FixIP' config, but the DHCP script is simultaneously waiting for the temporary IP set!
   * This race cannot be solved adequately without refactoring the whole DHCP lease processing!!!
   */
  if(IPConfig::SourceIsAnyOf(ip_config_, IPSource::STATIC, IPSource::FIXIP) && new_ip_config.source_ == IPSource::TEMPORARY){
    LogWarning("IPLink: setting temporary IP on 'Static' or 'FixIP' config is ignored: " + new_ip_config.interface_);
    return Status::Ok();
  }

  Status status = Configure(new_ip_config);

  if (status.IsOk()) {
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
  } else {
    address_ = address;
    netmask_ = netmask;
  }
  if (shall_trigger_event_folder_) {
    event_manager_.NotifyNetworkChanges(EventType::SYSTEM, EventLayer::IP_CHANGE_FILES, ip_config_.interface_);
  }
}

void IPLink::RefreshIP(){
  if(IPConfig::SourceIsAnyOf(ip_config_, IPSource::BOOTP, IPSource::DHCP)){
    IPConfig ip_config = ip_config_;
    ip_config.source_ = IPSource::NONE;
    ip_configure_.Configure(ip_config);

    NotifyNetworkChanges();

    ip_configure_.Configure(ip_config_);
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

  SendGratuitousArpOnPortIfAssociatedBridgeIsConfigured();

  NotifyNetworkChanges();
}

static bool ShouldFlush(const IPConfig &config) {
  return !IPConfig::SourceIsAnyOf(config, IPSource::EXTERNAL, IPSource::STATIC);
}

void IPLink::NotifyNetworkChanges() {
  if (shall_trigger_event_folder_) {
    event_manager_.NotifyNetworkChanges(EventType::SYSTEM, EventLayer::EVENT_FOLDER);
  }
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

  NotifyNetworkChanges();
}

void IPLink::SendGratuitousArpOnBridge() {
  if (address_ != ZeroIP) {
    gratuitous_arp_.Send(address_, netdev_, netdev_);
  }
}

void IPLink::SendGratuitousArpOnPortIfAssociatedBridgeIsConfigured() {

  if (netdev_->GetKind() == DeviceType::Port) {
    auto br_netdev = netdev_->GetParent();

    if (br_netdev) {
      auto br_ip_link = ip_links_.Get(br_netdev->GetName());

      if (br_ip_link) {
        auto br_ip_address = br_ip_link->address_;

        if (br_ip_address != ZeroIP) {
          usleep(50 * 1000); // TODO: Workaround for PFC-ADV, see BUWA 3901
          gratuitous_arp_.Send(br_ip_address, netdev_->GetParent(), netdev_);
        }
      }
    }
  }
}

}
