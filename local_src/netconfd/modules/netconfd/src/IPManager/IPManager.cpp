// SPDX-License-Identifier: GPL-2x.0-or-later

#include "IPManager.hpp"

#include <unistd.h>
#include <cassert>
#include <fstream>
#include <algorithm>

#include "TypesHelper.hpp"
#include "CollectionUtils.hpp"
#include "IpAddressManipulator.hpp"
#include "Logger.hpp"
#include "NetDevManager.hpp"
#include "FixIP.hpp"
#include "IpConfigHelper.hpp"
#include "JsonConverter.hpp"
#include "DynamicIPClientFactory.hpp"
#include "DynamicIPEventAction.hpp"

namespace netconf {

namespace {

DynamicIPType IPSourceToDynamicIPClient(IPSource source) {
  DynamicIPType type = DynamicIPType::UNDEFINED;
  if (source == IPSource::DHCP) {
    type = DynamicIPType::DHCP;
  }
  if (source == IPSource::BOOTP) {
    type = DynamicIPType::BOOTP;
  }
  return type;
}

void AddDefaultIPConfigOfMissingBridges(IPConfigs &ip_configs, const Bridges &bridges) {
  ::std::for_each(bridges.begin(), bridges.end(), [&](const auto &bridge) {
    auto it = ::std::find_if(ip_configs.begin(), ip_configs.end(), [&](auto &config) {
      return config.interface_ == bridge;
    });
    if (it == ip_configs.end()) {
      ip_configs.emplace_back(bridge, IPSource::NONE, ZeroIP, ZeroIP);
    }
  });
}

}

IPManager::IPManager(IEventManager &event_manager, IPersistenceProvider &persistence_provider,
                     INetDevManager &netdev_manager, IDipSwitch &ip_dip_switch,
                     IInterfaceInformation &interface_information, IDynamicIPClientAdministrator &dyn_ip_client,
                     IIPController &ip_controller, ::std::shared_ptr<IIPMonitor> &ip_monitor,
                     IHostnameManager &hostname_manager)
    : event_manager_ { event_manager },
      persistence_provider_ { persistence_provider },
      ip_dip_switch_ { ip_dip_switch },
      netdev_manager_ { netdev_manager },
      interface_information_ { interface_information },
      dyn_ip_client_admin_ { dyn_ip_client },
      ip_controller_ { ip_controller },
      ip_monitor_ { ip_monitor },
      hostname_manager_ { hostname_manager } {

  netdev_manager_.RegisterForNetDevConstructionEvents(*this);
  ip_monitor_->RegisterEventHandler(*this);
  hostname_manager_.RegisterIPManager(*this);

  auto netdevs = netdev_manager.GetNetDevs(SupportedInterfaces);
  std::for_each(netdevs.begin(), netdevs.end(), [this](auto &netdevptr) {
    this->OnNetDevCreated(netdevptr);
  });

}

IPLinkPtr IPManager::GetIPLinkByName(const ::std::string &interface_name) const {
  auto matching_name = [&](::std::shared_ptr<IPLink> ip_link) {
    return interface_name == ip_link->GetName();
  };

  auto ip_link = ::std::find_if(ip_links_.begin(), ip_links_.end(), matching_name);
  if (ip_link != ip_links_.end()) {
    return *ip_link;
  }
  return nullptr;
}

IPLinkPtr IPManager::CreateOrGet(const ::std::string &interface_name) {
  auto ip_link = GetIPLinkByName(interface_name);
  if (ip_link) {
    return ip_link;
  }

  auto netdev = netdev_manager_.GetByName(interface_name);
  auto if_index = netdev->GetIndex();

  ip_link = ::std::make_shared<IPLink>(if_index, interface_name, ip_monitor_->GetIPAddress(if_index),
                                       ip_monitor_->GetNetmask(if_index), netdev->GetLinkState());

  ip_links_.push_back(ip_link);

  return ip_link;
}

void IPManager::ConfigureDynamicIPClient(IPLinkPtr &link) {

  auto client = dyn_ip_client_admin_.GetClient(link->GetName());
  if (client) {
    // 1) if the desired client exists we do nothing and return.
    if (client->GetType() == IPSourceToDynamicIPClient(link->GetSource())) {
      return;
    }
    // 2) if another client exists or IpSource is static e.g. we terminate it.
    if (client->GetType() != IPSourceToDynamicIPClient(link->GetSource())) {
      hostname_manager_.OnLeaseFileRemove(link->GetName());
      dyn_ip_client_admin_.DeleteClient(link->GetName());
      ip_controller_.Flush(link->GetName());
    }
  }

  auto source = link->GetSource();
  if (source == IPSource::DHCP) {
    ip_controller_.Flush(link->GetName());
    dyn_ip_client_admin_.AddClient(DynamicIPType::DHCP, link->GetName(), hostname_manager_.GetHostname());
  }
  if (source == IPSource::BOOTP) {
    ip_controller_.Flush(link->GetName());
    dyn_ip_client_admin_.AddClient(DynamicIPType::BOOTP, link->GetName(), hostname_manager_.GetHostname());
  }

}

Status IPManager::ConfigureIP(const IPLinkPtr &link) const {

  Status status;

  if (IPConfig::SourceIsAnyOf(link->GetIPConfig(), IPSource::STATIC, IPSource::FIXIP)) {
    if(link->GetLinkState() == eth::InterfaceLinkState::Up){
      status = ip_controller_.Configure(link->GetIPConfig());
    }
  }
  if (IPConfig::SourceIsAnyOf(link->GetIPConfig(), IPSource::NONE, IPSource::EXTERNAL)) {
    ip_controller_.Flush(link->GetIPConfig().interface_);
  }

  return status;

}

Status IPManager::Configure(const IPConfigs &configs) {
  Status status;

  for (auto &ip_config : configs) {
    auto ip_link = CreateOrGet(ip_config.interface_);

    ip_link->SetIPConfig(ip_config);
    gratuitous_arp_.EnableGratuitousArp(ip_link);
    ConfigureDynamicIPClient(ip_link);
    ConfigureIP(ip_link);

  }

  return status;
}

IPConfigs IPManager::GetIPConfigs() const {
  IPConfigs ip_configs;
  ip_configs.reserve(ip_links_.size());

  for (auto &link : ip_links_) {
    ip_configs.emplace_back(link->GetIPConfig());
  }
  return ip_configs;
}

IPConfigs IPManager::GetIPConfigs(const Bridges &bridges) const {
  IPConfigs configs;
  auto get_ip_configs_by_bridge_name = [&](::std::shared_ptr<IPLink> link) {
    if (IsIncluded(link->GetName(), bridges)) {
      configs.push_back(link->GetIPConfig());
    }
  };
  ::std::for_each(ip_links_.begin(), ip_links_.end(), get_ip_configs_by_bridge_name);

  AddDefaultIPConfigOfMissingBridges(configs, bridges);

  return configs;
}

IPConfigs IPManager::GetCurrentIPConfigs() const {
  IPConfigs ip_configs;
  ip_configs.reserve(ip_links_.size());
  for (auto &link : ip_links_) {
    ip_configs.emplace_back(link->GetCurrentIPConfig());
  }

  return ip_configs;
}

Status IPManager::ValidateIPConfigs(const IPConfigs &configs) const {
  return IPValidator::ValidateIPConfigs(configs);
}

void IPManager::OnNetDevCreated(NetDevPtr netdev) {
  CreateOrGet(netdev->GetName());
}

void IPManager::OnNetDevRemoved(NetDevPtr netdev) {

  auto it_link = ::std::find_if(ip_links_.begin(), ip_links_.end(), [&](auto &link) {
    return netdev->GetName() == link->GetName();
  });

  if (it_link != ip_links_.end()) {
    hostname_manager_.OnLeaseFileRemove((*it_link)->GetName());
    dyn_ip_client_admin_.DeleteClient((*it_link)->GetName());
    ip_links_.erase(it_link);
  }

  IPConfigs configs;
  Status status = persistence_provider_.Read(configs);

  auto it = ::std::find_if(configs.begin(), configs.end(), [&](IPConfig config) {
    return netdev->GetName() == config.interface_;
  });
  if (it != configs.end()) {
    configs.erase(it);
  }

  if (status.IsOk()) {
    status = persistence_provider_.Write(configs);
  }
}

void IPManager::OnLinkChange(NetDevPtr netdev) {

  auto link = CreateOrGet(netdev->GetName());
  if (link->GetIPConfig().source_ == IPSource::EXTERNAL) {
    return;
  }

  auto state = netdev->GetLinkState();
  link->SetLinkState(state);

  if (netdev->GetKind() == DeviceType::Bridge) {
    if (state == eth::InterfaceLinkState::Up) {

      gratuitous_arp_.EnableGratuitousArp(link);

      auto client = dyn_ip_client_admin_.GetClient(link->GetName());
      if (client) {
        client->Renew();
      }
      ConfigureIP(link);
    } else {
      auto client = dyn_ip_client_admin_.GetClient(link->GetName());
      if (client) {
        client->Release();
      }
      ip_controller_.Flush(link->GetName());
    }
  }

  //Port
  if (netdev->GetKind() == DeviceType::Port) {
    auto br_netdev = netdev->GetParent();
    if (br_netdev && br_netdev->GetLinkState() == eth::InterfaceLinkState::Up) {
      auto client = dyn_ip_client_admin_.GetClient(br_netdev->GetName());
      if (client) {
        auto any_up = netdev->IsAnySiblingUp();
        if (any_up.value_or(false)) {
          client->Renew();
        }
      }
    }
  }

  if (state == eth::InterfaceLinkState::Up) {
    if (netdev->GetKind() == DeviceType::Port) {
      auto bridge_netdev = netdev->GetParent();
      if (bridge_netdev) {
        auto br_link = GetIPLinkByName(bridge_netdev->GetName());
        gratuitous_arp_.SendGratuitousArpOnPort(netdev, br_link->GetAddress());
      }
    }
  }
}

void IPManager::OnNetDevChangeInterfaceRelations(NetDevPtr netdev) {
  auto link = CreateOrGet(netdev->GetName());

  if (IPConfig::SourceIsAnyOf(link->GetIPConfig(), IPSource::BOOTP, IPSource::DHCP)) {

    gratuitous_arp_.EnableGratuitousArp(link);

    auto client = dyn_ip_client_admin_.GetClient(link->GetName());
    if (client) {
      client->Renew();
    }
  }
}

void IPManager::OnAddressChange(IIPEvent::ChangeType change_type, ::std::uint32_t if_index, ::std::string address,
                                ::std::string netmask) {
  auto find_interface_index_and_set_ip = [&](::std::shared_ptr<IPLink> link) {
    if (link->GetIfIndex() == if_index) {

      if (change_type == IIPEvent::ChangeType::Delete) {
        link->SetAddress(ZeroIP, ZeroIP);
      } else {
        link->SetAddress(address, netmask);
        auto netdev = netdev_manager_.GetByIfIndex(link->GetIfIndex());
        gratuitous_arp_.SendGratuitousArpOnBridge(netdev, address);
      }

      hostname_manager_.OnInterfaceIPChange();
      event_manager_.NotifyNetworkChanges(EventLayer::IP_CHANGE_FILES, link->GetName());
      event_manager_.NotifyNetworkChanges(EventLayer::EVENT_FOLDER);
      return true;
    }
    return false;
  };

  ::std::find_if(ip_links_.begin(), ip_links_.end(), find_interface_index_and_set_ip);
}

void IPManager::OnDynamicIPEvent(const ::std::string &interface, DynamicIPEventAction action) {

  auto client = dyn_ip_client_admin_.GetClient(interface);
  if (client) {
    switch (action) {
      case DynamicIPEventAction::BOUND: {
        /* bound: This argument is used when udhcpc moves from an unbound, to a bound state. All of the paramaters are set in
         enviromental variables, The script should configure the interface, and set any other relavent parameters (default gateway, dns server,
         etc). */

        client->UpdateContentFromLease();
        ip_controller_.Configure(interface, client->GetAddressFromLease(), client->GetNetmaskFromLease());
      }
        break;
      case DynamicIPEventAction::RELEASE:
        /*deconfig: This argument is used when udhcpc starts, and  when a leases is lost. The script should put the interface in an
         up, but deconfigured state, ie: ifconfig $interface 0.0.0.0.*/

        ip_controller_.Flush(interface);
        break;
      case DynamicIPEventAction::RENEW:
        /*renew: This argument is used when a DHCP lease is renewed. All of the paramaters are set in enviromental variables. This argument is
         used when the interface is already configured, so the IP address, will not change, however, the other DHCP paramaters, such as the
         default gateway, subnet mask, and dns server may change. */
        client->UpdateContentFromLease();
        ip_controller_.Configure(interface, client->GetAddressFromLease(), client->GetNetmaskFromLease());
        break;
      case DynamicIPEventAction::NAK:
        /* nak: This argument is used with udhcpc receives a NAK message. The script with the deconfig argument will be called directly
         afterwards, so no changes to the network interface are neccessary. This hook is provided for purely informational purposes (the
         message option may contain a reason for the NAK)*/

        break;
      default:
        LogError("IPManager::OnDynamicIPEvent IP Event: found unknown action");
        break;
    }
    hostname_manager_.OnLeaseFileChange(interface);
  }

}

void IPManager::OnHostnameChanged() {
  dyn_ip_client_admin_.RestartAllClients(hostname_manager_.GetHostname());
}

bool IPManager::HasToBePersisted(const IPConfig &ip_config) const {
  auto netdev = netdev_manager_.GetByName(ip_config.interface_);
  if (netdev && (netdev->GetKind() && persistetDevices)) {
    return (ip_config.source_ != IPSource::UNKNOWN);
  }
  return false;
}

Status IPManager::Persist(const IPConfigs &config) {
  IPConfigs new_config;
  persistence_provider_.Read(new_config);

  for (auto &config_entry : config) {
    if (HasToBePersisted(config_entry)) {
      AddIPConfig(config_entry, new_config);
    }
  }

  return persistence_provider_.Write(new_config);
}

Status IPManager::ApplyTempFixIpConfiguration() {

  const auto &bridges = netdev_manager_.GetBridgeNetDevs();
  IPConfigs ip_configs;
  for (const auto &netdev : bridges) {
    ip_configs.emplace_back(netdev->GetName(), IPSource::NONE, ZeroIP, ZeroIP);
  }

  SetFixedIps(ip_configs);

  return Configure(ip_configs);
}

Status IPManager::ApplyIpConfiguration(const IPConfigs &config) {
  DipSwitchIpConfig dip_switch_ip_config;
  persistence_provider_.Read(dip_switch_ip_config);

  auto persisted_config = config;
  ComplementNetmasks(persisted_config);
// Apply config combined with DIP setting and persist IP-Configuration
  auto applied_config = persisted_config;
  ModifyIpConfigByDipSwitch(applied_config, dip_switch_ip_config, ip_dip_switch_.GetState());
  Status status = Apply(applied_config);
  if (status.IsOk()) {
    status = Persist(persisted_config);
  }

  return status;
}

Status IPManager::ApplyDipConfiguration(const DipSwitchIpConfig &dip_switch_ip_config) {
  auto applied_ip_config = GetIPConfigs( { DIP_SWITCH_BRIDGE });
  auto persisted_dip_switch_config = dip_switch_ip_config;
  persisted_dip_switch_config.address_ = IpAddressManipulator::ChangeLastAddressOctet(dip_switch_ip_config.address_, 0);
  ModifyIpConfigByDipSwitch(applied_ip_config, persisted_dip_switch_config, ip_dip_switch_.GetState());

// Apply ip but do not persist ip configuration.
// Persist DIP switch IP configuration only.
  Status status = Apply(applied_ip_config);

  if (status.IsOk() && ip_dip_switch_.GetMode() != DipSwitchMode::HW_NOT_AVAILABLE) {
    status = persistence_provider_.Write(persisted_dip_switch_config);
  }

  return status;
}

Status IPManager::ApplyIpConfiguration(const IPConfigs &ip_configs, const DipSwitchIpConfig &dip_switch_ip_config) {
  /*
   * TODO: This Apply is only used for the restore process, it should be renamed.
   * It also should not use the validation path which tests the config against the actual IPs in the system.
   */
  IPConfigs persisted_ipconfig = ip_configs;
  ComplementNetmasks(persisted_ipconfig);

  auto applied_config = persisted_ipconfig;
  ModifyIpConfigByDipSwitch(applied_config, dip_switch_ip_config, ip_dip_switch_.GetState());

  auto itDipIpConfig = GetIpConfigByInterface(applied_config, DIP_SWITCH_BRIDGE);
  if (itDipIpConfig != applied_config.end()) {
    DeactivateIpConfigsWithConflicsTo(*itDipIpConfig, applied_config);
  }
  Status status = Apply(applied_config);

  if (status.IsOk()) {
    status = Persist(persisted_ipconfig);
  }

  if (status.IsOk() && ip_dip_switch_.GetMode() != DipSwitchMode::HW_NOT_AVAILABLE) {
    status = persistence_provider_.Write(dip_switch_ip_config);
  }

  return status;
}

Status IPManager::Apply(const IPConfigs &config) {

  Status status = IPValidator::Validate(config, GetIPConfigs(), interface_information_.GetInterfaceInformations());

  if (status.IsOk()) {
    status = Configure(config);
  }

  return status;
}

} /* namespace netconf */
