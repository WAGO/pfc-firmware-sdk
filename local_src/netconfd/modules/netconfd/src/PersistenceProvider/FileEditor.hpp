// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include <string>

#include "Status.hpp"
#include "IFileEditor.hpp"

namespace netconf {

class FileEditor : public IFileEditor {
 public:
  FileEditor() = default;
  ~FileEditor() override = default;

  FileEditor(const FileEditor&) = default;
  FileEditor& operator=(const FileEditor&) = delete;
  FileEditor(const FileEditor&&) = delete;
  FileEditor& operator=(const FileEditor&&) = delete;

  Status Read(const ::std::string& file_path, ::std::string& data) const override;
  Status Write(const ::std::string& file_path, const ::std::string& data) const override;
  Status WriteAndReplace(const ::std::string& file_path, const ::std::string& data) const override;
  Status Append(const ::std::string& file_path, const ::std::string& data) const override;

};

} /* namespace netconf */
