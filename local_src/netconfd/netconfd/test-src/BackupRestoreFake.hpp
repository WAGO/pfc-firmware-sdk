// SPDX-License-Identifier: GPL-2.0-or-later
//------------------------------------------------------------------------------
///  \file     BackupRestoreFake.hpp
///
///  \brief    <short description of the file contents>
///
///  \author   <author> : WAGO Kontakttechnik GmbH & Co. KG
//------------------------------------------------------------------------------
#ifndef NETCONFD_TEST_SRC_BACKUPRESTOREFAKE_HPP_
#define NETCONFD_TEST_SRC_BACKUPRESTOREFAKE_HPP_

#include "IBackupRestore.hpp"
#include <string>

namespace netconf {

class BackupRestoreFake : public IBackupRestore {
 public:
  ::std::uint32_t GetBackupParameterCount() const {return 0;}

  Status Backup(const ::std::string& file_path, const ::std::string& network_data, const ::std::string& dipswitch_data, uint32_t version) const {
    (void) file_path;
    backup_network_data_content = network_data;
    backup_dipswicth_data_content = dipswitch_data;
    backup_version = version;
    return backup_status;
  }

  Status Restore(const ::std::string& file_path, ::std::string& network_data, ::std::string& dipswitch_data, uint32_t& version) const {
    (void) file_path;
    network_data = restore_network_data_content;
    dipswitch_data = restore_network_dipswitch_content;
    version = restore_version;
    return restore_status;
  }

  mutable ::std::string backup_network_data_content;
  mutable ::std::string backup_dipswicth_data_content;
  ::std::string restore_network_data_content;
  ::std::string restore_network_dipswitch_content;
  Status backup_status;
  Status restore_status;
  mutable ::std::uint32_t backup_version;
  ::std::uint32_t restore_version;

};

}  // namespace netconf

#endif /* NETCONFD_TEST_SRC_BACKUPRESTOREFAKE_HPP_ */
//---- End of source file ------------------------------------------------------

