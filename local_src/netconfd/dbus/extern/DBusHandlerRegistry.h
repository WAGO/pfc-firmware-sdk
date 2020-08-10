// SPDX-License-Identifier: GPL-2.0-or-later
#pragma once

#include <netconfd_dbus.h>

#include <functional>
#include "IDBusHandlerRegistry.h"

namespace netconf {
namespace dbus {

class DBusHandlerRegistry : public IDBusHandlerRegistry {
 public:
  using Getter = ::std::function<::std::string(void)>;
  using GetterStringKey = ::std::function<::std::string(::std::string)>;
  using Setter = ::std::function<int(::std::string)>;

  DBusHandlerRegistry();
  ~DBusHandlerRegistry() override;

  DBusHandlerRegistry(const DBusHandlerRegistry&) = delete;
  DBusHandlerRegistry& operator=(const DBusHandlerRegistry&) = delete;
  DBusHandlerRegistry(DBusHandlerRegistry&&) = delete;
  DBusHandlerRegistry& operator=(DBusHandlerRegistry&&) = delete;

  GDBusObjectSkeleton* GetInterfaceObject() const override;
  GDBusObjectSkeleton* GetIPObject() const override;
  GDBusObjectSkeleton* GetBackupObject() const override;

  void RegisterSetBridgeConfigHandler(Setter&& handler) {
    set_bridge_config_handler_ = std::move(handler);
  }

  void RegisterGetBridgeConfigHandler(Getter&& handler) {
    get_bridge_config_handler_ = std::move(handler);
  }

  void RegisterGetDeviceInterfacesHandler(Getter&& handler) {
    get_device_interfaces_handler_ = ::std::move(handler);
  }

  void RegisterSetInterfaceConfigHandler(Setter&& handler) {
    set_interface_config_handler_ = ::std::move(handler);
  }

  void RegisterGetInterfaceConfigHandler(Getter&& handler) {
    get_interface_config_handler_ = ::std::move(handler);
  }

  void RegisterGetBackupParamCountHandler(Getter&& handler) {
    get_backup_param_count_handler_ = ::std::move(handler);
  }

  void RegisterBackupHandler(::std::function<int(::std::string, ::std::string)>&& handler) {
    backup_handler_ = std::move(handler);
  }

  void RegisterRestoreHandler(Setter&& handler) {
    restore_handler_ = std::move(handler);
  }

  void RegisterSetAllIPConfigsHandler(Setter&& handler) {
    set_all_ip_config_handler_ = std::move(handler);
  }

  void RegisterSetIPConfigHandler(Setter&& handler) {
    set_ip_config_handler_ = std::move(handler);
  }

  void RegisterGetAllIPConfigsHandler(Getter&& handler) {
    get_all_ip_config_handler_ = std::move(handler);
  }

  void RegisterGetAllCurrentIPConfigsHandler(Getter&& handler) {
    get_all_current_ip_config_handler_ = std::move(handler);
  }

  void RegisterGetIPConfigHandler(GetterStringKey&& handler) {
    get_ip_config_handler_ = std::move(handler);
  }

  void RegisterTempFixIpHandler(::std::function<void(void)>&& handler) {
    tempfixip_handler_ = std::move(handler);
  }

  void RegisterGetDipSwitchConfigHandler(Getter&& handler) {
    get_dip_switch_config_handler_ = std::move(handler);
  }

  void RegisterSetDipSwitchConfigHandler(Setter&& handler) {
    set_dip_switch_config_handler_ = std::move(handler);
  }

 private:
  static gboolean SetBridgeConfig(netconfdInterface_config *object,
                                  GDBusMethodInvocation *invocation,
                                  const gchar *arg_config, gpointer user_data);

  static gboolean GetBridgeConfig(netconfdInterface_config *object,
                                  GDBusMethodInvocation *invocation,
                                  gpointer user_data);

  static gboolean GetDeviceInterfaces(netconfdInterface_config *object,
                                      GDBusMethodInvocation *invocation,
                                      gpointer user_data);

  static gboolean SetInterfaceConfig(netconfdInterface_config *object,
                                     GDBusMethodInvocation *invocation,
                                     const gchar *arg_config,
                                     gpointer user_data);

  static gboolean GetInterfaceConfig(netconfdInterface_config *object,
                                     GDBusMethodInvocation *invocation,
                                     gpointer user_data);




  static gboolean SetAllIPConfig(netconfdIp_config *object,
                                 GDBusMethodInvocation *invocation,
                                 const gchar *arg_config, gpointer user_data);

  static gboolean SetIPConfig(netconfdIp_config *object,
                              GDBusMethodInvocation *invocation,
                              const gchar *arg_config, gpointer user_data);

  static gboolean GetAllIPConfig(netconfdIp_config *object,
                                 GDBusMethodInvocation *invocation,
                                 gpointer user_data);

  static gboolean GetAllCurrentIPConfig(netconfdIp_config *object,
                                       GDBusMethodInvocation *invocation,
                                       gpointer user_data);

  static gboolean GetIPConfig(netconfdIp_config *object,
                              GDBusMethodInvocation *invocation,
                              const gchar *arg_config, gpointer user_data);

  static gboolean TemporaryFixIP(netconfdIp_config *object,
                              GDBusMethodInvocation *invocation,
                              gpointer user_data);

  static gboolean GetDipSwitchConfig(netconfdIp_config *object,
                              GDBusMethodInvocation *invocation,
                              gpointer user_data);

  static gboolean SetDipSwitchConfig(netconfdIp_config *object,
                                 GDBusMethodInvocation *invocation,
                                 const gchar *arg_config, gpointer user_data);


  static gboolean GetBackupParamCount(netconfdBackup *object,
                                      GDBusMethodInvocation *invocation,
                                      gpointer user_data);

  static gboolean Backup(netconfdBackup *object,
                         GDBusMethodInvocation *invocation,
                         const gchar *arg_content, const gchar *arg_targetversion , gpointer user_data);

  static gboolean Restore(netconfdBackup *object,
                          GDBusMethodInvocation *invocation,
                          const gchar *arg_config, gpointer user_data);


  //interface_config
  netconfdObjectSkeleton *interface_object_;
  netconfdInterface_config* interface_config_;

  Setter set_bridge_config_handler_;
  Getter get_bridge_config_handler_;
  Getter get_device_interfaces_handler_;
  Setter set_interface_config_handler_;
  Getter get_interface_config_handler_;
  Getter get_backup_param_count_handler_;


  //ip_config
  netconfdObjectSkeleton *ip_object_;
  netconfdIp_config* ip_config_;

  Setter set_all_ip_config_handler_;
  Setter set_ip_config_handler_;
  Getter get_all_ip_config_handler_;
  Getter get_all_current_ip_config_handler_;
  Getter get_dip_switch_config_handler_;
  Setter set_dip_switch_config_handler_;
  GetterStringKey get_ip_config_handler_;
  ::std::function<void(void)> tempfixip_handler_;

  //backup & restore
  netconfdObjectSkeleton *backup_object_;
  netconfdBackup* backup_;
  ::std::function<int(::std::string, ::std::string)> backup_handler_;
  Setter restore_handler_;



};

} /* namespace dbus */
} /* namespace netconf */
