// SPDX-License-Identifier: LGPL-3.0-or-later
#pragma once

#include <chrono>
#include <string>
#include <vector>

#include "DbusResult.hpp"
#include "SharedSemaphore.hpp"

struct DBusConnection;

namespace netconf
{

  class DbusMsgPtr;

  class NetconfdDbusClient
  {
    public:
      NetconfdDbusClient();
      explicit NetconfdDbusClient(int dbus_timeout_millis);
      ~NetconfdDbusClient();

      bool CheckServiceAvailability(::std::chrono::seconds timeout);

      DbusResult SetBridgeConfig(const ::std::string &config);
      DbusResult GetBridgeConfig();

      DbusResult SetIpConfigs(const ::std::string &config);
      DbusResult GetIpConfigs();
      DbusResult GetCurrentIpConfigs();

      DbusResult SetInterfaceConfigs(const ::std::string &config);
      DbusResult GetInterfaceConfigs();
      DbusResult GetInterfaceStatuses();

      DbusResult GetDeviceInterfaces();

      DbusResult SetTemporaryFixedIpAddress();

      DbusResult GetDipSwitchConfig();
      DbusResult SetDipSwitchConfig(const ::std::string &config);

      DbusResult Backup(const ::std::string& backup_file_path, const ::std::string& targetversion);
      DbusResult Restore(const ::std::string &backup_file_path);
      DbusResult GetBackupParameterCount();

      DbusResult NotifyDynamicIPAction(const ::std::string &event);
      DbusResult ReloadHostConf();

    private:
      DBusConnection *conn_;
      int timeout_millis_;

      /**
       * semaphore to serialize the access to netconfds' dbus interface. workaround for an internal issue in netconfd.
       */
      SharedSemaphore dbus_access_semaphore_;

      DBusConnection* ConnectToDbus(::std::chrono::seconds timeout);
      DbusResult Send(const DbusMsgPtr &msg, const ::std::string &content);
      DbusResult Send(const DbusMsgPtr &msg);
      DbusResult GetStrings(const DbusMsgPtr &msg);


  };

}  // namespace netconf
