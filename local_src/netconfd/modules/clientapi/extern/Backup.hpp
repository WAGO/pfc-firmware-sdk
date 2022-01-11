// SPDX-License-Identifier: LGPL-3.0-or-later
#pragma once

#include <string>

#include "Status.hpp"

namespace netconf {
namespace api {

/**
 * @brief Instructs the netconfd network config daemon to write its configuration
 * to the backup file.
 * @note Don't call this method when the backup file already contains netconfd data.
 * @note On status see /var/log/messages for deatils.
 *  *
 * @param backup_file_path Absolute path to the backup file
 * @param targetversion The target firmware version of the backup, empty if same as running firmware.
 * @return Status Result of the backup operation, @see Status::Ok on success, @see Status::Status otherwise.
 */
Status Backup(::std::string backup_file_path, ::std::string targetversion = "");

/**
 * @brief Instructs the netconfd network config daemon to restore its configuration
 * from the backup file.
 *
 * @note On status see /var/log/messages for deatils. *
 * @param backup_file_path Absolute path to the backup file.
 * @return Status Result of the restore operation, @see Status::Ok on success, @see Status::Status otherwise.
 */
Status Restore(::std::string backup_file_path);

/**
 * @brief Get the Backup Parameter Count.
 *
 * @param ::std::string Count of parameters as string;
 * @return status
 */
Status GetBackupParameterCount(::std::string& count);


} // namespace api
} // namespace netconf
