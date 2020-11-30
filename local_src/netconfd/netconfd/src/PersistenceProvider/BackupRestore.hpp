// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include <string>

#include "Error.hpp"
#include "IBackupRestore.hpp"
#include "IFileEditor.hpp"


namespace netconf {

class BackupRestore : public IBackupRestore {
 public:
  BackupRestore(IFileEditor& file_editor, uint32_t chars_per_line_in_backup_file);
  virtual ~BackupRestore() = default;

  BackupRestore(const BackupRestore&) = delete;
  BackupRestore& operator=(const BackupRestore&) = delete;
  BackupRestore(const BackupRestore&&) = delete;
  BackupRestore& operator=(const BackupRestore&&) = delete;

  uint32_t GetBackupParameterCount() const override;
  Error Backup(const ::std::string& file_path, const ::std::string& network_data, const ::std::string& dip_switch_data, uint32_t version) const override;
  Error Restore(const ::std::string& file_path, ::std::string& backup_network_data, ::std::string& backup_dipswitch_data, uint32_t& version) const override;

  const ::std::string KEY_NETCONFD_VERSION = "network.version=";
  const ::std::string KEY_NETCONFD_NETWORK_DATA = "network.data=";
  const ::std::string KEY_NETCONFD_DIPSWITCH_DATA = "network.dipswitch=";
 private:

  Error AppendTextWithKeyAndSeparateOnNewLine(const ::std::string& file_path, const ::std::string& key, const ::std::string& data) const;


  IFileEditor& file_editor_;
  uint32_t chars_per_line_;


};

} /* namespace netconf */
