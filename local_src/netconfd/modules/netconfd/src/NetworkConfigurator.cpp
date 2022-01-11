// SPDX-License-Identifier: GPL-2.0-or-later

#include <BridgeController.hpp>
#include "NetworkConfigurator.hpp"
#include <DeviceProperties.hpp>
#include "BridgeManager.hpp"

#include <cerrno>
#include <cstring>
#include <utility>

#include "MacController.hpp"
#include "DBusHandlerRegistry.h"
#include "DipSwitch.hpp"
#include "EthernetInterfaceFactory.hpp"
#include "EventManager.hpp"
#include "IPManager.hpp"
#include "Logger.hpp"
#include "NetDevManager.hpp"
#include "NetlinkAddressCache.hpp"
#include "NetlinkLinkCache.hpp"
#include "NetlinkMonitor.hpp"
#include "NetworkConfigBrain.hpp"
#include "PersistenceProvider.hpp"
#include "MacDistributor.hpp"
#include "Server.h"
#include "UriEscape.hpp"
#include "Hostname/HostnameWatcher.hpp"

namespace netconf {

using namespace std::string_literals;

class NetworkConfiguratorImpl {
 public:
  explicit NetworkConfiguratorImpl(InterprocessCondition& start_condition, StartWithPortstate startWithPortState);
  virtual ~NetworkConfiguratorImpl() = default;

  NetworkConfiguratorImpl(const NetworkConfiguratorImpl&) = delete;
  NetworkConfiguratorImpl& operator=(const NetworkConfiguratorImpl&) = delete;
  NetworkConfiguratorImpl(const NetworkConfiguratorImpl&&)           = delete;
  NetworkConfiguratorImpl& operator=(const NetworkConfiguratorImpl&&) = delete;

 private:
  const ::std::string persistence_file_path = "/etc/specific"s;
  const ::std::string DEV_DIP_SWITCH_VALUE  = "/dev/dip-switch/value"s;

  BridgeController bridge_controller_;

  NetlinkMonitor netlink_monitor_;
  ::std::shared_ptr<IInterfaceMonitor> interface_monitor_;
  ::std::shared_ptr<IIPMonitor> ip_monitor_;
  DeviceProperties device_properties_;
  MacController mac_controller_;
  MacDistributor mac_distributor_;
  NetDevManager netdev_manager_;
  DipSwitch ip_dip_switch_;
  PersistenceProvider persistence_provider_;
  EventManager event_manager_;
  BridgeManager bridge_manager_;
  EthernetInterfaceFactory ethernet_interface_factory_;
  InterfaceConfigManager interface_manager_;
  IPManager ip_manager_;
  NetworkConfigBrain network_config_brain_;

