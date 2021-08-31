// SPDX-License-Identifier: LGPL-3.0-or-later

#include "Backup.hpp"
#include "NetconfdDbusClient.hpp"
#include "Status.hpp"
#include "Convert.hpp"
#include "JsonConverter.hpp"

namespace netconf {
namespace api {

Status Backup(::std::string backup_file_path, ::std::string targetversion) {  //NOLINT(performance-unnecessary-value-param)
  NetconfdDbusClient client;
  auto result = client.Backup(backup_file_path, targetversion);
  return result.error_;

}

Status Restore(::std::string backup_file_path) {  //NOLINT(performance-unnecessary-value-param)
  NetconfdDbusClient client;
  auto result = client.Restore(backup_file_path);
  return result.error_;
}

Status GetBackupParameterCount(::std::string& count) {
  NetconfdDbusClient client;
  auto result = client.GetBackupParameterCount();

  count = result.value_json_;
  return result.error_;
}

}  // namespace api
}  // namespace netconf
