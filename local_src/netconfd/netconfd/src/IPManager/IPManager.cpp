// SPDX-License-Identifier: GPL-2x.0-or-later

#include "TypesHelper.hpp"
#include "IPManager.hpp"
#include <algorithm>
#include "Helper.hpp"
#include "IpAddressManipulator.hpp"
#include "Logger.hpp"
#include "NetDevManager.hpp"
#include "FixIP.hpp"

namespace netconf {

constexpr auto DIP_SWITCH_BRIDGE = "br0";

IPManager::IPManager(const IDeviceProperties &properties_provider, const IBridgeInformation &interface_information,
                     IEventManager &event_manager, IPersistenceProvider &persistence_provider,
                     INetDevManager &netdev_manager, IDipSwitch &ip_dip_switch,
                     ::std::shared_ptr<IIPMonitor> ip_monitor)
    : properties_provider_ { properties_provider },
      interface_information_ { interface_information },
      event_manager_ { event_manager },
      persistence_provider_ { persistence_provider },
      ip_dip_switch_ { ip_dip_switch },
      netdev_manager_ { netdev_manager },
      ip_monitor_ { ip_monitor },
      dhcp_client_controller_ { properties_provider_, file_editor_ },
      ip_configurator_ { ip_controller_, dhcp_client_controller_, bootp_controller_, *this } {
  netdev_manager.RegisterForNetDevConstructionEvents(*this);
  ip_monitor->RegisterEventHandler(*this);

  auto netdevs = netdev_manager.GetNetDevs(SupportedInterfaces);
  std::for_each(netdevs.begin(), netdevs.end(), [this](auto &netdevptr) {
    this->CreateOrGet(netdevptr->GetName());
  });

}

::std::shared_ptr<IPLink> IPManager::CreateOrGet(const ::std::string &interface_name) {
  auto matching_name = [&](::std::shared_ptr<IPLink> ip_link) {
    return interface_name == ip_link->GetName();
  };

  auto ip_link_in_list = ::std::find_if(ip_links_.begin(), ip_links_.end(), matching_name);
  if (ip_link_in_list != ip_links_.end()) {
    return *ip_link_in_list;
  }

  auto netdevptr = netdev_manager_.GetByName(interface_name);
  auto if_index = netdevptr->GetIndex();

  ::std::shared_ptr<IPLink> ip_link =
      ::std::make_shared < IPLink
          > (ip_configurator_, event_manager_, gratuitous_arp_, *this, netdevptr, ip_monitor_->GetIPAddress(if_index), ip_monitor_
              ->GetNetmask(if_index));

  ip_links_.push_back(ip_link);

  return ip_link;
}

::std::shared_ptr<IPLink> IPManager::Get(const ::std::string &interface) {
  auto matching_name = [&](::std::shared_ptr<IPLink> ip_link) {
    return interface == ip_link->GetName();
  };

  auto ip_link_in_list = ::std::find_if(ip_links_.begin(), ip_links_.end(), matching_name);
  if (ip_link_in_list != ip_links_.end()) {
    return *ip_link_in_list;
  }
  return nullptr;
}

Status IPManager::Configure(const IPConfigs &configs) {
  Status status;

  for (auto &ip_config : configs) {
    auto ip_link = CreateOrGet(ip_config.interface_);
    status = ip_link->SetIPConfig(ip_config);

    if (status.IsNotOk()) {
      break;
    }
  }

  return status;
}

static void AddDefaultIPConfigOfMissingBridges(IPConfigs &ip_configs, const Bridges &bridges) {
  ::std::for_each(bridges.begin(), bridges.end(), [&](const auto &bridge) {
    auto it = ::std::find_if(ip_configs.begin(), ip_configs.end(), [&](auto &config) {
      return config.interface_ == bridge;
    });
    if (it == ip_configs.end()) {
      ip_configs.emplace_back(bridge, IPSource::NONE, ZeroIP, ZeroIP);
    }
  });
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

IPConfigs IPManager::QueryAllCurrentIPConfigsThatAreNotIncludetInIPConfigs(const IPConfigs &ip_configs) const {

  IPConfigs configs;
  for (auto ip_link : ip_links_) {
    auto it = ::std::find_if(ip_configs.begin(), ip_configs.end(), [&](IPConfig ip_config) {
      return ip_config.interface_ == ip_link->GetCurrentIPConfig().interface_;
    });
    if (it == ip_configs.end()) {
      auto ip_config = ip_link->GetCurrentIPConfig();
      if (ip_config.address_ != ZeroIP && ip_config.address_ != "") {
        configs.push_back(ip_config);
      }
    }
  }
  return configs;
}

Status IPManager::ValidateIPConfigs(const IPConfigs &configs) const {
  return IPValidator::ValidateIPConfigs(configs);
}

void IPManager::OnNetDevRemoved(NetDevPtr netdev) {
  ip_links_.remove_if([netdev](auto ip_link) {
    return netdev->GetName() == ip_link->GetName();
  });

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

void IPManager::OnNetDevCreated(NetDevPtr netdev) {
  CreateOrGet(netdev->GetName());
}

void IPManager::OnNetDevChangeInterfaceRelations(NetDevPtr netdev) {
  auto ip_link = CreateOrGet(netdev->GetName());
  ip_link->RefreshIP();
}

void IPManager::OnAddressChange(IIPEvent::ChangeType change_type, ::std::uint32_t if_index, ::std::string address,
                                ::std::string netmask) {
  auto find_interface_index_and_set_ip = [&](::std::shared_ptr<IPLink> link) {
    if (link->GetIfIndex() == if_index) {
      link->OnAddressChange(change_type, address, netmask);
      return true;
    }
    return false;
  };

  ::std::find_if(ip_links_.begin(), ip_links_.end(), find_interface_index_and_set_ip);
}

bool IPManager::HasToApplyDipSwitchConfig() const {
  auto mode = ip_dip_switch_.GetMode();
  return (mode != DipSwitchMode::HW_NOT_AVAILABLE && mode != DipSwitchMode::OFF);
}

void IPManager::ModifyIpConfigByDipSwitch(IPConfigs &config, const DipSwitchIpConfig &dip_switch_config) {
  if (!HasToApplyDipSwitchConfig()) {
    return;
  }

  auto it = ::std::find_if(config.begin(), config.end(), [](const IPConfig &cfg) {
    return cfg.interface_ == "br0";
  });

  if (it == config.end() || it->source_ == IPSource::TEMPORARY) {
    return;
  }

  auto mode = ip_dip_switch_.GetMode();
  if (mode == DipSwitchMode::DHCP) {
    it->source_ = IPSource::DHCP;
  } else {
    if (dip_switch_config.IsIncomplete()) {
      it->address_ = DIP_SWITCH_DEFAULT_IP_CONFIG.address_;
      it->netmask_ = DIP_SWITCH_DEFAULT_IP_CONFIG.netmask_;
    } else {
      it->address_ = dip_switch_config.address_;
      it->netmask_ = dip_switch_config.netmask_;
    }
    it->source_ = IPSource::STATIC;
    it->address_ = IpAddressManipulator::ChangeLastAddressOctet(*it, ip_dip_switch_.GetValue()).address_;

    // remove conflicting ip configs on other interfaces
    config.erase(
        ::std::remove_if(
            config.begin(),
            config.end(),
            [this, it](IPConfig ipconfig) {
              return ipconfig.interface_ != "br0" && ipconfig.source_ == IPSource::STATIC
                  && IPValidator::IsSameSubnet(ipconfig, *it);
            })
        , config.end());
  }
}

bool IPManager::HasToBePersisted(const IPConfig &ip_config) const {
  auto netdev = netdev_manager_.GetByName(ip_config.interface_);
  if (netdev && (netdev->GetKind() && persistetDevices)) {
    return (ip_config.source_ != IPSource::TEMPORARY && ip_config.source_ != IPSource::UNKNOWN);
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
  ModifyIpConfigByDipSwitch(applied_config, dip_switch_ip_config);
  Status status = Apply(applied_config);
  if (status.IsOk()) {
    status = Persist(persisted_config);
  }

  return status;
}

Status IPManager::ApplyIpConfiguration(const DipSwitchIpConfig &dip_switch_ip_config) {
  auto applied_ip_config = GetIPConfigs( { DIP_SWITCH_BRIDGE });
  auto persisted_dip_switch_config = dip_switch_ip_config;
  persisted_dip_switch_config.address_ = IpAddressManipulator::ChangeLastAddressOctet(dip_switch_ip_config.address_, 0);
  ModifyIpConfigByDipSwitch(applied_ip_config, persisted_dip_switch_config);
  // Apply ip but do not persist ip configuration.
  // Persist DIP switch IP configuration only.

  Status status = Apply(applied_ip_config);

  if (status.IsOk() && ip_dip_switch_.GetMode() != DipSwitchMode::HW_NOT_AVAILABLE) {
    status = persistence_provider_.Write(persisted_dip_switch_config);
  }

  return status;
}

Status IPManager::ApplyIpConfiguration(const IPConfigs &ip_configs, const DipSwitchIpConfig &dip_switch_ip_config) {
  IPConfigs persisted_ipconfig = ip_configs;
  ComplementNetmasks(persisted_ipconfig);

  auto applied_config = persisted_ipconfig;
  ModifyIpConfigByDipSwitch(applied_config, dip_switch_ip_config);

  Status status = Apply(applied_config);

  if (status.IsOk()) {
    status = Persist(persisted_ipconfig);
  }

  if (status.IsOk() && ip_dip_switch_.GetMode() != DipSwitchMode::HW_NOT_AVAILABLE) {
    status = persistence_provider_.Write(dip_switch_ip_config);
  }

  return status;
}

void IPManager::ClearIpConfiguration() {
  auto ip_configs = GetIPConfigs();
  for (auto& ipcfg: ip_configs) {
    ipcfg = IPConfig{ipcfg.interface_, IPSource::NONE, "", ""};
  }
  Configure(ip_configs);
}

Status IPManager::CheckExistenceAndAccess(const IPConfigs &ip_configs) const {
  Interfaces not_assignable_interfaces = interface_information_.GetBridgeAssignedInterfaces();
  auto status = Status { };
  for (const auto &ip_config : ip_configs) {
    const Interface &interface = ip_config.interface_;
    const auto netdev = netdev_manager_.GetByName(interface);

    if (netdev) {
      if (netdev->IsIpConfigReadonly()) {
        status.Set(StatusCode::IP_CONFIG_READONLY, interface);
      }
    } else {
      status.Set(StatusCode::INTERFACE_NOT_EXISTING, interface);
    }
  }

  return status;

}

Status IPManager::ValidateIPConfigIsApplicableToSystem(const IPConfigs &ip_configs) const {
  IPConfigs remain_ip_configs = QueryAllCurrentIPConfigsThatAreNotIncludetInIPConfigs(ip_configs);
  IPConfigs new_ip_configs = IPValidator::FilterValidStaticAndTemporaryIPConfigs(ip_configs);
  return IPValidator::ValidateCombinabilityOfIPConfigs(new_ip_configs, remain_ip_configs);
}

Status IPManager::Apply(const IPConfigs &config) {
  Status status = IPValidator::ValidateIPConfigs(config);

  if (status.IsOk()) {
    status = CheckExistenceAndAccess(config);
  }

  if (status.IsOk()) {
    status = ValidateIPConfigIsApplicableToSystem(config);
  }

  if (status.IsOk()) {
    status = Configure(config);
  }

  return status;
}

} /* namespace netconf */
