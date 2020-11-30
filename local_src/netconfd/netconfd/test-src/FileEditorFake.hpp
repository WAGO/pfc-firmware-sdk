// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * FileEditorFake.hpp
 *
 *  Created on: 15.08.2019
 *      Author: u014487
 */

#pragma once

#include "IFileEditor.hpp"
#include <string>

namespace netconf {

class FileEditorFake : public IFileEditor {
 public:

  FileEditorFake() = default;
  ~FileEditorFake() = default;

  Error Read(const ::std::string& file_path, ::std::string& data) const override {
    (void) file_path;
    data = content_;
    return return_status;
  }

  Error Write(const ::std::string& file_path, const ::std::string& data) const override {
    (void) file_path;
    content_ = data;
    return return_status;
  }

  Error Append(const ::std::string& file_path, const ::std::string& data) const override {
    (void) file_path;
    content_ += data;
    return return_status;
  }

  Error return_status;

  mutable ::std::string content_;
};

}  // namespace netconf
