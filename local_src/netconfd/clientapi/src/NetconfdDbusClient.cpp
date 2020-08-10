#include "NetconfdDbusClient.hpp"

#include <dbus/dbus.h>

#include <chrono>
#include <memory>
#include "DbusError.hpp"
#include "Types.hpp"

namespace netconf {

using ::std::string;

static constexpr auto dbus_target  = "de.wago.netconfd";
static constexpr auto send_timeout = 60000;

NetconfdDbusClient::NetconfdDbusClient(int dbus_timeout_millis) : timeout_millis_{dbus_timeout_millis} {
  DbusError err;
  conn_ = dbus_bus_get(DBUS_BUS_SYSTEM, &err);

  if (not CheckServiceAvailability(::std::chrono::seconds{60})) {
      throw ::std::runtime_error("netconfd dbus interface not available.");
  }

}

NetconfdDbusClient::NetconfdDbusClient() : NetconfdDbusClient(send_timeout) {
}

NetconfdDbusClient::~NetconfdDbusClient() {
  dbus_connection_unref(conn_);  // We dont own BUS_SYSTEM so use unref
}

class DbusMsgPtr : public ::std::unique_ptr<DBusMessage, decltype(dbus_message_unref) *> {
 public:
  using ::std::unique_ptr<DBusMessage, decltype(dbus_message_unref) *>::unique_ptr;

  explicit DbusMsgPtr(DBusMessage *dbus_msg)
      : ::std::unique_ptr<DBusMessage, decltype(dbus_message_unref) *>(dbus_msg, dbus_message_unref) {
  }
};

class DbusMsgContent {
 public:
  explicit DbusMsgContent(const DbusMsgPtr &msg): iter{}{
    dbus_message_iter_init_append(msg.get(), &iter);
  }

  void Add(const string& str) {
    auto content_cstr = str.c_str();
    dbus_message_iter_append_basic(&iter, DBUS_TYPE_STRING, &content_cstr);
  }

