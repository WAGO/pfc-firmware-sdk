// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include "IIPManager.hpp"

#include <memory>
#include <set>

#include "IPValidator.hpp"
#include "IBridgeInformation.hpp"
#include "BootpClientController.hpp"
#include "DHCPClientController.hpp"
#include "FileEditor.hpp"
#include "IDeviceProperties.hpp"
#include "IDipSwitch.hpp"
#include "IEventManager.hpp"
#include "IIPLinks.hpp"
#include "IIPMonitor.hpp"
#include "INetDevManager.hpp"
#include "IPConfigurator.hpp"
#include "IPController.hpp"
#include "IPLink.hpp"
#include "IPersistenceProvider.hpp"
#include "IIPInformation.hpp"
#include "GratuitousArp.hpp"
#include "INetDevEvents.hpp"
#include "IInterfaceInformation.hpp"

namespace netconf {



class IPManager : public IIPManager, public INetDevEvents, public IIPLinks, public IIPEvent, public IIPInformation {
 public:

  static constexpr DeviceType persistetDevices = DeviceType::Bridge;

  IPManager(const IDeviceProperties &properties_provider,
            IEventManager &event_manager, IPersistenceProvider &persistence_provider, INetDevManager &netdev_manager,
            IDipSwitch &ip_dip_switch, IInterfaceInformation& interface_information, ::std::shared_ptr<IIPMonitor> ip_monitor);
  virtual ~IPManager() = default;

  IPManager(const IPManager &) = delete;
  IPManager &operator=(const IPManager &) = delete;
  IPManager(const IPManager &&)           = delete;
  IPManager &operator=(const IPManager &&) = delete;

  Status Configure(const IPConfigs &config);
  Status ValidateIPConfigs(const IPConfigs &configs) const override;

  IPConfigs GetIPConfigs() const override;
  IPConfigs GetIPConfigs(const Bridges &bridges) const override;
  IPConfigs GetCurrentIPConfigs() const override;

  void OnNetDevCreated(NetDevPtr netdev) override;
  void OnNetDevRemoved(NetDevPtr netdev) override;
  void OnNetDevChangeInterfaceRelations(NetDevPtr netdev) override;
  void OnAddressChange(IIPEvent::ChangeType change_type, ::std::uint32_t if_index, ::std::string address,
                       ::std::string netmask) override;

  ::std::shared_ptr<IPLink> CreateOrGet(const ::std::string &interface_name) override;
  ::std::shared_ptr<IPLink> Get(const ::std::string &interface) override;

  Status ApplyTempFixIpConfiguration() override;
  Status ApplyIpConfiguration(const IPConfigs &config) override;
  Status ApplyDipConfiguration(const DipSwitchIpConfig &dip_switch_ip_config) override;
  Status ApplyIpConfiguration(const IPConfigs &ip_configs, const DipSwitchIpConfig &dip_switch_ip_config) override;
  void ClearIpConfiguration() override;

 private:
  Status ValidateIPConfigIsApplicableToSystem(const IPConfigs &configs) const;
  Status CheckExistenceAndAccess(const IPConfigs &configs) const;

  IPConfigs QueryAllCurrentIPConfigsThatAreNotIncludetInIPConfigs(const IPConfigs &ip_configs) const;

  Status Apply(const IPConfigs &config);
  Status Persist(const IPConfigs &config);
  bool HasToBePersisted(const IPConfig &ip_config) const;

  const IDeviceProperties &properties_provider_;
  IEventManager &event_manager_;
  IPersistenceProvider &persistence_provider_;
  const IDipSwitch &ip_dip_switch_;
  INetDevManager &netdev_manager_;
  IInterfaceInformation& interface_information_;
  ::std::shared_ptr<IIPMonitor> ip_monitor_;
  FileEditor file_editor_;
  DHCPClientController dhcp_client_controller_;
  BootpClientController bootp_controller_;
  IPController ip_controller_;
  IPConfigurator ip_configurator_;
  GratuitousArp gratuitous_arp_;
  IPLinks ip_links_;

};

} /* namespace netconf */
