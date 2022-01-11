// SPDX-License-Identifier: GPL-2.0-or-later

#include "DBusHandlerRegistry.h"
#include <cassert>

namespace netconf {
namespace dbus {

using ::std::string;

template <typename SkeletonType, typename CallbackFunc>
void GSignalConnect(SkeletonType* obj, const char * signalName, CallbackFunc &&f, void* data){
  auto handler_id = g_signal_connect(obj, signalName, G_CALLBACK(f), data);
  if (handler_id <= 0) {
    throw ::std::runtime_error("DBusHandlerRegistry: Failed to connect signal");
  }
}

DBusHandlerRegistry::DBusHandlerRegistry() {
  // interface_config
  interface_object_ = netconfd_object_skeleton_new("/de/wago/netconfd/interface_config");
  interface_config_ = netconfd_interface_config_skeleton_new();
  netconfd_object_skeleton_set_interface_config(interface_object_, interface_config_);

  GSignalConnect(interface_config_, "handle-set", SetBridgeConfig, this);

  GSignalConnect(interface_config_, "handle-get", GetBridgeConfig, this);

  GSignalConnect(interface_config_, "handle-getdeviceinterfaces", GetDeviceInterfaces, this);

  GSignalConnect(interface_config_, "handle-setinterfaceconfig", SetInterfaceConfig, this);

  GSignalConnect(interface_config_, "handle-getinterfaceconfig", GetInterfaceConfig, this);

  GSignalConnect(interface_config_, "handle-getinterfacestatuses", GetInterfaceStatuses, this);

  // ip_config
  ip_object_ = netconfd_object_skeleton_new("/de/wago/netconfd/ip_config");
  ip_config_ = netconfd_ip_config_skeleton_new();
  netconfd_object_skeleton_set_ip_config(ip_object_, ip_config_);

  GSignalConnect(ip_config_, "handle-setall", SetAllIPConfig, this);

  GSignalConnect(ip_config_, "handle-set", SetIPConfig, this);

  GSignalConnect(ip_config_, "handle-getall", GetAllIPConfig, this);

  GSignalConnect(ip_config_, "handle-getallcurrent", GetAllCurrentIPConfig, this);

  GSignalConnect(ip_config_, "handle-get", GetIPConfig, this);

  GSignalConnect(ip_config_, "handle-tempfixip", TemporaryFixIP, this);

  GSignalConnect(ip_config_, "handle-getdipswitchconfig", GetDipSwitchConfig, this);

  GSignalConnect(ip_config_, "handle-setdipswitchconfig", SetDipSwitchConfig, this);

  // backup & restore
  backup_object_ = netconfd_object_skeleton_new("/de/wago/netconfd/backup");
  backup_ = netconfd_backup_skeleton_new();
  netconfd_object_skeleton_set_backup(backup_object_, backup_);

  GSignalConnect(backup_, "handle-backup", Backup, this);

  GSignalConnect(backup_, "handle-restore", Restore, this);

  GSignalConnect(backup_, "handle-getbackupparamcount", GetBackupParamCount, this);

}

DBusHandlerRegistry::~DBusHandlerRegistry() {
  g_object_unref(interface_object_);
  g_object_unref(interface_config_);

  g_object_unref(ip_object_);
  g_object_unref(ip_config_);

  g_object_unref(backup_object_);
  g_object_unref(backup_);
}

GDBusObjectSkeleton *DBusHandlerRegistry::GetInterfaceObject() const {
  return G_DBUS_OBJECT_SKELETON(interface_object_);
}

GDBusObjectSkeleton *DBusHandlerRegistry::GetIPObject() const {
  return G_DBUS_OBJECT_SKELETON(ip_object_);
}

GDBusObjectSkeleton* DBusHandlerRegistry::GetBackupObject() const
{
  return G_DBUS_OBJECT_SKELETON(backup_object_);
}

gboolean DBusHandlerRegistry::SetBridgeConfig(netconfdInterface_config *object, GDBusMethodInvocation *invocation,
                                              const gchar *arg_config, gpointer user_data) {
  auto this_ = reinterpret_cast<DBusHandlerRegistry *>(user_data);

  if (this_->set_bridge_config_handler_) {
    auto result = this_->set_bridge_config_handler_(arg_config);
    netconfd_interface_config_complete_set(object, invocation, result.c_str());
  } else {
    g_dbus_method_invocation_return_dbus_error(invocation, "de.wago.netconfd1.interface_config.Error.Set",
                                               "No Handler for Set request");
  }
  return true;
}

gboolean DBusHandlerRegistry::GetBridgeConfig(netconfdInterface_config *object, GDBusMethodInvocation *invocation,
                                              gpointer user_data) {
  auto this_ = reinterpret_cast<DBusHandlerRegistry *>(user_data);

  if (this_->get_bridge_config_handler_) {
    ::std::string data;
    auto result = this_->get_bridge_config_handler_(data);
    netconfd_interface_config_complete_get(object, invocation, data.c_str(), result.c_str());
  } else {
    g_dbus_method_invocation_return_dbus_error(invocation, "de.wago.netconfd1.interface_config.Error.Get",
                                               "No Handler for Get request");
  }
  return true;
}

gboolean DBusHandlerRegistry::GetDeviceInterfaces(netconfdInterface_config *object, GDBusMethodInvocation *invocation,
                                                  gpointer user_data) {
  auto this_ = reinterpret_cast<DBusHandlerRegistry *>(user_data);

  if (this_->get_device_interfaces_handler_) {
    ::std::string data;
    auto result = this_->get_device_interfaces_handler_(data);
    netconfd_interface_config_complete_getdeviceinterfaces(object, invocation, data.c_str(), result.c_str());
  } else {
    g_dbus_method_invocation_return_dbus_error(
        invocation, "de.wago.netconfd1.interface_config.Error.GetDeviceInterfaces", "No Handler for Get request");
  }
  return true;
}

gboolean DBusHandlerRegistry::SetInterfaceConfig(netconfdInterface_config *object, GDBusMethodInvocation *invocation,
                                                 const gchar *arg_config, gpointer user_data) {
  auto this_ = reinterpret_cast<DBusHandlerRegistry *>(user_data);

  if (this_->set_interface_config_handler_) {
    auto result = this_->set_interface_config_handler_(arg_config);
    netconfd_interface_config_complete_setinterfaceconfig(object, invocation, result.c_str());
  } else {
    g_dbus_method_invocation_return_dbus_error(
        invocation, "de.wago.netconfd1.interface_config.Error.SetInterfaceConfig", "No Handler for Get request");
  }
  return true;
}

gboolean DBusHandlerRegistry::GetInterfaceConfig(netconfdInterface_config *object, GDBusMethodInvocation *invocation,
                                                 gpointer user_data) {
  auto this_ = reinterpret_cast<DBusHandlerRegistry *>(user_data);

  if (this_->get_interface_config_handler_) {
    string data;
    auto result = this_->get_interface_config_handler_(data);
    netconfd_interface_config_complete_getinterfaceconfig(object, invocation, data.c_str(), result.c_str());
  } else {
    g_dbus_method_invocation_return_dbus_error(
        invocation, "de.wago.netconfd1.interface_config.Error.GetInterfaceConfig", "No Handler for Get request");
  }
  return true;
}

gboolean DBusHandlerRegistry::GetInterfaceStatuses(netconfdInterface_config *object, GDBusMethodInvocation *invocation,
                                                 gpointer user_data) {
  auto this_ = reinterpret_cast<DBusHandlerRegistry *>(user_data);

  if (this_->get_interface_statuses_handler_) {
    string data;
    auto result = this_->get_interface_statuses_handler_(data);
    netconfd_interface_config_complete_getinterfacestatuses(object, invocation, data.c_str(), result.c_str());
  } else {
    g_dbus_method_invocation_return_dbus_error(
        invocation, "de.wago.netconfd1.interface_config.Error.GetInterfaceStatuses", "No Handler for Get request");
  }
  return true;
}

gboolean DBusHandlerRegistry::GetBackupParamCount(netconfdBackup *object, GDBusMethodInvocation *invocation,
                                                  gpointer user_data) {
  auto this_ = reinterpret_cast<DBusHandlerRegistry *>(user_data);

  if (this_->get_backup_param_count_handler_) {
    auto count = this_->get_backup_param_count_handler_();
    netconfd_backup_complete_getbackupparamcount(object, invocation, count.c_str());
  } else {
    g_dbus_method_invocation_return_dbus_error(
        invocation, "de.wago.netconfd1.interface_config.Error.GetBackupParamCount", "No Handler for Get request");
  }
  return true;
}

gboolean DBusHandlerRegistry::Backup(netconfdBackup *object,
                                     GDBusMethodInvocation *invocation,
                                     const gchar *arg_content, const gchar *arg_targetversion , gpointer user_data) {
  auto this_ = reinterpret_cast<DBusHandlerRegistry *>(user_data);

  if (this_->backup_handler_) {
    auto result = this_->backup_handler_(arg_content, arg_targetversion);
    netconfd_backup_complete_backup(object, invocation, result.c_str());
  } else {
    g_dbus_method_invocation_return_dbus_error(invocation, "de.wago.netconfd1.backup.Error.Backup",
                                               "No Handler for Backup request");
  }
  return true;
}

gboolean DBusHandlerRegistry::Restore(netconfdBackup *object, GDBusMethodInvocation *invocation,
                                      const gchar *arg_config, gpointer user_data) {
  auto this_ = reinterpret_cast<DBusHandlerRegistry *>(user_data);

  if (this_->restore_handler_) {
    auto result = this_->restore_handler_(arg_config);
    netconfd_backup_complete_restore(object, invocation, result.c_str());
  } else {
    g_dbus_method_invocation_return_dbus_error(invocation, "de.wago.netconfd1.interface_config.Error.Restore",
                                               "No Handler for Restore request");
  }
  return true;
}

gboolean DBusHandlerRegistry::SetAllIPConfig(netconfdIp_config *object, GDBusMethodInvocation *invocation,
                                             const gchar *arg_config, gpointer user_data) {
  auto this_ = reinterpret_cast<DBusHandlerRegistry *>(user_data);

  if (this_->set_all_ip_config_handler_) {
    auto result = this_->set_all_ip_config_handler_(arg_config);
    netconfd_ip_config_complete_setall(object, invocation, result.c_str());
  } else {
    g_dbus_method_invocation_return_dbus_error(invocation, "de.wago.netconfd1.ip_config.Error.SetAll",
                                               "No Handler for SetAll request");
  }
  return true;
}

gboolean DBusHandlerRegistry::SetIPConfig(netconfdIp_config *object, GDBusMethodInvocation *invocation,
                                          const gchar *arg_config, gpointer user_data) {
  auto this_ = reinterpret_cast<DBusHandlerRegistry *>(user_data);

  if (this_->set_ip_config_handler_) {
    auto result = this_->set_ip_config_handler_(arg_config);
    netconfd_ip_config_complete_set(object, invocation, result.c_str());
  } else {
    g_dbus_method_invocation_return_dbus_error(invocation, "de.wago.netconfd1.ip_config.Error.Set",
                                               "No Handler for Set request");
  }
  return true;
}

gboolean DBusHandlerRegistry::GetAllIPConfig(netconfdIp_config *object, GDBusMethodInvocation *invocation,
                                             gpointer user_data) {
  auto this_ = reinterpret_cast<DBusHandlerRegistry *>(user_data);

  if (this_->get_all_ip_config_handler_) {
    string data;
    auto result = this_->get_all_ip_config_handler_(data);
    netconfd_ip_config_complete_getall(object, invocation, data.c_str(), result.c_str() );
  } else {
    g_dbus_method_invocation_return_dbus_error(invocation, "de.wago.netconfd1.ip_config.Error.GetAll",
                                               "No Handler for GetAll request");
  }
  return true;
}

gboolean DBusHandlerRegistry::GetAllCurrentIPConfig(netconfdIp_config *object, GDBusMethodInvocation *invocation,
                                             gpointer user_data) {
  auto this_ = reinterpret_cast<DBusHandlerRegistry *>(user_data);

  if (this_->get_all_current_ip_config_handler_) {
    string data;
    auto result = this_->get_all_current_ip_config_handler_(data);
    netconfd_ip_config_complete_getallcurrent(object, invocation, data.c_str(), result.c_str());
  } else {
    g_dbus_method_invocation_return_dbus_error(invocation, "de.wago.netconfd1.ip_config.Error.GetAllCurrent",
                                               "No Handler for GetAll request");
  }
  return true;
}

gboolean DBusHandlerRegistry::GetIPConfig(netconfdIp_config *object, GDBusMethodInvocation *invocation,
                                          const gchar *arg_config, gpointer user_data) {
  auto this_ = reinterpret_cast<DBusHandlerRegistry *>(user_data);

  if (this_->get_ip_config_handler_) {
    string data;
    auto result = this_->get_ip_config_handler_(arg_config, data);
    netconfd_ip_config_complete_get(object, invocation, data.c_str(), result.c_str());
  } else {
    g_dbus_method_invocation_return_dbus_error(invocation, "de.wago.netconfd1.ip_config.Error.Get",
                                               "No Handler for GetIPConfig request");
  }
  return true;
}

gboolean DBusHandlerRegistry::TemporaryFixIP(netconfdIp_config *object, GDBusMethodInvocation *invocation,
                                             gpointer user_data) {
  auto this_ = reinterpret_cast<DBusHandlerRegistry *>(user_data);

  if (this_->tempfixip_handler_) {
    auto result = this_->tempfixip_handler_();
    netconfd_ip_config_complete_tempfixip(object, invocation,result.c_str());
  } else {
    g_dbus_method_invocation_return_dbus_error(invocation, "de.wago.netconfd1.ip_config.Error.TempTixIP",
                                               "No Handler for TempTixIP");
  }
  return true;
}

gboolean DBusHandlerRegistry::GetDipSwitchConfig(netconfdIp_config *object, GDBusMethodInvocation *invocation,
                                                   gpointer user_data) {
  auto this_ = reinterpret_cast<DBusHandlerRegistry *>(user_data);

  if (this_->get_dip_switch_config_handler_) {
    string data;
    auto result = this_->get_dip_switch_config_handler_(data);
    netconfd_ip_config_complete_getdipswitchconfig(object, invocation, data.c_str(), result.c_str());
  } else {
    g_dbus_method_invocation_return_dbus_error(invocation, "de.wago.netconfd1.ip_config.Error.Get",
                                               "No Handler for GetDipSwitchConfig request");
  }
  return true;
}

gboolean DBusHandlerRegistry::SetDipSwitchConfig(netconfdIp_config *object, GDBusMethodInvocation *invocation,
                                                   const gchar *arg_config, gpointer user_data) {
  auto this_ = reinterpret_cast<DBusHandlerRegistry *>(user_data);

  if (this_->set_dip_switch_config_handler_) {
    auto result = this_->set_dip_switch_config_handler_(arg_config);
    netconfd_ip_config_complete_setdipswitchconfig(object, invocation, result.c_str());
  } else {
    g_dbus_method_invocation_return_dbus_error(invocation, "de.wago.netconfd1.ip_config.Error.Get",
                                               "No Handler for SetDipSwitchConfig request");
  }
  return true;
}

} /* namespace dbus */
} /* namespace netconf */
