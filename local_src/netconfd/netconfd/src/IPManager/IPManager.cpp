// SPDX-License-Identifier: GPL-2.0-or-later

#include "IPManager.hpp"
#include <algorithm>
#include "Helper.hpp"
#include "IpAddressManipulator.hpp"
#include "Logger.hpp"
#include "TypesHelper.hpp"

namespace netconfd {

IPManager::IPManager(const IDeviceProperties &properties_provider, const IInterfaceInformation &interface_information,
                     IEventManager &event_manager, IPersistenceProvider &persistence_provider,
                     INetDevManager &netdev_manager, IDipSwitch &ip_dip_switch,
                     ::std::shared_ptr<IIPMonitor> ip_monitor)
    :
    properties_provider_ { properties_provider },
    interface_information_ { interface_information },
    event_manager_ { event_manager },
    persistence_provider_ { persistence_provider },
    ip_dip_switch_ { ip_dip_switch },
    netdev_manager_ { netdev_manager },
    ip_monitor_ { ip_monitor },
    dhcp_client_controller_ { properties_provider_, file_editor_ },
    ip_configurator_ { ip_controller_, dhcp_client_controller_, bootp_controller_, interface_information_, *this },
    ip_validator_ { ip_controller_ } {
  netdev_manager.RegisterForNetDevConstructionEvents(*this);
  ip_monitor->RegisterEventHandler(*this);
}

::std::shared_ptr<IPLink> IPManager::CreateOrGet(const IPConfig &ip_config) {
  auto matching_name = [&](::std::shared_ptr<IPLink> ip_link) {
    return ip_config.interface_ == ip_link->GetName();
  };

  auto ip_link_in_list = ::std::find_if(ip_links_.begin(), ip_links_.end(), matching_name);
  if (ip_link_in_list != ip_links_.end()) {
    return *ip_link_in_list;
  }

  auto netdevptr = netdev_manager_.GetByName(ip_config.interface_);
  auto if_index = netdevptr->GetIndex();

  ::std::shared_ptr<IPLink> ip_link = ::std::make_shared<IPLink>(ip_configurator_, event_manager_, netdevptr,
                                                                 ip_monitor_->GetIPAddress(if_index),
                                                                 ip_monitor_->GetNetmask(if_index));

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
    auto ip_link = CreateOrGet(ip_config);
    status = ip_link->SetIPConfig(ip_config);

    if (status.NotOk()) {
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
      ip_configs.emplace_back(bridge, IPSource::NONE, ZeroIP, ZeroIP, ZeroIP);
    }
  });
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

IPConfigs IPManager::GetCurrentIPConfigs(const Bridges &bridges) const {
  IPConfigs configs;
  auto get_ip_configs_by_bridge_name = [&](::std::shared_ptr<IPLink> link) {
    if (IsIncluded(link->GetName(), bridges)) {
      configs.push_back(link->GetCurrentIPConfig());
    }
  };
  ::std::for_each(ip_links_.begin(), ip_links_.end(), get_ip_configs_by_bridge_name);

  AddDefaultIPConfigOfMissingBridges(configs, bridges);

  return configs;
}

IPConfigs IPManager::GetIPConfigs() const {
  IPConfigs ip_configs;

  for (auto &link : ip_links_) {
    ip_configs.emplace_back(link->GetIPConfig());
  }
  return ip_configs;
}

bool IPManager::IsApplicableToSystem(const IPConfigs &configs) const {
  Interfaces not_assignable_interfaces = interface_information_.GetBridgeAssignedInterfaces();
  const auto &netdevs = netdev_manager_.GetNetDevs();

  return ip_validator_.IsInterfaceApplicableToSystem(configs, netdevs, not_assignable_interfaces);
}

Status IPManager::ValidateIPConfigs(const IPConfigs &configs, const bool interference_has_to_be_checked) const {
  return ip_validator_.ValidateIPConfigs(configs, interference_has_to_be_checked);
}