  dbus::Server dbus_server_;
  dbus::DBusHandlerRegistry dbus_handler_registry_;
  ::UriEscape uri_escape_;
  HostnameWatcher hostnameWatcher_;
};

NetworkConfiguratorImpl::NetworkConfiguratorImpl(InterprocessCondition& start_condition, StartWithPortstate startWithPortState)
    : interface_monitor_{static_cast<::std::shared_ptr<IInterfaceMonitor>>(netlink_monitor_.Add<NetlinkLinkCache>())},
      ip_monitor_{static_cast<::std::shared_ptr<IIPMonitor>>(netlink_monitor_.Add<NetlinkAddressCache>())},
      device_properties_{bridge_controller_},
      mac_distributor_{device_properties_.GetMac(), device_properties_.GetMacCount(), mac_controller_},
      netdev_manager_{interface_monitor_, bridge_controller_, mac_distributor_},
      ip_dip_switch_{DEV_DIP_SWITCH_VALUE},
      persistence_provider_{persistence_file_path, device_properties_, ip_dip_switch_},
      event_manager_{netdev_manager_},
      bridge_manager_ { bridge_controller_, device_properties_, netdev_manager_ },
      interface_manager_ { netdev_manager_, persistence_provider_, ethernet_interface_factory_ },
      ip_manager_ { device_properties_, event_manager_, persistence_provider_, netdev_manager_,
          ip_dip_switch_,interface_manager_, ip_monitor_ },
      network_config_brain_ { bridge_manager_, bridge_manager_, ip_manager_, event_manager_, device_properties_,
          persistence_provider_, ip_dip_switch_, interface_manager_, netdev_manager_ },
      hostnameWatcher_{[](std::string newhostname){LogDebug("New Hostname: "s + newhostname);}}
{


  event_manager_.RegisterNetworkInformation(bridge_manager_, ip_manager_, interface_manager_);

  dbus_server_.AddInterface(dbus_handler_registry_);

  dbus_handler_registry_.RegisterSetBridgeConfigHandler([this](std::string data) {
    data = this->uri_escape_.Unescape(data);
    LogDebug("DBUS Req: SetBridgeConfig: " + data);
    return this->network_config_brain_.SetBridgeConfig(data);
  });

  dbus_handler_registry_.RegisterGetBridgeConfigHandler([this](std::string& data) -> std::string {
    auto status = this->network_config_brain_.GetBridgeConfig(data);
    LogDebug("DBUS Req: GetBridgeConfig: " + data);
    return status;
  });

  dbus_handler_registry_.RegisterGetDeviceInterfacesHandler([this](std::string& data) -> std::string {
    auto status = this->network_config_brain_.GetInterfaceInformation(data);
    LogDebug("DBUS Req: GetInterfaceInformation: " + data);
    return status;
  });

  dbus_handler_registry_.RegisterSetInterfaceConfigHandler([this](std::string data) {
    data = this->uri_escape_.Unescape(data);
    LogDebug("DBUS Req: SetInterfaceConfig " + data);
    return this->network_config_brain_.SetInterfaceConfig(data);
  });

  dbus_handler_registry_.RegisterGetInterfaceConfigHandler([this](std::string& data) -> ::std::string {
    auto status = this->network_config_brain_.GetInterfaceConfig(data);
    LogDebug("DBUS Req: GetInterfaceConfig" + data);
    return status;
  });

  dbus_handler_registry_.RegisterGetInterfaceStatusesHandler([this](std::string& data) -> ::std::string {
     auto status = this->network_config_brain_.GetInterfaceStatuses(data);
     LogDebug("DBUS Req: GetInterfaceStatuses" + data);
     return status;
   });

  // Backup and Restore API
  dbus_handler_registry_.RegisterGetBackupParamCountHandler([this]() -> std::string {
    auto data = this->network_config_brain_.GetBackupParamterCount();
    LogDebug("DBUS Req: GetBackupParamCount: " + data);
    return data;
  });

  dbus_handler_registry_.RegisterBackupHandler([this](std::string file_path, std::string targetversion) {
    LogDebug("DBUS Req: Backup: " + file_path + "; targetversion=" + targetversion);
    return this->network_config_brain_.Backup(file_path, targetversion);
  });

  dbus_handler_registry_.RegisterRestoreHandler([this](std::string file_path) {
    LogDebug("DBUS Req: Restore: " + file_path);
    return this->network_config_brain_.Restore(file_path);
  });

  // IP config
  dbus_handler_registry_.RegisterSetAllIPConfigsHandler([this](std::string data) {
    data = this->uri_escape_.Unescape(data);
    LogDebug("DBUS Req: SetAllIPConfigs : " + data);
    return this->network_config_brain_.SetAllIPConfigs(data);
  });

  dbus_handler_registry_.RegisterSetIPConfigHandler([this](std::string data) {
    data = this->uri_escape_.Unescape(data);
    LogDebug("DBUS Req: SetIPConfig : " + data);
    return this->network_config_brain_.SetIPConfig(data);
  });

  dbus_handler_registry_.RegisterGetAllIPConfigsHandler([this](std::string& data) -> std::string {
    auto status = this->network_config_brain_.GetAllIPConfigs(data);
    LogDebug("DBUS Req: GetAllIPConfigs: " + data);
    return status;
  });

  dbus_handler_registry_.RegisterGetAllCurrentIPConfigsHandler([this](std::string& data) -> std::string {
    auto status = this->network_config_brain_.GetCurrentIPConfigs(data);
    LogDebug("DBUS Req: GetAllCurrentIPConfigs: " + data);
    return status;
  });

  dbus_handler_registry_.RegisterGetIPConfigHandler([this](std::string data_in, std::string& data) -> std::string {
    auto status = this->network_config_brain_.GetIPConfig(data_in, data);
    LogDebug("DBUS Req: GetIPConfig for itf: " + data_in);
    return status;
  });

  dbus_handler_registry_.RegisterTempFixIpHandler([this]() -> ::std::string {
    LogDebug("DBUS Req: TempFixIp");
    return this->network_config_brain_.TempFixIp();
  });

  dbus_handler_registry_.RegisterGetDipSwitchConfigHandler([this](std::string& data) -> std::string {
    auto status = this->network_config_brain_.GetDipSwitchConfig(data);
    LogDebug("DBUS Req: GetDipSwitchConfig" + data);
    return status;
  });

  dbus_handler_registry_.RegisterSetDipSwitchConfigHandler([this](std::string data_in) {
    auto status = this->network_config_brain_.SetDipSwitchConfig(data_in);
    LogDebug("DBUS Req: SetDipSwitchConfig");
    return status;
  });
  LogInfo("NetworkConfigurator completed DBUS registration");
  start_condition.Notify();

  network_config_brain_.Start(startWithPortState);
}

NetworkConfigurator::NetworkConfigurator(InterprocessCondition& start_condition, StartWithPortstate startWithPortState) {
    network_configurator_ = ::std::make_unique<NetworkConfiguratorImpl>(start_condition, startWithPortState);
}

NetworkConfigurator::~NetworkConfigurator() {
  if (network_configurator_) {
    network_configurator_.reset();
  }
}

}  // namespace netconf

//---- End of source file ------------------------------------------------------
