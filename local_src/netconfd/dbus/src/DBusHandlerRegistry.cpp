/*
 * BrideInterface.cpp
 *
 *  Created on: 04.06.2019
 *      Author: u014487
 */

#include <DBusHandlerRegistry.h>
#include <cassert>

namespace netconfd {
namespace dbus {

DBusHandlerRegistry::DBusHandlerRegistry() {

  interface_object_ = netconfd_object_skeleton_new(
      "/de/wago/netconfd/interface_config");
  interface_config_ = netconfd_interface_config_skeleton_new();
  netconfd_object_skeleton_set_interface_config(interface_object_,
                                                interface_config_);

  //interface_config
  auto handler_id = g_signal_connect(interface_config_, "handle-set",
                                     G_CALLBACK(SetBridgeConfig), this);
  if (handler_id <= 0) {
    throw ::std::runtime_error(
        "DBusHandlerRegistry: Failed to connect SetBridgeConfig handler");
  }

  handler_id = g_signal_connect(interface_config_, "handle-get",
                                G_CALLBACK(GetBridgeConfig), this);
  if (handler_id <= 0) {
    throw ::std::runtime_error(
        "DBusHandlerRegistry: Failed to connect GetBridgeConfig handler");
  }

  handler_id = g_signal_connect(interface_config_, "handle-getdeviceinterfaces",
                                G_CALLBACK(GetDeviceInterfaces), this);
  if (handler_id <= 0) {
    throw ::std::runtime_error(
        "DBusHandlerRegistry: Failed to connect GetDeviceInterfaces handler");
  }

  handler_id = g_signal_connect(interface_config_, "handle-setinterfaceconfig",
                                G_CALLBACK(SetInterfaceConfig), this);
  if (handler_id <= 0) {
    throw ::std::runtime_error(
        "DBusHandlerRegistry: Failed to connect SetInterfaceConfig handler");
  }

  handler_id = g_signal_connect(interface_config_, "handle-getinterfaceconfig",
                                G_CALLBACK(GetInterfaceConfig), this);
  if (handler_id <= 0) {
    throw ::std::runtime_error(
        "DBusHandlerRegistry: Failed to connect GetInterfaceConfig handler");
  }

  handler_id = g_signal_connect(interface_config_, "handle-getbackupparamcount",
                                G_CALLBACK(GetBackupParamCount), this);
  if (handler_id <= 0) {
    throw ::std::runtime_error(
        "DBusHandlerRegistry: Failed to connect GetBackupParamCount handler");
  }

  handler_id = g_signal_connect(interface_config_, "handle-backup",
                                G_CALLBACK(Backup), this);
  if (handler_id <= 0) {
    throw ::std::runtime_error(
        "DBusHandlerRegistry: Failed to connect Backup handler");
  }

  handler_id = g_signal_connect(interface_config_, "handle-restore",
                                G_CALLBACK(Restore), this);
  if (handler_id <= 0) {
    throw ::std::runtime_error(
        "DBusHandlerRegistry: Failed to connect Restore handler");
  }

  //ip_config
  ip_object_ = netconfd_object_skeleton_new("/de/wago/netconfd/ip_config");
  ip_config_ = netconfd_ip_config_skeleton_new();
  netconfd_object_skeleton_set_ip_config(ip_object_, ip_config_);

  handler_id = g_signal_connect(ip_config_, "handle-setall",
                                G_CALLBACK(SetAllIPConfig), this);
  if (handler_id <= 0) {
    throw ::std::runtime_error(
        "DBusHandlerRegistry: Failed to connect SetAllIPConfig handler");
  }

  handler_id = g_signal_connect(ip_config_, "handle-set",
                                G_CALLBACK(SetIPConfig), this);
  if (handler_id <= 0) {
    throw ::std::runtime_error(
        "DBusHandlerRegistry: Failed to connect SetIPConfig handler");
  }

  handler_id = g_signal_connect(ip_config_, "handle-getall",
                                G_CALLBACK(GetAllIPConfig), this);
  if (handler_id <= 0) {
    throw ::std::runtime_error(
        "DBusHandlerRegistry: Failed to connect GetIPAllConfig handler");
  }

  handler_id = g_signal_connect(ip_config_, "handle-get",
                                G_CALLBACK(GetIPConfig), this);
  if (handler_id <= 0) {
    throw ::std::runtime_error(
        "DBusHandlerRegistry: Failed to connect GetIPConfig handler");
  }

}

DBusHandlerRegistry::~DBusHandlerRegistry() {
  g_object_unref(interface_object_);
  g_object_unref(interface_config_);

  g_object_unref(ip_object_);
  g_object_unref(ip_config_);
}

GDBusObjectSkeleton* DBusHandlerRegistry::GetInterfaceObject() const {

  return G_DBUS_OBJECT_SKELETON(interface_object_);
}

GDBusObjectSkeleton* DBusHandlerRegistry::GetIPObject() const {

  return G_DBUS_OBJECT_SKELETON(ip_object_);
}

gboolean DBusHandlerRegistry::SetBridgeConfig(netconfdInterface_config *object,
                                              GDBusMethodInvocation *invocation,
                                              const gchar *arg_config,
                                              gpointer user_data) {

  auto this_ = reinterpret_cast<DBusHandlerRegistry*>(user_data);

  if (this_->set_bridge_config_handler_) {
    auto set_result = this_->set_bridge_config_handler_(arg_config);
    netconfd_interface_config_complete_set(object, invocation, set_result);
  } else {
    g_dbus_method_invocation_return_dbus_error(
        invocation, "de.wago.netconfd1.interface_config.Error.Set",
        "No Handler for Set request");
  }
  return true;
}

gboolean DBusHandlerRegistry::GetBridgeConfig(netconfdInterface_config* object,
                                              GDBusMethodInvocation* invocation,
                                              gpointer user_data) {

  auto this_ = reinterpret_cast<DBusHandlerRegistry*>(user_data);

  if (this_->get_bridge_config_handler_) {
    auto set_result = this_->get_bridge_config_handler_();
    netconfd_interface_config_complete_get(object, invocation,
                                           set_result.c_str());
  } else {
    g_dbus_method_invocation_return_dbus_error(
        invocation, "de.wago.netconfd1.interface_config.Error.Get",
        "No Handler for Get request");
  }
  return true;

}

gboolean DBusHandlerRegistry::GetDeviceInterfaces(
    netconfdInterface_config *object, GDBusMethodInvocation *invocation,
    gpointer user_data) {

  auto this_ = reinterpret_cast<DBusHandlerRegistry*>(user_data);

  if (this_->get_device_interfaces_handler_) {
    auto set_result = this_->get_device_interfaces_handler_();
    netconfd_interface_config_complete_getdeviceinterfaces(object, invocation,
                                                           set_result.c_str());
  } else {
    g_dbus_method_invocation_return_dbus_error(
        invocation,
        "de.wago.netconfd1.interface_config.Error.GetDeviceInterfaces",
        "No Handler for Get request");
  }
  return true;

}

gboolean DBusHandlerRegistry::SetInterfaceConfig(
    netconfdInterface_config *object, GDBusMethodInvocation *invocation,
    const gchar *arg_config, gpointer user_data) {

  auto this_ = reinterpret_cast<DBusHandlerRegistry*>(user_data);

  if (this_->set_interface_config_handler_) {
    auto set_result = this_->set_interface_config_handler_(arg_config);
    netconfd_interface_config_complete_setinterfaceconfig(object, invocation,
                                                           set_result);
  } else {
    g_dbus_method_invocation_return_dbus_error(
        invocation,
        "de.wago.netconfd1.interface_config.Error.SetInterfaceConfig",
        "No Handler for Get request");
  }
  return true;

}

gboolean DBusHandlerRegistry::GetInterfaceConfig(
    netconfdInterface_config *object, GDBusMethodInvocation *invocation,
    gpointer user_data) {

  auto this_ = reinterpret_cast<DBusHandlerRegistry*>(user_data);

  if (this_->get_interface_config_handler_) {
    auto set_result = this_->get_interface_config_handler_();
    netconfd_interface_config_complete_getinterfaceconfig(object, invocation,
                                                          set_result.c_str());
  } else {
    g_dbus_method_invocation_return_dbus_error(
        invocation,
        "de.wago.netconfd1.interface_config.Error.GetInterfaceConfig",
        "No Handler for Get request");
  }
  return true;

}


gboolean DBusHandlerRegistry::GetBackupParamCount(
    netconfdInterface_config *object, GDBusMethodInvocation *invocation,
    gpointer user_data) {

  auto this_ = reinterpret_cast<DBusHandlerRegistry*>(user_data);

  if (this_->get_backup_param_count_handler_) {
    auto set_result = this_->get_backup_param_count_handler_();
    netconfd_interface_config_complete_getbackupparamcount(object, invocation,
                                                           set_result.c_str());
  } else {
    g_dbus_method_invocation_return_dbus_error(
        invocation,
        "de.wago.netconfd1.interface_config.Error.GetBackupParamCount",
        "No Handler for Get request");
  }
  return true;

}

gboolean DBusHandlerRegistry::Backup(netconfdInterface_config *object,
                                     GDBusMethodInvocation *invocation,
                                     const gchar *arg_config,
                                     gpointer user_data) {

  auto this_ = reinterpret_cast<DBusHandlerRegistry*>(user_data);

  if (this_->backup_handler_) {
    auto set_result = this_->backup_handler_(arg_config);
    netconfd_interface_config_complete_backup(object, invocation, set_result);
  } else {
    g_dbus_method_invocation_return_dbus_error(
        invocation, "de.wago.netconfd1.interface_config.Error.Backup",
        "No Handler for Backup request");
  }
  return true;
}

gboolean DBusHandlerRegistry::Restore(netconfdInterface_config *object,
                                      GDBusMethodInvocation *invocation,
                                      const gchar *arg_config,
                                      gpointer user_data) {

  auto this_ = reinterpret_cast<DBusHandlerRegistry*>(user_data);

  if (this_->restore_handler_) {
    auto set_result = this_->restore_handler_(arg_config);
    netconfd_interface_config_complete_restore(object, invocation, set_result);
  } else {
    g_dbus_method_invocation_return_dbus_error(
        invocation, "de.wago.netconfd1.interface_config.Error.Restore",
        "No Handler for Restore request");
  }
  return true;
}

gboolean DBusHandlerRegistry::SetAllIPConfig(netconfdIp_config *object,
                                             GDBusMethodInvocation *invocation,
                                             const gchar *arg_config,
                                             gpointer user_data) {

  auto this_ = reinterpret_cast<DBusHandlerRegistry*>(user_data);

  if (this_->set_all_ip_config_handler_) {
    auto set_result = this_->set_all_ip_config_handler_(arg_config);
    netconfd_ip_config_complete_setall(object, invocation, set_result);
  } else {
    g_dbus_method_invocation_return_dbus_error(
        invocation, "de.wago.netconfd1.ip_config.Error.SetAll",
        "No Handler for SetAll request");
  }
  return true;
}

gboolean DBusHandlerRegistry::SetIPConfig(netconfdIp_config *object,
                                          GDBusMethodInvocation *invocation,
                                          const gchar *arg_config,
                                          gpointer user_data) {

  auto this_ = reinterpret_cast<DBusHandlerRegistry*>(user_data);

  if (this_->set_ip_config_handler_) {
    auto set_result = this_->set_ip_config_handler_(arg_config);
    netconfd_ip_config_complete_set(object, invocation, set_result);
  } else {
    g_dbus_method_invocation_return_dbus_error(
        invocation, "de.wago.netconfd1.ip_config.Error.Set",
        "No Handler for Set request");
  }
  return true;
}

gboolean DBusHandlerRegistry::GetAllIPConfig(netconfdIp_config* object,
                                             GDBusMethodInvocation* invocation,
                                             gpointer user_data) {

  auto this_ = reinterpret_cast<DBusHandlerRegistry*>(user_data);

  if (this_->get_all_ip_config_handler_) {
    auto set_result = this_->get_all_ip_config_handler_();
    netconfd_ip_config_complete_getall(object, invocation, set_result.c_str());
  } else {
    g_dbus_method_invocation_return_dbus_error(
        invocation, "de.wago.netconfd1.ip_config.Error.GetAll",
        "No Handler for GetAll request");
  }
  return true;

}

gboolean DBusHandlerRegistry::GetIPConfig(netconfdIp_config* object,
                                          GDBusMethodInvocation* invocation,
                                          const gchar *arg_config,
                                          gpointer user_data) {

  auto this_ = reinterpret_cast<DBusHandlerRegistry*>(user_data);

  if (this_->get_ip_config_handler_) {
    auto set_result = this_->get_ip_config_handler_(arg_config);
    netconfd_ip_config_complete_get(object, invocation, set_result.c_str());
  } else {
    g_dbus_method_invocation_return_dbus_error(
        invocation, "de.wago.netconfd1.ip_config.Error.Get",
        "No Handler for GetIPConfig request");
  }
  return true;

}

} /* namespace dbus */
} /* namespace netconfd */
