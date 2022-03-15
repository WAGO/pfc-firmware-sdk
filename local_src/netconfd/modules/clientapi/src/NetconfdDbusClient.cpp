// SPDX-License-Identifier: LGPL-3.0-or-later

#include "NetconfdDbusClient.hpp"

#include <chrono>
#include <memory>
#include <mutex>
#include <exception>
#include <thread>
#include <type_traits>

#include <dbus/dbus.h>
#include <sys/stat.h>

#include "DbusError.hpp"
#include "Types.hpp"
#include "SharedSemaphore.hpp"

namespace netconf {

static constexpr auto dbus_target = "de.wago.netconfd";
static constexpr auto send_timeout = 60000;
static constexpr auto dbus_access_semaphore_path = "/netconfapi";
static constexpr auto dbus_access_semaphore_init_value = 1;

class DbusMsgPtr : public ::std::unique_ptr<DBusMessage, decltype(dbus_message_unref)*> {
 public:
  using ::std::unique_ptr<DBusMessage, decltype(dbus_message_unref)*>::unique_ptr;

  explicit DbusMsgPtr(DBusMessage *dbus_msg)
      : ::std::unique_ptr<DBusMessage, decltype(dbus_message_unref)*>(dbus_msg, dbus_message_unref) {
  }
};

class DbusMsgContent {
 public:
  explicit DbusMsgContent(const DbusMsgPtr &msg)
      : iter { } {
    dbus_message_iter_init_append(msg.get(), &iter);
  }

  void Add(const ::std::string &str) {
    auto content_cstr = str.c_str();
    dbus_message_iter_append_basic(&iter, DBUS_TYPE_STRING, &content_cstr);
  }

