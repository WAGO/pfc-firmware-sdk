// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include <string>

#include "Error.hpp"
#include "IFileEditor.hpp"

namespace netconf {

class FileEditor : public IFileEditor {
 public:
  FileEditor() = default;
  ~FileEditor() override = default;

  FileEditor(const FileEditor&) = delete;
  FileEditor& operator=(const FileEditor&) = delete;
  FileEditor(const FileEditor&&) = delete;
  FileEditor& operator=(const FileEditor&&) = delete;

  Error Read(const ::std::string& file_path, ::std::string& data) const override;
  Error Write(const ::std::string& file_path, const ::std::string& data) const override;
  Error Append(const ::std::string& file_path, const ::std::string& data) const override;

};

} /* namespace netconf */
