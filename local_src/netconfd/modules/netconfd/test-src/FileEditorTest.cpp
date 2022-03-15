// SPDX-License-Identifier: GPL-2.0-or-later
//------------------------------------------------------------------------------
///  \file     AJsonParser.cpp
///
///  \brief    <short description of the file contents>
///
///  \author   <author> : WAGO Kontakttechnik GmbH & Co. KG
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// include files
//------------------------------------------------------------------------------

#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <stdio.h>

#include <fstream>
#include <iostream>
#include <memory>

#include "FileEditor.hpp"
#include "Status.hpp"

using namespace testing;

namespace netconf {

class AFileEditor : public Test {
 public:
  ::std::unique_ptr<FileEditor> file_editor;

  const ::std::string path_         = "persistence_provider_test.json";
  const ::std::string empty_path_   = "";
  const ::std::string missing_path_ = "persistence_provider_test.json.missing";

  AFileEditor() {
    file_editor = ::std::make_unique<FileEditor>();
  }

  void SetUp() override {
    remove(path_.c_str());
    remove(missing_path_.c_str());
  }

  void TearDown() override {
  }

  ::std::string data_ = R"(
    "XXX"
    "YYY"
  )";

  void WriteConfigFile(const ::std::string& file, const ::std::string& content) {
    ::std::ofstream config_stream(file);
    config_stream << content;
    config_stream.close();
  }

  ::std::string ReadConfigFile(const ::std::string& file) {
    ::std::ifstream config_stream(file);
    ::std::string json_config((std::istreambuf_iterator<char>(config_stream)), std::istreambuf_iterator<char>());
    config_stream.close();

    return json_config;
  }
};

TEST_F(AFileEditor, ReadsAConfiguration) {
  WriteConfigFile(path_, data_);

  ::std::string data;
  Status status = file_editor->Read(path_, data);

  EXPECT_EQ(data, data_);
  EXPECT_TRUE(status.IsOk());
}

TEST_F(AFileEditor, TriesToReadAMissingFile) {
  ::std::string data;
  Status status = file_editor->Read(missing_path_, data);

  EXPECT_EQ(data.size(), 0);
  EXPECT_EQ(StatusCode::FILE_READ, status.GetStatusCode());
}

TEST_F(AFileEditor, WritesAConfiguration) {
  Status status = file_editor->WriteAndReplace(path_, data_);
  EXPECT_TRUE(status.IsOk());

  EXPECT_EQ(data_, ReadConfigFile(path_));
}

TEST_F(AFileEditor, AppensAConfiguration) {
  WriteConfigFile(path_, data_);

  Status status = file_editor->Append(path_, data_);

  EXPECT_TRUE(status.IsOk());
  EXPECT_EQ(data_ + data_, ReadConfigFile(path_));
}

TEST_F(AFileEditor, TriesToAppendAMissingFile) {
  Status status = file_editor->Append(missing_path_, data_);

  EXPECT_EQ(StatusCode::FILE_WRITE, status.GetStatusCode());
}

}  // namespace netconf
