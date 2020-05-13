// SPDX-License-Identifier: GPL-2.0-or-later
#pragma once

#include <chrono>
#include <string>

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

      bool SetBridgeConfig(const ::std::string &config);
      ::std::string GetBridgeConfig();

      bool SetIpConfigs(const ::std::string &config);
      ::std::string GetIpConfigs();
      ::std::string GetCurrentIpConfigs();

      bool SetInterfaceConfigs(const ::std::string &config);
      ::std::string GetInterfaceConfigs();

      ::std::string GetDeviceInterfaces();

      bool SetTemporaryFixedIpAddress();

      ::std::string GetDipSwitchConfig();
      bool SetDipSwitchConfig(const ::std::string &config);

      bool Backup(const ::std::string& backup_file_path, const ::std::string& targetversion);
      bool Restore(const ::std::string &backup_file_path);
      ::std::string GetBackupParameterCount();

    private:
      DBusConnection *conn_;

      ::std::string GetString(const DbusMsgPtr &msg);
      int32_t SendString(const DbusMsgPtr &msg,
                         const ::std::string &content);
      int32_t Send(const DbusMsgPtr &msg);
      int timeout_millis_;
  };

}  // namespace netconf
