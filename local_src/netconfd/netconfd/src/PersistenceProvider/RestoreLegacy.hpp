// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include "IBackupRestore.hpp"
#include "IFileEditor.hpp"

namespace netconfd {

class RestoreLegacy : public IBackupRestore {
 public:
  explicit RestoreLegacy(IFileEditor& file_editor);
  virtual ~RestoreLegacy() = default;

  ::std::uint32_t GetBackupParameterCount() const override {return 0;}
  Status Backup(const ::std::string& file_path, const ::std::string& data, uint32_t version) const override;
  Status Restore(const ::std::string& file_path, ::std::string& data, uint32_t& version) const override ;
 private:

  IFileEditor& file_editor_;
};

} /* namespace netconfd */
