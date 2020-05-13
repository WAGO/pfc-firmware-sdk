// SPDX-License-Identifier: GPL-2.0-or-later

#include "Backup.hpp"
#include "NetconfdDbusClient.hpp"

namespace netconf {

Status Backup(::std::string backup_file_path, ::std::string targetversion) { //NOLINT(performance-unnecessary-value-param)
  NetconfdDbusClient client;
  return client.Backup(backup_file_path, targetversion) ? Status::OK : Status::ERROR;
}

Status Restore(::std::string backup_file_path) { //NOLINT(performance-unnecessary-value-param)
    NetconfdDbusClient client;
    return client.Restore(backup_file_path) ? Status::OK : Status::ERROR;
}

::std::string GetBackupParameterCount() {
    NetconfdDbusClient client;
    return client.GetBackupParameterCount();
}

}  // namespace netconf