 private:
  DBusMessageIter iter;
};

NetconfdDbusClient::NetconfdDbusClient(int dbus_timeout_millis)
    : conn_ { ConnectToDbus(::std::chrono::seconds { 60 }) },
      timeout_millis_ { dbus_timeout_millis },
      dbus_access_semaphore_{dbus_access_semaphore_path, dbus_access_semaphore_init_value} {
  if (conn_ == nullptr) {
    throw ::std::runtime_error("connection to dbus failed.");
  }

  if (not CheckServiceAvailability(::std::chrono::seconds { 60 })) {
    throw ::std::runtime_error("netconfd dbus interface not available.");
  }
}

NetconfdDbusClient::NetconfdDbusClient()
    : NetconfdDbusClient(send_timeout) {

}

NetconfdDbusClient::~NetconfdDbusClient() {
  dbus_connection_unref(conn_);  // We dont own BUS_SYSTEM so use unref
}

DBusConnection* NetconfdDbusClient::ConnectToDbus(::std::chrono::seconds timeout) {
  DbusError err;
  auto end = std::chrono::steady_clock::now() + timeout;
  DBusConnection *connection;
  do {
    connection = dbus_bus_get(DBUS_BUS_SYSTEM, &err);
    if (connection == nullptr || dbus_error_is_set(&err) != 0) {
      dbus_error_free(&err);
      ::std::this_thread::sleep_for(::std::chrono::seconds { 1 });
    }
  } while (connection == nullptr && std::chrono::steady_clock::now() < end);
  return connection;
}

static DbusMsgPtr CreateBackupMessage(::std::string method_name)  // NOLINT(performance-unnecessary-value-param)
    {                                                                    // NOLINT(performance-unnecessary-value-param)
  return DbusMsgPtr(dbus_message_new_method_call(dbus_target, "/de/wago/netconfd/backup",  // object to call on
                                                 "de.wago.netconfd1.backup",  // interface to call on
                                                 method_name.c_str()));
}

static DbusMsgPtr CreateInterfaceMessage(::std::string method_name)  // NOLINT(performance-unnecessary-value-param)
    {                                                                    // NOLINT(performance-unnecessary-value-param)
  return DbusMsgPtr(dbus_message_new_method_call(dbus_target, "/de/wago/netconfd/interface_config",  // object to call on
                                                 "de.wago.netconfd1.interface_config",  // interface to call on
                                                 method_name.c_str()));
}

static DbusMsgPtr CreateEventMessage(::std::string method_name)  // NOLINT(performance-unnecessary-value-param)
    {                                                                    // NOLINT(performance-unnecessary-value-param)
  return DbusMsgPtr(dbus_message_new_method_call(dbus_target, "/de/wago/netconfd/event",  // object to call on
                                                 "de.wago.netconfd1.event",  // interface to call on
                                                 method_name.c_str()));
}

static DbusMsgPtr CreateIpMessage(::std::string method_name)  // NOLINT(performance-unnecessary-value-param)
    {                                                             // NOLINT(performance-unnecessary-value-param)
  return DbusMsgPtr(
      dbus_message_new_method_call(dbus_target, "/de/wago/netconfd/ip_config", "de.wago.netconfd1.ip_config",
                                   method_name.c_str()));
}

static void RequestServiceList(DBusConnection *bus, DBusPendingCall **pending_call) {
  DbusMsgPtr msg { dbus_message_new_method_call("org.freedesktop.DBus", "/org/freedesktop/DBus", "org.freedesktop.DBus",
                                                "ListNames") };
  dbus_connection_send_with_reply(bus, msg.get(), pending_call, -1);
  dbus_connection_flush(bus);
}

static DBusHandlerResult CheckNameOwnerChanged(DBusConnection *bus, DBusMessage *message, void *user_data) {
  auto pending_call = reinterpret_cast<DBusPendingCall**>(user_data);  // NOLINT(cppcoreguidelines-pro-type-reinterpret-cast)
  if (dbus_message_has_member(message, "NameOwnerChanged") != 0) {
    if (*pending_call == nullptr) {
      RequestServiceList(bus, pending_call);
    }
  }
  return DBUS_HANDLER_RESULT_HANDLED;
}

static bool CheckServiceList(DBusPendingCall *pending_call) {
  DBusMessageIter iter;
  DBusMessageIter value;
  DbusMsgPtr msg { dbus_pending_call_steal_reply(pending_call) };

  dbus_message_iter_init(msg.get(), &iter);

  if (dbus_message_iter_get_arg_type(&iter) == DBUS_TYPE_ARRAY) {
    dbus_message_iter_recurse(&iter, &value);
    while (dbus_message_iter_get_arg_type(&value) == DBUS_TYPE_STRING) {
      const char *name;
      dbus_message_iter_get_basic(&value, &name);
      using namespace std::literals;
      if (("de.wago.netconfd"s == name)) {
        return true;
      }
      dbus_message_iter_next(&value);
    }
  }
  return false;
}

bool NetconfdDbusClient::CheckServiceAvailability(std::chrono::seconds timeout) {
  DbusError error;
  auto end = std::chrono::steady_clock::now() + timeout;
  auto filter = "interface='org.freedesktop.DBus',member='NameOwnerChanged'";
  dbus_bus_add_match(conn_, filter, &error);
  if (dbus_error_is_set(&error) != 0) {
    return false;
  }

  DBusPendingCall *pending_call;
  bool found = false;

  dbus_connection_add_filter(conn_, CheckNameOwnerChanged, &pending_call, nullptr);
  RequestServiceList(conn_, &pending_call);
  while (!found && end > std::chrono::steady_clock::now() && (dbus_connection_read_write_dispatch(conn_, 250) != 0)) {
    if (pending_call != nullptr && (dbus_pending_call_get_completed(pending_call) != 0)) {
      found = CheckServiceList(pending_call);
      dbus_pending_call_unref(pending_call);
      pending_call = nullptr;
    }
  }

  dbus_connection_remove_filter(conn_, CheckNameOwnerChanged, &pending_call);
  dbus_bus_remove_match(conn_, filter, &error);
  return found;
}

static ::std::vector<::std::string> GetMessageStrings(const netconf::DbusMsgPtr &replymsg) {
  ::std::vector<::std::string> strings;
  DBusMessageIter args;
  for (auto has_argument = dbus_message_iter_init(replymsg.get(), &args); static_cast<bool>(has_argument); has_argument =
      dbus_message_iter_next(&args)) {
    if (DBUS_TYPE_STRING == dbus_message_iter_get_arg_type(&args)) {
      char *response = nullptr;
      dbus_message_iter_get_basic(&args, &response);
      strings.emplace_back(response);
    }
  }
  return strings;
}

DbusResult NetconfdDbusClient::GetBackupParameterCount() {
  auto msg = CreateBackupMessage("getbackupparamcount");
  return GetStrings(msg);
}

DbusResult NetconfdDbusClient::GetBridgeConfig() {
  auto msg = CreateInterfaceMessage("get");
  return GetStrings(msg);
}

DbusResult NetconfdDbusClient::SetBridgeConfig(const ::std::string &json_config) {
  auto msg = CreateInterfaceMessage("set");
  return Send(msg, json_config);
}

DbusResult NetconfdDbusClient::GetIpConfigs() {
  auto msg = CreateIpMessage("getall");
  return GetStrings(msg);
}

DbusResult NetconfdDbusClient::GetCurrentIpConfigs() {
  auto msg = CreateIpMessage("getallcurrent");
  return GetStrings(msg);
}

DbusResult NetconfdDbusClient::SetIpConfigs(const ::std::string &json_config) {
  auto msg = CreateIpMessage("setall");
  return Send(msg, json_config);
}

DbusResult NetconfdDbusClient::SetTemporaryFixedIpAddress() {
  auto msg = CreateIpMessage("tempfixip");
  return Send(msg);
}

DbusResult NetconfdDbusClient::GetDipSwitchConfig() {
  auto msg = CreateIpMessage("getdipswitchconfig");
  return GetStrings(msg);
}

DbusResult NetconfdDbusClient::SetDipSwitchConfig(const ::std::string &config) {
  auto msg = CreateIpMessage("setdipswitchconfig");
  return Send(msg, config);
}

DbusResult NetconfdDbusClient::GetInterfaceConfigs() {
  auto msg = CreateInterfaceMessage("getinterfaceconfig");
  return GetStrings(msg);
}

DbusResult NetconfdDbusClient::GetInterfaceStatuses() {
  auto msg = CreateInterfaceMessage("getinterfacestatuses");
  return GetStrings(msg);
}

DbusResult NetconfdDbusClient::SetInterfaceConfigs(const ::std::string &config) {
  auto msg = CreateInterfaceMessage("setinterfaceconfig");
  return Send(msg, config);
}

DbusResult NetconfdDbusClient::GetDeviceInterfaces() {
  auto msg = CreateInterfaceMessage("getdeviceinterfaces");
  return GetStrings(msg);
}

DbusResult NetconfdDbusClient::Backup(const ::std::string &backup_file_path, const ::std::string &targetversion) {
  auto msg = CreateBackupMessage("backup");
  DbusMsgContent msg_content { msg };
  msg_content.Add(backup_file_path);
  msg_content.Add(targetversion);
  return Send(msg);
}

DbusResult NetconfdDbusClient::Restore(const ::std::string &backup_file_path) {
  auto msg = CreateBackupMessage("restore");
  return Send(msg, backup_file_path);
}

DbusResult NetconfdDbusClient::NotifyDynamicIPAction(const ::std::string &event) {
  auto msg = CreateEventMessage("dynamicip");
  return Send(msg, event);
}

DbusResult NetconfdDbusClient::ReloadHostConf() {
  auto msg = CreateEventMessage("reloadhostconf");
  return Send(msg);
}

DbusResult NetconfdDbusClient::Send(const DbusMsgPtr &msg, const ::std::string &content) {
  DBusMessageIter args;

  dbus_message_iter_init_append(msg.get(), &args);
  auto content_cstr = content.c_str();
  dbus_message_iter_append_basic(&args, DBUS_TYPE_STRING, &content_cstr);

  // send message and get a handle for a reply
  return Send(msg);
}

DbusResult NetconfdDbusClient::Send(const DbusMsgPtr &msg) {

  // WORKAROUND: guard to serialize access to the dbus interface of netconfd to work around an issue of
  // netconfd's internal event handling (namely the lack of synchronization between dbus and netlink events)
  const ::std::lock_guard<SharedSemaphore> dbus_access_lock(dbus_access_semaphore_);
  DbusError error;
  auto replymsg = DbusMsgPtr { dbus_connection_send_with_reply_and_block(conn_, msg.get(), timeout_millis_, &error) };

  if (error.IsSet()) {
    return DbusResult { error };
  }
  auto strings = GetMessageStrings(replymsg);
  return DbusResult { strings[0] };
}

DbusResult NetconfdDbusClient::GetStrings(const DbusMsgPtr &msg) {

  // WORKAROUND: guard to serialize access to the dbus interface of netconfd to work around an issue of
  // netconfd's internal event handling (namely the lack of synchronization between dbus and netlink events)
  const ::std::lock_guard<SharedSemaphore> dbus_access_lock(dbus_access_semaphore_);
  DbusError error;
  auto replymsg = DbusMsgPtr { dbus_connection_send_with_reply_and_block(conn_, msg.get(), timeout_millis_, &error) };

  if (error.IsSet()) {
    return DbusResult { error };
  }

  ::std::vector<::std::string> strings = GetMessageStrings(replymsg);
  return DbusResult { strings[0], strings.size() >= 2 ? strings[1] : "" };
}

}  // namespace netconf
