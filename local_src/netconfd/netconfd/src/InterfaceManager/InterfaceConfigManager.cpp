// SPDX-License-Identifier: GPL-2.0-or-later

#include "InterfaceConfigManager.hpp"
#include <algorithm>
#include <boost/format.hpp>

#include "Logger.hpp"

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
    :
    persistence_provider_ { persistence_provider },
    ethernet_interface_factory_ { eth_factory } {

  InterfaceConfigs peristet_configs;
  auto read_persistence_data_status = persistence_provider.Read(peristet_configs);
  if (read_persistence_data_status.NotOk()) {
    LogWarning("InterfaceConfigManager: Failed to read persistence data, use default data set");
  }

  auto netdevs = netdev_manager.GetPortNetDevs();

  InitializeEthernetInterfaceMap(netdevs);

  InitializeCurrentConfigs(netdevs, peristet_configs);

  persistence_provider_.Write(current_config_);
}

void InterfaceConfigManager::InitializePorts() {
  ApplyPortConfigs(current_config_);
}

Status InterfaceConfigManager::Configure(InterfaceConfigs &port_configs) {
  auto status = IsPortConfigValid(port_configs);
  if (status.NotOk()) {
    return status;
  }

  UpdateCurrentInterfaceConfigs(port_configs);

  status = ApplyPortConfigs(current_config_);

  if (status.NotOk()) {
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
  auto device_does_not_exist = [this](const auto &port_config) {
    return ethernet_interfaces_.count(port_config.device_name_) == 0;
  };

  auto none_existing_entry = find_if(port_configs.begin(), port_configs.end(), device_does_not_exist);
  if (none_existing_entry != port_configs.end()) {
    auto error_message = (::boost::format("Unknown device '%s'") % none_existing_entry->device_name_).str();
    return Status { StatusCode::INVALID_CONFIG, error_message };
  }

  return Status { };
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
    auto error_message = (::boost::format("InterfaceConfigManager: Failed to apply port config for %s: %s")
        % cfg.device_name_ % e.what()).str();
    return Status { StatusCode::ERROR, error_message };
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
    return status.NotOk();
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

