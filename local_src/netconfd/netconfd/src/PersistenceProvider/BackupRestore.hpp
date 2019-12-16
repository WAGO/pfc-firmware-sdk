// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include <string>

#include "IBackupRestore.hpp"
#include "IFileEditor.hpp"
#include "Status.hpp"


namespace netconfd {

class BackupRestore : public IBackupRestore {
 public:
  BackupRestore(IFileEditor& file_editor, uint32_t chars_per_line_in_backup_file);
  virtual ~BackupRestore() = default;

  BackupRestore(const BackupRestore&) = delete;
  BackupRestore& operator=(const BackupRestore&) = delete;
  BackupRestore(const BackupRestore&&) = delete;
  BackupRestore& operator=(const BackupRestore&&) = delete;

  uint32_t GetBackupParameterCount() const override;
  Status Backup(const ::std::string& file_path, const ::std::string& data, uint32_t version) const override;
  Status Restore(const ::std::string& file_path, ::std::string& data, uint32_t& version) const override;

  const ::std::string KEY_NETCONFD = "network.data=";
  const ::std::string KEY_NETCONFD_VERSION = "network.version=";

 private:

  IFileEditor& file_editor_;
  uint32_t chars_per_line_;


};

} /* namespace netconfd */
