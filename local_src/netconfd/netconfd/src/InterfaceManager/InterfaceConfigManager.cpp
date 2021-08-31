// SPDX-License-Identifier: GPL-2.0-or-later

#include "InterfaceConfigManager.hpp"
#include <algorithm>
#include <boost/format.hpp>

#include "Logger.hpp"
#include "LinkModeConversion.hpp"

namespace netconf {

using namespace std::literals;

using ::std::copy_if;
using ::std::find_if;
using ::std::inserter;
using ::std::back_inserter;
using ::std::transform;

auto generate_default_port_config = [](const auto &netdev) noexcept {  // NOLINT(cert-err58-cpp): There will nothing be thrown here
  // Create the PortConfig object without the 'eth' prefix.
  return InterfaceConfig { netdev->GetLabel(), InterfaceState::UP, Autonegotiation::ON };
};

InterfaceConfigManager::InterfaceConfigManager(INetDevManager &netdev_manager,
                                               IPersistence<InterfaceConfigs> &persistence_provider,
                                               IEthernetInterfaceFactory &eth_factory)
    : persistence_provider_ { persistence_provider },
      ethernet_interface_factory_ { eth_factory } {

  InterfaceConfigs peristet_configs;
  auto read_persistence_data_status = persistence_provider.Read(peristet_configs);
  if (read_persistence_data_status.IsNotOk()) {
    LogWarning("InterfaceConfigManager: Failed to read persistence data, use default data set");
  }

  auto netdevs = netdev_manager.GetPortNetDevs();

  InitializeEthernetInterfaceMap(netdevs);

  InitializeCurrentConfigs(netdevs, peristet_configs);

  persistence_provider_.Write(current_config_);
}

void InterfaceConfigManager::InitializePorts(InterfaceState initalPortState) {
  auto inital_config = current_config_;
  if(initalPortState != InterfaceState::UNKNOWN)
  {
    for(auto& portcfg: inital_config){
      portcfg.state_ = initalPortState;
    }
  }

  ApplyPortConfigs(inital_config);
}

Status InterfaceConfigManager::Configure(InterfaceConfigs &port_configs) {
  auto status = IsPortConfigValid(port_configs);
  if (status.IsNotOk()) {
    return status;
  }

  UpdateCurrentInterfaceConfigs(port_configs);

  status = ApplyPortConfigs(current_config_);

  if (status.IsNotOk()) {
    InterfaceConfigs old_port_configs;
    ApplyPortConfigs(old_port_configs);
  } else {
    persistence_provider_.Write(current_config_);
  }

  return status;
}

[[gnu::const]]
const InterfaceConfigs& InterfaceConfigManager::GetPortConfigs() {
  return current_config_;
}

static InterfaceState EthDeviceStateToInterfaceState(eth::DeviceState state) {
  return (state == eth::DeviceState::Up) ? InterfaceState::UP : InterfaceState::DOWN;
}

static LinkState EthLinkStateToLinkState(eth::InterfaceLinkState state) {
  return (state == eth::InterfaceLinkState::Up) ? LinkState::UP : LinkState::DOWN;
}

InterfaceStatuses InterfaceConfigManager::GetCurrentPortStatuses() {

  InterfaceStatuses statuses;
  for (auto& [itf, eth_itf] : ethernet_interfaces_) {
    InterfaceStatus itf_status;

    itf_status.device_name_ = itf;

    eth_itf->UpdateConfig();
    auto state = eth_itf->GetState();
    itf_status.state_ = EthDeviceStateToInterfaceState(state);

    itf_status.duplex_ = static_cast<Duplex>(eth_itf->GetDuplex());
    itf_status.speed_ = eth_itf->GetSpeed();

    auto link_state = eth_itf->GetLinkState();
    itf_status.link_state_ = EthLinkStateToLinkState(link_state);

    itf_status.mac_ = eth_itf->GetMac();

    statuses.emplace_back(itf_status);

  }
  return statuses;
}

InterfaceInformation InterfaceConfigManager::GetInterfaceInformation(const NetDev &netdev) const {

  if (netdev.GetKind() == DeviceType::Port && ethernet_interfaces_.count(netdev.GetLabel())) {
    auto &eif = ethernet_interfaces_.at(netdev.GetLabel());
    return InterfaceInformation { netdev.GetName(), netdev.GetLabel(), netdev.GetKind(), netdev.IsIpConfigReadonly(),
        eif->GetAutonegSupport() ? AutonegotiationSupported::YES : AutonegotiationSupported::NO, ToLinkModes(
        eif->GetSupportedLinkModes()) };

  } else {
    return InterfaceInformation { netdev.GetName(), netdev.GetLabel(), netdev.GetKind(), netdev.IsIpConfigReadonly() };
  }

}

void InterfaceConfigManager::InitializeEthernetInterfaceMap(const NetDevs &netdevs) {
  auto create_ethernet_interface = [&](auto netdev) {
    return ::std::make_pair(netdev->GetLabel(), ethernet_interface_factory_.getEthernetInterface(netdev->GetName()));
  };

  transform(netdevs.begin(), netdevs.end(), inserter(ethernet_interfaces_, ethernet_interfaces_.end()),
            create_ethernet_interface);
}

void InterfaceConfigManager::InitializeCurrentConfigs(const NetDevs &netdevs,
                                                      const InterfaceConfigs &persistet_configs) {
  for (auto &netdev : netdevs) {
    current_config_.push_back(InterfaceConfig::DefaultConfig(netdev->GetLabel()));
  }
  UpdateCurrentInterfaceConfigs(persistet_configs);
}

Status InterfaceConfigManager::IsPortConfigValid(const InterfaceConfigs &port_configs) {

  for (auto &port_config : port_configs) {

    if (ethernet_interfaces_.count(port_config.device_name_) == 0) {
      return Status { StatusCode::INTERFACE_NOT_EXISTING, port_config.device_name_ };
    }

    auto& eif = ethernet_interfaces_.at(port_config.device_name_);

    if (port_config.autoneg_ != Autonegotiation::UNKNOWN) {
      auto autoneg_supported = eif->GetAutonegSupport() ? AutonegotiationSupported::YES : AutonegotiationSupported::NO;
      if (port_config.autoneg_ == Autonegotiation::ON) {
        if (autoneg_supported == AutonegotiationSupported::NO) {
          return Status { StatusCode::AUTONEGOTIATION_NOT_SUPPORTED, port_config.device_name_ };
        }
        // if auto negotiation is on we do not have to check the link mode any more
        continue;
      }
    }

    if (port_config.speed_ != InterfaceBase::UNKNOWN_SPEED && port_config.duplex_ != Duplex::UNKNOWN) {
      LinkModes link_modes = ToLinkModes(eif->GetSupportedLinkModes());
      auto link_mode_unsupported = ::std::find(link_modes.begin(), link_modes.end(),
                  LinkMode { port_config.speed_, port_config.duplex_ }) == link_modes.end();

      if (link_mode_unsupported) {
        ::std::string duplex = (port_config.duplex_ == Duplex::HALF) ? "half" : "full";
        return Status {
          StatusCode::LINK_MODE_NOT_SUPPORTED,
          port_config.device_name_, ::std::to_string(port_config.speed_), duplex };
      }
    }
  }

  return Status::Ok();
}

Status InterfaceConfigManager::ApplyPortConfig(InterfaceConfig const &cfg) {

  auto &eif = ethernet_interfaces_.at(cfg.device_name_);
  if (cfg.autoneg_ != Autonegotiation::UNKNOWN) {
    eif->SetAutoneg((cfg.autoneg_ == Autonegotiation::ON) ? eth::Autoneg::On : eth::Autoneg::Off);
  }
  if (cfg.state_ != InterfaceState::UNKNOWN) {
    eif->SetState((cfg.state_ == InterfaceState::UP) ? eth::DeviceState::Up : eth::DeviceState::Down);
  }
  if (cfg.duplex_ != Duplex::UNKNOWN) {
    eif->SetDuplex((cfg.duplex_ == Duplex::FULL) ? eth::Duplex::Full : eth::Duplex::Half);
  }
  if (cfg.speed_ > 0) {
    eif->SetSpeed(cfg.speed_);
  }

  try {
    eif->Commit();
  } catch (std::exception &e) {
    return Status { StatusCode::SET_INTERFACE, cfg.device_name_ };
  }
  return Status { StatusCode::OK };

}

Status InterfaceConfigManager::ApplyPortConfigs(InterfaceConfigs &port_configs) {
  ::std::vector<Status> applyResults;
  std::transform(port_configs.begin(), port_configs.end(), ::std::back_inserter(applyResults),
                 [this](auto &port_config) {
                   return this->ApplyPortConfig(port_config);
                 });

  auto first_negative_status = std::find_if(applyResults.begin(), applyResults.end(), [](auto &status) {
    return status.IsNotOk();
  });

  if (first_negative_status != applyResults.end()) {
    return *first_negative_status;
  }

  return Status { StatusCode::OK };
}

void InterfaceConfigManager::UpdateCurrentInterfaceConfigs(const InterfaceConfigs &port_configs) {

  for (auto &port_config : port_configs) {

    auto device_name_equels = [&](auto &config) {
      return port_config.device_name_ == config.device_name_;
    };
    auto it = ::std::find_if(current_config_.begin(), current_config_.end(), device_name_equels);
    if (it == current_config_.end()) {
      continue;
    }

    if (port_config.state_ != InterfaceState::UNKNOWN) {
      it->state_ = port_config.state_;
    }

    if (port_config.autoneg_ != Autonegotiation::UNKNOWN) {
      it->autoneg_ = port_config.autoneg_;
    }

    if (port_config.duplex_ != Duplex::UNKNOWN) {
      it->duplex_ = port_config.duplex_;
    }

    if (port_config.speed_ == 10 || port_config.speed_ == 100 || port_config.speed_ == 1000) {
      it->speed_ = port_config.speed_;
    }

  }
}

}

/* namespace netconf */

