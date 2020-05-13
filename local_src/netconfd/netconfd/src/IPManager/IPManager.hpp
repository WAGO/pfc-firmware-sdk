// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include "IIPManager.hpp"

#include <memory>
#include <set>

#include "BootpClientController.hpp"
#include "DHCPClientController.hpp"
#include "FileEditor.hpp"
#include "IDeviceProperties.hpp"
#include "IDipSwitch.hpp"
#include "IEventManager.hpp"
#include "IIPLinks.hpp"
#include "IIPMonitor.hpp"
#include "IInterfaceInformation.hpp"
#include "INetDevConstruction.hpp"
#include "INetDevManager.hpp"
#include "IPConfigurator.hpp"
#include "IPController.hpp"
#include "IPLink.hpp"
#include "IPValidator.hpp"
#include "IPersistenceProvider.hpp"

namespace netconfd {

const DipSwitchIpConfig DIP_SWITCH_DEFAULT_IP_CONFIG = DipSwitchIpConfig("192.168.1.0", "255.255.255.0");

class IPManager : public IIPManager, public INetDevConstruction, public IIPLinks, public IIPEvent {
 public:
  IPManager(const IDeviceProperties &properties_provider, const IInterfaceInformation &interface_information,
            IEventManager &event_manager, IPersistenceProvider &persistence_provider, INetDevManager &netdev_manager,
            IDipSwitch &ip_dip_switch, ::std::shared_ptr<IIPMonitor> ip_monitor);
  virtual ~IPManager() = default;

  IPManager(const IPManager &) = delete;
  IPManager &operator=(const IPManager &) = delete;
  IPManager(const IPManager &&)           = delete;
  IPManager &operator=(const IPManager &&) = delete;

  Status Configure(const IPConfigs &config);
  bool IsApplicableToSystem(const IPConfigs &configs) const override;
  Status ValidateIPConfigs(const IPConfigs &configs, const bool interference_has_to_be_checked) const override;
  Status ValidateIPConfigIsApplicableToSystem(const IPConfigs &configs) const override;

  IPConfigs GetIPConfigs() const override;
  IPConfigs GetIPConfigs(const Bridges &bridges) const override;
  IPConfigs GetCurrentIPConfigs(const Bridges &bridges) const override;

  void OnNetDevCreated(NetDevPtr /*netdev*/) override{};
  void OnNetDevRemoved(NetDevPtr netdev) override;
  void OnAddressChange(IIPEvent::ChangeType change_type, ::std::uint32_t if_index, ::std::string address,
                       ::std::string netmask) override;

  ::std::shared_ptr<IPLink> CreateOrGet(const IPConfig &ip_config) override;
  ::std::shared_ptr<IPLink> Get(const ::std::string &interface) override;

  Status ApplyTempFixIpConfiguration(const IPConfigs &config) override;
  Status ApplyIpConfiguration(const IPConfigs &config) override;
  Status ApplyIpConfiguration(const DipSwitchIpConfig &dip_switch_ip_config) override;
  Status ApplyIpConfiguration(const IPConfigs &ip_configs, const DipSwitchIpConfig &dip_switch_ip_config) override;
  void ModifyIpConfigByDipSwitch(IPConfigs &ip_configs);
  void ModifyIpConfigByDipSwitch(IPConfigs &config, const DipSwitchIpConfig &dip_switch_config);
  bool HasToApplyDipSwitchConfig() const;

 private:
  IPConfigs GetAllIpConfigs(const Interfaces &interfaces) const;
  Status CompletePartialIPConfig(IPConfigs &configs) const;

  Status Apply(const IPConfigs &config, const DipSwitchIpConfig &dip_switch_ip_config);
  Status Persist(const IPConfigs &config);
  bool HasToBePersisted(const IPConfig &ip_config) const;

  const IDeviceProperties &properties_provider_;
  const IInterfaceInformation &interface_information_;
  IEventManager &event_manager_;
  IPersistenceProvider &persistence_provider_;
  const IDipSwitch &ip_dip_switch_;
  INetDevManager &netdev_manager_;
  ::std::shared_ptr<IIPMonitor> ip_monitor_;
  FileEditor file_editor_;
  DHCPClientController dhcp_client_controller_;
  BootpClientController bootp_controller_;
  IPController ip_controller_;
  IPConfigurator ip_configurator_;
  IPValidator ip_validator_;

  ::std::list<::std::shared_ptr<IPLink>> ip_links_;
};

} /* namespace netconfd */
