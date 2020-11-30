// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include "IIPManager.hpp"

#include <memory>
#include <set>

#include "../InterfaceManager/IBridgeInformation.hpp"
#include "BootpClientController.hpp"
#include "DHCPClientController.hpp"
#include "FileEditor.hpp"
#include "IDeviceProperties.hpp"
#include "IDipSwitch.hpp"
#include "IEventManager.hpp"
#include "IIPLinks.hpp"
#include "IIPMonitor.hpp"
#include "INetDevConstruction.hpp"
#include "INetDevManager.hpp"
#include "IPConfigurator.hpp"
#include "IPController.hpp"
#include "IPLink.hpp"
#include "IPValidator.hpp"
#include "IPersistenceProvider.hpp"
#include "IIPInformation.hpp"

namespace netconf {

const DipSwitchIpConfig DIP_SWITCH_DEFAULT_IP_CONFIG = DipSwitchIpConfig("192.168.1.0", "255.255.255.0");

class IPManager : public IIPManager, public INetDevConstruction, public IIPLinks, public IIPEvent, public IIPInformation {
 public:

  static constexpr DeviceType persistetDevices = DeviceType::Bridge;

  IPManager(const IDeviceProperties &properties_provider, const IBridgeInformation &interface_information,
            IEventManager &event_manager, IPersistenceProvider &persistence_provider, INetDevManager &netdev_manager,
            IDipSwitch &ip_dip_switch, ::std::shared_ptr<IIPMonitor> ip_monitor);
  virtual ~IPManager() = default;

  IPManager(const IPManager &) = delete;
  IPManager &operator=(const IPManager &) = delete;
  IPManager(const IPManager &&)           = delete;
  IPManager &operator=(const IPManager &&) = delete;

  Error Configure(const IPConfigs &config);
  Error ValidateIPConfigs(const IPConfigs &configs) const override;

  IPConfigs GetIPConfigs() const override;
  IPConfigs GetIPConfigs(const Bridges &bridges) const override;
  IPConfigs GetCurrentIPConfigs() const override;

  void OnNetDevCreated(NetDevPtr netdev) override;
  void OnNetDevRemoved(NetDevPtr netdev) override;
  void OnAddressChange(IIPEvent::ChangeType change_type, ::std::uint32_t if_index, ::std::string address,
                       ::std::string netmask) override;

  ::std::shared_ptr<IPLink> CreateOrGet(const ::std::string &interface_name) override;
  ::std::shared_ptr<IPLink> Get(const ::std::string &interface) override;

  Error ApplyTempFixIpConfiguration(const IPConfigs &config) override;
  Error ApplyIpConfiguration(const IPConfigs &config) override;
  Error ApplyIpConfiguration(const DipSwitchIpConfig &dip_switch_ip_config) override;
  Error ApplyIpConfiguration(const IPConfigs &ip_configs, const DipSwitchIpConfig &dip_switch_ip_config) override;
  void ModifyIpConfigByDipSwitch(IPConfigs &ip_configs);
  void ModifyIpConfigByDipSwitch(IPConfigs &config, const DipSwitchIpConfig &dip_switch_config);
  bool HasToApplyDipSwitchConfig() const;

 private:
  Error ValidateIPConfigIsApplicableToSystem(const IPConfigs &configs) const;
  Error CheckExistenceAndAccess(const IPConfigs &configs) const;

  IPConfigs QueryAllCurrentIPConfigsThatAreNotIncludetInIPConfigs(const IPConfigs &ip_configs) const;

  Error Apply(const IPConfigs &config, const DipSwitchIpConfig &dip_switch_ip_config);
  Error Persist(const IPConfigs &config);
  bool HasToBePersisted(const IPConfig &ip_config) const;

  IPLinks ip_links_;
  const IDeviceProperties &properties_provider_;
  const IBridgeInformation &interface_information_;
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

};

} /* namespace netconf */
