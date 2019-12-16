// SPDX-License-Identifier: GPL-2.0-or-later
//------------------------------------------------------------------------------
///  \file     BackupExecutorTest.cpp
///
///  \brief    <short description of the file contents>
///
///  \author   <author> : WAGO Kontakttechnik GmbH & Co. KG
//------------------------------------------------------------------------------
#include "BackupRestore.hpp"
#include "MockIFileEditor.hpp"
#include <gtest/gtest.h>
#include "FileEditorFake.hpp"
#include <limits>

using namespace testing;

namespace netconfd {

using ::std::string;
using ::std::unique_ptr;
using ::std::uint32_t;
using ::std::size_t;


class ABackupRestore : public Test {
 public:

  FileEditorFake mock_file_editor_;
  unique_ptr<BackupRestore> backup_restore_;
  uint32_t restored_version;
  size_t backup_chars_per_line;
  string data;
  string path;
  string expected_file_content;

  void SetUp() override {
    path = "backup_file.txt";
    backup_chars_per_line = 5;
    backup_restore_ = ::std::make_unique<BackupRestore>(mock_file_editor_, backup_chars_per_line);
    restored_version = ::std::numeric_limits<uint32_t>::max();
  }



};



TEST_F(ABackupRestore, GetBackupParameterCount) {

  EXPECT_EQ(2, backup_restore_->GetBackupParameterCount());

}

TEST_F(ABackupRestore, CreatesABackup) {


  data = "abc";
  expected_file_content = R"(network.version=42
network.data=abc
)";

  Status status = backup_restore_->Backup(path, data, 42);



  ASSERT_EQ(StatusCode::OK, status.Get());
  EXPECT_EQ(expected_file_content, mock_file_editor_.content_);

}

TEST_F(ABackupRestore, CreatesABackupOverSeveralLines) {

  data = "0123456789abcd";

  expected_file_content = R"(network.version=42
network.data=01234
network.data=56789
network.data=abcd
)";

  Status status = backup_restore_->Backup(path, data, 42);

  EXPECT_EQ(StatusCode::OK, status.Get());
  EXPECT_EQ(expected_file_content, mock_file_editor_.content_);
}

TEST_F(ABackupRestore, RestoresABackup) {

  mock_file_editor_.content_ = R"(
XXX=123
XXX=123
network.version=42
network.data=abc
XXX=123
   )";


  Status status = backup_restore_->Restore(path, data, restored_version);

  EXPECT_EQ("abc", data);
  EXPECT_EQ(42, restored_version);
  ASSERT_EQ(StatusCode::OK, status.Get());
}

TEST_F(ABackupRestore, RestoresAnEmptyBackup) {

  mock_file_editor_.content_ = R"(
XXX=123
XXX=123
network.data=
XXX=123
   )";


  Status status = backup_restore_->Restore(path, data, restored_version);

  EXPECT_EQ("", data);
  EXPECT_EQ(0, restored_version);
  EXPECT_EQ(StatusCode::BACKUP_FILE_ERROR, status.Get());
}

TEST_F(ABackupRestore, RestoresABackupOverSeveralLines) {

  mock_file_editor_.content_ =
      R"(
XXX=123
XXX=123
network.version=42
network.data=01234
network.data=56789
network.data=abcd
XXX=123
   )";

  Status status = backup_restore_->Restore(path, data, restored_version);

  EXPECT_EQ("0123456789abcd", data);
  ASSERT_EQ(StatusCode::OK, status.Get());
}

TEST_F(ABackupRestore, RestoresABackupOverSeveralLinesAndIgnoreUninterstingKeys) {

  mock_file_editor_.content_ =
      R"(
XXX=123
XXX=123
network.version=42
network.data=56789
#packet: network=xxx
network.data=abcd
XXX=123
   )";


  Status status = backup_restore_->Restore(path, data, restored_version);

  EXPECT_EQ("56789abcd", data);
  EXPECT_EQ(42, restored_version);
  ASSERT_EQ(StatusCode::OK, status.Get());
}

TEST_F(ABackupRestore, FailedToRestoresABackupReadReturnsAError) {

  mock_file_editor_.content_ = R"(
XXX=123
XXX=123
XXX=123
   )";

  mock_file_editor_.return_status = Status(StatusCode::FILE_READ_ERROR);

  Status status = backup_restore_->Restore(path, data, restored_version);

  EXPECT_EQ("", data);
  EXPECT_EQ(StatusCode::FILE_READ_ERROR, status.Get());
}

TEST_F(ABackupRestore, FailedToRestoresABackupNoKeyFound) {

  mock_file_editor_.content_ = R"(
XXX=123
XXX=123
XXX=123
   )";


  Status status = backup_restore_->Restore(path, data, restored_version);

  EXPECT_EQ("", data);
  EXPECT_EQ(StatusCode::BACKUP_FILE_ERROR, status.Get());
}

TEST_F(ABackupRestore, FailedToRestoresABackupNetconfdBackupVersionInvalid) {

  mock_file_editor_.content_ = R"(
network.version=42;01234
network.data=56789
XXX=123
   )";


  Status status = backup_restore_->Restore(path, data, restored_version);

  EXPECT_EQ("", data);
  EXPECT_EQ(0, restored_version);
  EXPECT_EQ(StatusCode::BACKUP_FILE_ERROR, status.Get());
}



} /* namespace netconfd */