Status IPManager::ValidateIPConfigIsApplicableToSystem(const IPConfigs &configs) const {
  const auto &eth_itfs = netdev_manager_.GetNetDevs();

  return ip_validator_.ValidateIPConfigIsApplicableToSystem(configs, eth_itfs);
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

  if (status.Ok()) {
    status = persistence_provider_.Write(configs);
  }
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

void IPManager::ModifyIpConfigByDipSwitch(IPConfigs &ip_configs) {
  DipSwitchIpConfig dip_switch_ip_config;
  Status status = persistence_provider_.Read(dip_switch_ip_config);

  ModifyIpConfigByDipSwitch(ip_configs, dip_switch_ip_config);
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
    it->broadcast_ = "";

    // remove conflicting ip configs on other interfaces
    config.erase(::std::remove_if(config.begin(), config.end(), [this, it] (IPConfig ipconfig) {
      return ipconfig.interface_ != "br0" &&
            ipconfig.source_ == IPSource::STATIC &&
            this->ip_validator_.IsSameSubnet(ipconfig, *it);
    }), config.end());
  }
}

bool IPManager::HasToBePersisted(const IPConfig &ip_config) const {
  if (ip_config.interface_ == "wwan0") {
    return false;
  }

  return (ip_config.source_ != IPSource::TEMPORARY && ip_config.source_ != IPSource::UNKNOWN);
}

Status IPManager::Persist(const IPConfigs &config) {
  IPConfigs new_config;
  persistence_provider_.Read(new_config);

  for (auto &config_entry : config) {
    if (HasToBePersisted(config_entry)) {
      AddIPConfig(config_entry, new_config);
    }
  }

  Status status = persistence_provider_.Write(new_config);

  if (status.NotOk()) {
    status.Prepend("Failed to persist the ip configuration: ");
  }

  return status;
}

Status IPManager::ApplyTempFixIpConfiguration(const IPConfigs &config) {
  DipSwitchIpConfig dip;
  const auto& bridges = netdev_manager_.GetBridgeNetDevs();
  IPConfigs ip_configs;
  for(const auto& netdev: bridges) {
    ip_configs.emplace_back(netdev->GetName(), IPSource::NONE);
  }

  auto status = Apply(ip_configs, dip);
  if (status.Ok()) {
    status = Apply(config, dip);
  }

  return status;
}

Status IPManager::ApplyIpConfiguration(const IPConfigs &config) {
  DipSwitchIpConfig dip_switch_ip_config;
  persistence_provider_.Read(dip_switch_ip_config);

  // Apply and persist IP-Configuration
  IPConfigs new_config = config;
  ComplementNetmasks(new_config);
  Status status = Apply(new_config, dip_switch_ip_config);
  if (status.Ok()) {
    status = Persist(new_config);
  }

  return status;
}

Status IPManager::ApplyIpConfiguration(const DipSwitchIpConfig &dip_switch_ip_config) {
  auto ip_configs = GetIPConfigs();
  // Apply ip but do not persist ip configuration.
  // Persist dip switch ip configuration only.

  Status status = Apply(ip_configs, dip_switch_ip_config);

  if (status.Ok() && ip_dip_switch_.GetMode() != DipSwitchMode::HW_NOT_AVAILABLE) {
    status = persistence_provider_.Write(dip_switch_ip_config);
  }

  return status;
}

Status IPManager::ApplyIpConfiguration(const IPConfigs &ip_configs, const DipSwitchIpConfig &dip_switch_ip_config) {
  IPConfigs new_config = ip_configs;
  ComplementNetmasks(new_config);
  Status status = Apply(new_config, dip_switch_ip_config);

  if (status.Ok()) {
    status = Persist(new_config);
  }

  if (status.Ok() && ip_dip_switch_.GetMode() != DipSwitchMode::HW_NOT_AVAILABLE) {
    status = persistence_provider_.Write(dip_switch_ip_config);
  }

  return status;
}

Status IPManager::Apply(const IPConfigs &config, const DipSwitchIpConfig &dip_switch_ip_config) {
  auto configs = config;

  ModifyIpConfigByDipSwitch(configs, dip_switch_ip_config);

  Status status = ValidateIPConfigs(configs, true);

  if (status.Ok()) {
    if (!IsApplicableToSystem(configs)) {
      status.Append("The ip configuration is not applicable to this system");
    }
  }

  if (status.Ok()) {
    status = ValidateIPConfigIsApplicableToSystem(configs);
  }

  if (status.Ok()) {
    status = Configure(configs);
  }

  if (status.NotOk()) {
    status.Prepend("Failed to apply the ip configuration: ");
  }

  return status;
}

} /* namespace netconfd */
