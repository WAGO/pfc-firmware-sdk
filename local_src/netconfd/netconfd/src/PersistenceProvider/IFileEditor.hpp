// SPDX-License-Identifier: GPL-2.0-or-later

 #pragma once

#include "Error.hpp"

namespace netconf {

class IFileEditor {
 public:

  IFileEditor() = default;
  virtual ~IFileEditor() = default;
  IFileEditor(const IFileEditor&) = default;
  IFileEditor& operator=(const IFileEditor&) = default;
  IFileEditor(IFileEditor&&) = default;
  IFileEditor& operator=(IFileEditor&&) = default;

  virtual Error Read(const ::std::string& file_path, ::std::string& data) const =0;
  virtual Error Write(const ::std::string& file_path,
                       const ::std::string& data) const =0;
  virtual Error Append(const ::std::string& file_path,
                        const ::std::string& data) const =0;

};

}  // namespace netconf
