// SPDX-License-Identifier: GPL-2.0-or-later

#include "BackupRestoreHandler.hpp"

#include <string>
#include <iostream>
#include <exception>

#include "Backup.hpp"
#include "OptionStrings.hpp"

namespace po = boost::program_options;

namespace network_config {

BackupRestoreHandler::BackupRestoreHandler(const po::variables_map &vm)
    :
    vm_ { vm } {
  const auto &optstr = GetOptions();

  if (vm_.count(optstr.backup.name) > 0) {
    execute_ = [this]() {
    const auto &optstr2= GetOptions();
      auto backup_file_path = vm_[optstr2.backup.name].as<::std::string>();
      ::std::string targetversion;
      if (vm_.count(optstr2.backup_targetversion.name) > 0) {
        targetversion = vm_[optstr2.backup_targetversion.name].as<::std::string>();
      }
      auto status = netconf::Backup(backup_file_path, targetversion);
      if (netconf::Status::OK != status) {
        throw ::std::runtime_error("Failed to run backup.");
      }
    };
  } else if (vm_.count(optstr.restore.name) > 0) {
    execute_ = [this]() {
      const auto &optstr2 = GetOptions();
      auto restore_file_path = vm_[optstr2.restore.name].as<::std::string>();
      auto status = netconf::Restore(restore_file_path);
      if (netconf::Status::OK != status) {
        throw ::std::runtime_error("Failed to run restore.");
      }
    };
  } else if (vm_.count(optstr.get_backup_parameter_count.name) > 0) {
    execute_ = []() {
      auto count = netconf::GetBackupParameterCount();
      ::std::cout << count;
    };
  }
}

void BackupRestoreHandler::Execute() {
  execute_();
}

} /* namespace network_config */
