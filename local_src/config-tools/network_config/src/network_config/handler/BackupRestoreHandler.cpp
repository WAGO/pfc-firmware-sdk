// SPDX-License-Identifier: GPL-2.0-or-later

#include "BackupRestoreHandler.hpp"

#include <Backup.hpp>
#include <exception>
#include <iostream>
#include <string>

#include "NetconfStatus.hpp"
#include "OptionStrings.hpp"
#include "Utils.hpp"

namespace po = boost::program_options;

namespace network_config {

BackupRestoreHandler::BackupRestoreHandler(const po::variables_map &vm)
    : vm_ { vm } {
}

void BackupRestoreHandler::Execute() {
  const auto &optstr = GetOptions();
  if (Contains(vm_, optstr.backup)) {
    auto backup_file_path = GetValueOf(vm_, optstr.backup);
    ::std::string targetversion;
    if (Contains(vm_, optstr.backup_targetversion)) {
      targetversion = GetValueOf(vm_, optstr.backup_targetversion);
    }
    auto error = netconf::api::Backup(backup_file_path, targetversion);
    if (error.IsNotOk()) {
      throw NetconfStatus(error);
    }
  } else if (Contains(vm_, optstr.restore)) {
    auto restore_file_path = GetValueOf(vm_, optstr.restore);
    auto error = netconf::api::Restore(restore_file_path);
    if (error.IsNotOk()) {
      throw NetconfStatus(error);
    }
  } else if (Contains(vm_, optstr.get_backup_parameter_count)) {
    ::std::string count;
    auto error = netconf::api::GetBackupParameterCount(count);
    if (error.IsNotOk()) {
      throw NetconfStatus(error);
    }
    ::std::cout << count;
  }
}

} /* namespace network_config */
