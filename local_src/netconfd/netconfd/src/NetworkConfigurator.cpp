// SPDX-License-Identifier: GPL-2.0-or-later

#include "BridgeManager.hpp"
#include "NetworkConfigurator.hpp"

#include <cerrno>
#include <cstring>
#include <utility>

#include "InterfaceMonitor.hpp"
#include "DBusHandlerRegistry.h"
#include "PersistenceProvider.hpp"
#include "EventManager.hpp"
#include "BridgeController.hpp"
#include "IPManager.hpp"
#include "DevicePropertiesProvider.hpp"
#include "Server.h"
#include "Logger.hpp"
#include "NetworkConfigBrain.hpp"
#include "DipSwitch.hpp"
#include "EthernetInterfaceFactory.hpp"
#include "UriEscape.hpp"


namespace netconfd {

class NetworkConfiguratorImpl {
 public:
  explicit NetworkConfiguratorImpl();
  virtual ~NetworkConfiguratorImpl() = default;

  NetworkConfiguratorImpl(const NetworkConfiguratorImpl&) = delete;
  NetworkConfiguratorImpl& operator=(const NetworkConfiguratorImpl&) = delete;
  NetworkConfiguratorImpl(const NetworkConfiguratorImpl&&) = delete;
  NetworkConfiguratorImpl& operator=(const NetworkConfiguratorImpl&&) = delete;

 private:
  const ::std::string persistence_file_path = "/etc/specific";
  const ::std::string DEV_DIP_SWITCH_VALUE = "/dev/dip-switch/value";

  BridgeController bridge_controller_;

  PersistenceProvider persistence_provider_;
  DevicePropertiesProvider device_properties_provider_;
  EventManager event_manager_;
  JsonConfigConverter json_config_converter_;
  InterfaceMonitor interface_monitor_;
  BridgeManager interface_manager_;
  DipSwitch ip_dip_switch_;
  EthernetInterfaceFactory ethernet_interface_factory_;
  InterfaceConfigManager interface_config_manager_;
  IPManager ip_manager_;
  NetworkConfigBrain network_config_brain_;

  dbus::Server dbus_server_;
  dbus::DBusHandlerRegistry dbus_handler_registry_;
  ::UriEscape uri_escape_;
};

NetworkConfiguratorImpl::NetworkConfiguratorImpl()
    : persistence_provider_ { persistence_file_path },
      device_properties_provider_ { bridge_controller_ },
      event_manager_ { device_properties_provider_ },
      interface_manager_ { bridge_controller_, device_properties_provider_ },
      ip_dip_switch_ { DEV_DIP_SWITCH_VALUE },
      interface_config_manager_ { interface_manager_, persistence_provider_, json_config_converter_ , ethernet_interface_factory_},
      ip_manager_ { device_properties_provider_, interface_manager_ },
      network_config_brain_ { interface_manager_, interface_manager_, ip_manager_, event_manager_,
          device_properties_provider_, json_config_converter_, persistence_provider_, ip_dip_switch_,
          interface_monitor_ , interface_config_manager_} {

  dbus_server_.AddInterface(dbus_handler_registry_);

  dbus_handler_registry_.RegisterSetBridgeConfigHandler([this](std::string config) {
    config = this->uri_escape_.Unescape(config);
    LogDebug("DBUS Req: SetBridgeConfig: " + config);
    auto status = this->network_config_brain_.SetBridgeConfig(config);
    if(status.NotOk()) {
      return 1;
    }
    return 0;
  });

  dbus_handler_registry_.RegisterGetBridgeConfigHandler([this]()->std::string {
    ::std::string config = this->network_config_brain_.GetBridgeConfig();
    LogDebug("DBUS Req: GetBridgeConfig: " + config);
    return config;
  });

  dbus_handler_registry_.RegisterGetDeviceInterfacesHandler([this]()->std::string {
    ::std::string interfaces = this->network_config_brain_.GetDeviceInterfaces();
    LogDebug("DBUS Req: GetDeviceInterfaces: " + interfaces);
    return interfaces;
  });

  dbus_handler_registry_.RegisterSetInterfaceConfigHandler([this](std::string config) {
    config = this->uri_escape_.Unescape(config);
    LogDebug("DBUS Req: SetInterfaceConfig " + config);
    auto status = this->network_config_brain_.SetInterfaceConfig(config);
    if(status.NotOk()) {
      return 1;
    }
    return 0;
  });

  dbus_handler_registry_.RegisterGetInterfaceConfigHandler([this]()->::std::string {
    LogDebug("DBUS Req: GetInterfaceConfig");
    auto itf_cfg = this->network_config_brain_.GetInterfaceConfig();
    return itf_cfg;
  });

  // Backup and Restore API
  dbus_handler_registry_.RegisterGetBackupParamCountHandler([this]()->std::string {
    ::std::string count = ::std::to_string(this->network_config_brain_.GetBackupParamterCount());
    LogDebug("DBUS Req: GetBackupParamCount: " + count);
    return count;
  });

  dbus_handler_registry_.RegisterBackupHandler([this](std::string file_path) {
    LogDebug("DBUS Req: Backup: " + file_path);
    Status status = this->network_config_brain_.Backup(file_path);
    if(status.NotOk()) {
      return 1;
    }
    return 0;
  });

  dbus_handler_registry_.RegisterRestoreHandler([this](std::string file_path) {
    LogDebug("DBUS Req: Restore: " + file_path);
    Status status = this->network_config_brain_.Restore(file_path);
    if(status.NotOk()) {
      return 1;
    }
    return 0;
  });

  //IP config
  dbus_handler_registry_.RegisterSetAllIPConfigsHandler([this](std::string config) {
    config = this->uri_escape_.Unescape(config);
    LogDebug("DBUS Req: SetAllIPConfigs : " + config);
    auto status = this->network_config_brain_.SetAllIPConfigs(config);
    if(status.NotOk()) {
      return 1;
    }
    return 0;
  });

  dbus_handler_registry_.RegisterSetIPConfigHandler([this](std::string config) {
    config = this->uri_escape_.Unescape(config);
    LogDebug("DBUS Req: SetIPConfig : " + config);
    auto status = this->network_config_brain_.SetIPConfig(config);
    if(status.NotOk()) {
      return 1;
    }
    return 0;
  });

  dbus_handler_registry_.RegisterGetAllIPConfigsHandler([this]()->std::string {
    ::std::string config = this->network_config_brain_.GetAllIPConfigs();
    LogDebug("DBUS Req: GetAllIPConfigs: " + config);
    return config;
  });

  dbus_handler_registry_.RegisterGetIPConfigHandler([this](std::string config_in)->std::string {
    ::std::string config = this->network_config_brain_.GetIPConfig(config_in);
    LogDebug("DBUS Req: GetIPConfig for itf: " + config_in);
    return config;
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
