// SPDX-License-Identifier: GPL-2.0-or-later

#include "NetworkConfigurator.hpp"
#include <DeviceProperties.hpp>
#include "BridgeManager.hpp"

#include <cerrno>
#include <cstring>
#include <utility>

#include "BridgeController.hpp"
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
#include "Server.h"
#include "UriEscape.hpp"

namespace netconfd {

class NetworkConfiguratorImpl {
 public:
  explicit NetworkConfiguratorImpl();
  virtual ~NetworkConfiguratorImpl() = default;

  NetworkConfiguratorImpl(const NetworkConfiguratorImpl&) = delete;
  NetworkConfiguratorImpl& operator=(const NetworkConfiguratorImpl&) = delete;
  NetworkConfiguratorImpl(const NetworkConfiguratorImpl&&)           = delete;
  NetworkConfiguratorImpl& operator=(const NetworkConfiguratorImpl&&) = delete;

 private:
  const ::std::string persistence_file_path = "/etc/specific";
  const ::std::string DEV_DIP_SWITCH_VALUE  = "/dev/dip-switch/value";

  BridgeController bridge_controller_;

  NetlinkMonitor netlink_monitor_;
  ::std::shared_ptr<IInterfaceMonitor> interface_monitor_;
  ::std::shared_ptr<IIPMonitor> ip_monitor_;
  DeviceProperties device_properties_;
  NetDevManager netdev_manager_;
  DipSwitch ip_dip_switch_;
  PersistenceProvider persistence_provider_;
  EventManager event_manager_;
  JsonConfigConverter json_config_converter_;
  BridgeManager interface_manager_;
  EthernetInterfaceFactory ethernet_interface_factory_;
  InterfaceConfigManager interface_config_manager_;
  IPManager ip_manager_;
  NetworkConfigBrain network_config_brain_;