 private:
  DBusMessageIter iter;
};

static DbusMsgPtr CreateBackupMessage(::std::string method_name)  // NOLINT(performance-unnecessary-value-param)
{                                                                    // NOLINT(performance-unnecessary-value-param)
  return DbusMsgPtr(dbus_message_new_method_call(dbus_target,
                                                 "/de/wago/netconfd/backup",  // object to call on
                                                 "de.wago.netconfd1.backup",  // interface to call on
                                                 method_name.c_str()));
}

static DbusMsgPtr CreateInterfaceMessage(::std::string method_name)  // NOLINT(performance-unnecessary-value-param)
{                                                                    // NOLINT(performance-unnecessary-value-param)
  return DbusMsgPtr(dbus_message_new_method_call(dbus_target,
                                                 "/de/wago/netconfd/interface_config",  // object to call on
                                                 "de.wago.netconfd1.interface_config",  // interface to call on
                                                 method_name.c_str()));
}

static DbusMsgPtr CreateIpMessage(::std::string method_name)  // NOLINT(performance-unnecessary-value-param)
{                                                             // NOLINT(performance-unnecessary-value-param)
  return DbusMsgPtr(dbus_message_new_method_call(dbus_target, "/de/wago/netconfd/ip_config",
                                                 "de.wago.netconfd1.ip_config", method_name.c_str()));
}

static void RequestServiceList(DBusConnection *bus, DBusPendingCall **pending_call) {
  DbusMsgPtr msg{ dbus_message_new_method_call("org.freedesktop.DBus", "/org/freedesktop/DBus", "org.freedesktop.DBus",
                                                "ListNames")};
  dbus_connection_send_with_reply(bus, msg.get(), pending_call, -1);
  dbus_connection_flush(bus);
}

static DBusHandlerResult CheckNameOwnerChanged(DBusConnection *bus, DBusMessage *message, void *user_data) {
  auto pending_call =
      reinterpret_cast<DBusPendingCall **>(user_data);  // NOLINT(cppcoreguidelines-pro-type-reinterpret-cast)
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
  DbusMsgPtr msg{ dbus_pending_call_steal_reply(pending_call)};

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
  auto end    = std::chrono::steady_clock::now() + timeout;
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

::std::string NetconfdDbusClient::GetBridgeConfig() {
  auto msg = CreateInterfaceMessage("get");
  return GetString(msg);
}

bool NetconfdDbusClient::SetBridgeConfig(const ::std::string &json_config) {
  auto msg         = CreateInterfaceMessage("set");
  int32_t response = SendString(msg, json_config);

  return (response == 0);
}

::std::string NetconfdDbusClient::GetIpConfigs() {
  auto msg = CreateIpMessage("getall");
  return GetString(msg);
}

::std::string NetconfdDbusClient::GetCurrentIpConfigs() {
  auto msg = CreateIpMessage("getallcurrent");
  return GetString(msg);
}

bool NetconfdDbusClient::SetIpConfigs(const ::std::string &json_config) {
  auto msg         = CreateIpMessage("setall");
  int32_t response = SendString(msg, json_config);

  return (response == 0);
}

bool NetconfdDbusClient::SetTemporaryFixedIpAddress() {
  auto msg         = CreateIpMessage("tempfixip");
  int32_t response = Send(msg);

  return (response == 0);
}

::std::string NetconfdDbusClient::GetDipSwitchConfig() {
  auto msg = CreateIpMessage("getdipswitchconfig");
  return GetString(msg);
}

bool NetconfdDbusClient::SetDipSwitchConfig(const ::std::string& config) {
  auto msg      = CreateIpMessage("setdipswitchconfig");
  auto response = SendString(msg, config);
  return response == 0;
}

::std::string NetconfdDbusClient::GetInterfaceConfigs() {
  auto msg = CreateInterfaceMessage("getinterfaceconfig");
  return GetString(msg);
}

bool NetconfdDbusClient::SetInterfaceConfigs(const ::std::string& config) {
  auto msg         = CreateInterfaceMessage("setinterfaceconfig");
  int32_t response = SendString(msg, config);

  return (response == 0);
}

::std::string NetconfdDbusClient::GetDeviceInterfaces() {
  auto msg = CreateInterfaceMessage("getdeviceinterfaces");
  return GetString(msg);
}

bool NetconfdDbusClient::Backup(const ::std::string& backup_file_path, const ::std::string& targetversion) {
  auto msg        = CreateBackupMessage("backup");
  DbusMsgContent msg_content{msg};
  msg_content.Add(backup_file_path);
  msg_content.Add(targetversion);
  int32_t respone = Send(msg);
  return (respone == 0);
}

bool NetconfdDbusClient::Restore(const ::std::string& backup_file_path) {
  auto msg        = CreateBackupMessage("restore");
  int32_t respone = SendString(msg, backup_file_path);
  return (respone == 0);
}

::std::string NetconfdDbusClient::GetBackupParameterCount() {
  auto msg = CreateBackupMessage("getbackupparamcount");
  return GetString(msg);
}

::std::string NetconfdDbusClient::GetString(const DbusMsgPtr& msg) {
  // send message and get a handle for a reply
  DbusError error;
  auto replymsg = DbusMsgPtr{dbus_connection_send_with_reply_and_block(conn_, msg.get(), timeout_millis_, &error)};

  // read the parameters
  if (replymsg) {
    DBusMessageIter args;
    if (dbus_message_iter_init(replymsg.get(), &args) == 0) {
      fprintf(stderr, "Message has no arguments!\n");
    } else if (DBUS_TYPE_STRING != dbus_message_iter_get_arg_type(&args)) {
      fprintf(stderr, "Argument is not string!\n");
    } else {
      char *response = nullptr;
      dbus_message_iter_get_basic(&args, &response);
      return response;
    }
  }

  return "";
}

int32_t NetconfdDbusClient::Send(const DbusMsgPtr &msg) {
  DbusError error;
  auto replymsg = DbusMsgPtr{dbus_connection_send_with_reply_and_block(conn_, msg.get(), timeout_millis_, &error)};

  return (dbus_error_is_set(&error) != 0) ? -1 : 0;
}

int32_t NetconfdDbusClient::SendString(const DbusMsgPtr &msg, const ::std::string &content) {
  DBusMessageIter args;
  int32_t response = -1;

  dbus_message_iter_init_append(msg.get(), &args);
  auto content_cstr = content.c_str();
  dbus_message_iter_append_basic(&args, DBUS_TYPE_STRING, &content_cstr);

  // send message and get a handle for a reply
  DbusError error;
  auto replymsg = DbusMsgPtr{dbus_connection_send_with_reply_and_block(conn_, msg.get(), timeout_millis_, &error)};

  if (replymsg) {
    // read the parameters
    if (dbus_message_iter_init(replymsg.get(), &args) == 0) {
      fprintf(stderr, "Message has no arguments!\n");
    } else if (DBUS_TYPE_INT32 != dbus_message_iter_get_arg_type(&args)) {
      fprintf(stderr, "Argument is not int32! %d\n", dbus_message_iter_get_arg_type(&args));
    } else {
      dbus_message_iter_get_basic(&args, &response);
    }
  }

  return response;
}

}  // namespace netconf
