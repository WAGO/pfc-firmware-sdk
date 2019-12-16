// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include <string>
#include "Status.hpp"

namespace netconfd {

class IBackupRestore {
 public:
  IBackupRestore() = default;
  virtual ~IBackupRestore() = default;

  IBackupRestore(const IBackupRestore&) = delete;
  IBackupRestore& operator=(const IBackupRestore&) = delete;
  IBackupRestore(const IBackupRestore&&) = delete;
  IBackupRestore& operator=(const IBackupRestore&&) = delete;

  virtual ::std::uint32_t GetBackupParameterCount() const = 0;
  virtual Status Backup(const ::std::string& file_path, const ::std::string& data, uint32_t version) const = 0;
  virtual Status Restore(const ::std::string& file_path, ::std::string& data, uint32_t& version) const = 0;
};

} /* namespace netconfd */