  dbus::Server dbus_server_;
  dbus::DBusHandlerRegistry dbus_handler_registry_;
  ::UriEscape uri_escape_;
};

NetworkConfiguratorImpl::NetworkConfiguratorImpl()
    : interface_monitor_{static_cast<::std::shared_ptr<IInterfaceMonitor>>(netlink_monitor_.Add<NetlinkLinkCache>())},
      ip_monitor_{static_cast<::std::shared_ptr<IIPMonitor>>(netlink_monitor_.Add<NetlinkAddressCache>())},
      device_properties_{bridge_controller_},
      netdev_manager_{interface_monitor_, bridge_controller_},
      ip_dip_switch_{DEV_DIP_SWITCH_VALUE},
      persistence_provider_{persistence_file_path, device_properties_, ip_dip_switch_},
      event_manager_{device_properties_},
      interface_manager_{bridge_controller_, device_properties_, netdev_manager_},
      interface_config_manager_{netdev_manager_, persistence_provider_, json_config_converter_,
                                ethernet_interface_factory_},
      ip_manager_{device_properties_, interface_manager_, event_manager_, persistence_provider_,
                  netdev_manager_, ip_dip_switch_, ip_monitor_},
      network_config_brain_{interface_manager_, interface_manager_, ip_manager_, event_manager_,
                            device_properties_, json_config_converter_, persistence_provider_, ip_dip_switch_,
                            interface_config_manager_, netdev_manager_} {
  dbus_server_.AddInterface(dbus_handler_registry_);

  dbus_handler_registry_.RegisterSetBridgeConfigHandler([this](std::string config) {
    config = this->uri_escape_.Unescape(config);
    LogDebug("DBUS Req: SetBridgeConfig: " + config);
    auto status = this->network_config_brain_.SetBridgeConfig(config);
    if (status.NotOk()) {
      return 1;
    }
    return 0;
  });

  dbus_handler_registry_.RegisterGetBridgeConfigHandler([this]() -> std::string {
    ::std::string config = this->network_config_brain_.GetBridgeConfig();
    LogDebug("DBUS Req: GetBridgeConfig: " + config);
    return config;
  });

  dbus_handler_registry_.RegisterGetDeviceInterfacesHandler([this]() -> std::string {
    ::std::string interfaces = this->network_config_brain_.GetDeviceInterfaces();
    LogDebug("DBUS Req: GetDeviceInterfaces: " + interfaces);
    return interfaces;
  });

  dbus_handler_registry_.RegisterSetInterfaceConfigHandler([this](std::string config) {
    config = this->uri_escape_.Unescape(config);
    LogDebug("DBUS Req: SetInterfaceConfig " + config);
    auto status = this->network_config_brain_.SetInterfaceConfig(config);
    if (status.NotOk()) {
      return 1;
    }
    return 0;
  });

  dbus_handler_registry_.RegisterGetInterfaceConfigHandler([this]() -> ::std::string {
    LogDebug("DBUS Req: GetInterfaceConfig");
    auto itf_cfg = this->network_config_brain_.GetInterfaceConfig();
    return itf_cfg;
  });

  // Backup and Restore API
  dbus_handler_registry_.RegisterGetBackupParamCountHandler([this]() -> std::string {
    ::std::string count = ::std::to_string(this->network_config_brain_.GetBackupParamterCount());
    LogDebug("DBUS Req: GetBackupParamCount: " + count);
    return count;
  });

  dbus_handler_registry_.RegisterBackupHandler([this](std::string file_path, std::string targetversion) {
    LogDebug("DBUS Req: Backup: " + file_path + "; targetversion=" + targetversion);
    Status status = this->network_config_brain_.Backup(file_path, targetversion);
    if (status.NotOk()) {
      return 1;
    }
    return 0;
  });

  dbus_handler_registry_.RegisterRestoreHandler([this](std::string file_path) {
    LogDebug("DBUS Req: Restore: " + file_path);
    Status status = this->network_config_brain_.Restore(file_path);
    if (status.NotOk()) {
      return 1;
    }
    return 0;
  });

  // IP config
  dbus_handler_registry_.RegisterSetAllIPConfigsHandler([this](std::string config) {
    config = this->uri_escape_.Unescape(config);
    LogDebug("DBUS Req: SetAllIPConfigs : " + config);
    auto status = this->network_config_brain_.SetAllIPConfigs(config);
    if (status.NotOk()) {
      return 1;
    }
    return 0;
  });

  dbus_handler_registry_.RegisterSetIPConfigHandler([this](std::string config) {
    config = this->uri_escape_.Unescape(config);
    LogDebug("DBUS Req: SetIPConfig : " + config);
    auto status = this->network_config_brain_.SetIPConfig(config);
    if (status.NotOk()) {
      return 1;
    }
    return 0;
  });

  dbus_handler_registry_.RegisterGetAllIPConfigsHandler([this]() -> std::string {
    ::std::string config = this->network_config_brain_.GetAllIPConfigs();
    LogDebug("DBUS Req: GetAllIPConfigs: " + config);
    return config;
  });

  dbus_handler_registry_.RegisterGetAllCurrentIPConfigsHandler([this]() -> std::string {
    ::std::string config = this->network_config_brain_.GetAllCurrentIPConfigs();
    LogDebug("DBUS Req: GetAllCurrentIPConfigs: " + config);
    return config;
  });

  dbus_handler_registry_.RegisterGetIPConfigHandler([this](std::string config_in) -> std::string {
    ::std::string config = this->network_config_brain_.GetIPConfig(config_in);
    LogDebug("DBUS Req: GetIPConfig for itf: " + config_in);
    return config;
  });

  dbus_handler_registry_.RegisterTempFixIpHandler([this]() {
    LogDebug("DBUS Req: TempFixIp");
    this->network_config_brain_.TempFixIp();
  });

  dbus_handler_registry_.RegisterGetDipSwitchConfigHandler([this]() -> std::string {
    ::std::string config = this->network_config_brain_.GetDipSwitchConfig();
    LogDebug("DBUS Req: GetDipSwitchConfig");
    return config;
  });

  dbus_handler_registry_.RegisterSetDipSwitchConfigHandler([this](std::string config_in) {
    Status status = this->network_config_brain_.SetDipSwitchConfig(config_in);
    LogDebug("DBUS Req: SetDipSwitchConfig");
    if (status.NotOk()) {
      return 1;
    }
    return 0;
  });

  network_config_brain_.Start();
}

NetworkConfigurator::NetworkConfigurator() {
  try {
    network_configurator_ = ::std::make_unique<NetworkConfiguratorImpl>();
  } catch (std::runtime_error& ex) {
    ::std::string exception_massege(ex.what());
    LogError("NetConf initialization exception: " + exception_massege);
  } catch (...) {
    LogError("NetConf unknown initialization exception");
  }
}

NetworkConfigurator::~NetworkConfigurator() {
  try {
    if (network_configurator_) {
      network_configurator_.reset();
    }
  } catch (...) {
    LogError("NetConf unknown termination exception");
  }
}

}  // namespace netconfd

//---- End of source file ------------------------------------------------------
