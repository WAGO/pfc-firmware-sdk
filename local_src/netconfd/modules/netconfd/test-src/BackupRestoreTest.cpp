// SPDX-License-Identifier: GPL-2.0-or-later
//------------------------------------------------------------------------------
///  \file     BackupExecutorTest.cpp
///
///  \brief    <short description of the file contents>
///
///  \author   <author> : WAGO GmbH & Co. KG
//------------------------------------------------------------------------------
#include "BackupRestore.hpp"
#include "MockIFileEditor.hpp"
#include <gtest/gtest.h>
#include "FileEditorFake.hpp"
#include <limits>
#include "TypesTestHelper.hpp"

using namespace testing;

namespace netconf {

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
  string network_data;
  string dipswitch_data;
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


  network_data = "abc";
  dipswitch_data = "xxx";
  expected_file_content = R"(network.version=42
network.data=abc
network.dipswitch=xxx
)";

  Status status = backup_restore_->Backup(path, network_data, dipswitch_data, 42);



  ASSERT_EQ(StatusCode::OK, status.GetStatusCode());
  EXPECT_EQ(expected_file_content, mock_file_editor_.content_);

}

TEST_F(ABackupRestore, CreatesABackupOverSeveralLines) {

  network_data = "0123456789abcd";
  dipswitch_data = "xxx";

  expected_file_content = R"(network.version=42
network.data=01234
network.data=56789
network.data=abcd
network.dipswitch=xxx
)";

  Status status = backup_restore_->Backup(path, network_data, dipswitch_data, 42);

  EXPECT_EQ(StatusCode::OK, status.GetStatusCode());
  EXPECT_EQ(expected_file_content, mock_file_editor_.content_);
}

TEST_F(ABackupRestore, RestoresABackup) {

  mock_file_editor_.content_ = R"(
XXX=123
XXX=123
network.version=42
network.data=abc
network.dipswitch=xxx
XXX=123
   )";


  Status status = backup_restore_->Restore(path, network_data, dipswitch_data, restored_version);

  EXPECT_EQ("abc", network_data);
  EXPECT_EQ("xxx", dipswitch_data);
  EXPECT_EQ(42, restored_version);
  ASSERT_EQ(StatusCode::OK, status.GetStatusCode());
}

TEST_F(ABackupRestore, RestoresAnEmptyBackup) {

  mock_file_editor_.content_ = R"(
XXX=123
XXX=123
network.data=
network.dipswitch=
XXX=123
   )";


  Status status = backup_restore_->Restore(path, network_data, dipswitch_data, restored_version);

  EXPECT_EQ("", network_data);
  EXPECT_EQ("", dipswitch_data);
  EXPECT_EQ(0, restored_version);
  EXPECT_EQ(StatusCode::BACKUP_CONTENT_MISSING, status.GetStatusCode()) << status.ToString();
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
network.dipswitch=abc
XXX=123
   )";

  Status status = backup_restore_->Restore(path, network_data, dipswitch_data, restored_version);

  EXPECT_EQ("0123456789abcd", network_data);
  EXPECT_EQ("abc", dipswitch_data);
  ASSERT_EQ(StatusCode::OK, status.GetStatusCode());
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
network.dipswitch=999
   )";


  Status status = backup_restore_->Restore(path, network_data, dipswitch_data, restored_version);

  EXPECT_EQ("56789abcd", network_data);
  EXPECT_EQ("999", dipswitch_data);
  EXPECT_EQ(42, restored_version);
  ASSERT_EQ(StatusCode::OK, status.GetStatusCode());
}

TEST_F(ABackupRestore, FailedToRestoresABackupReadReturnsAError) {

  mock_file_editor_.content_ = R"(
XXX=123
XXX=123
XXX=123
   )";

  mock_file_editor_.return_status = Status(StatusCode::FILE_READ);

  Status status = backup_restore_->Restore(path, network_data, dipswitch_data, restored_version);

  EXPECT_EQ("", network_data);
  EXPECT_EQ(StatusCode::FILE_READ, status.GetStatusCode());
}

TEST_F(ABackupRestore, FailedToRestoresABackupNoKeyFound) {

  mock_file_editor_.content_ = R"(
XXX=123
XXX=123
XXX=123
   )";


  Status status = backup_restore_->Restore(path, network_data, dipswitch_data, restored_version);

  EXPECT_EQ("", network_data);
  EXPECT_EQ(StatusCode::BACKUP_CONTENT_MISSING, status.GetStatusCode()) << status.ToString();
}

TEST_F(ABackupRestore, FailedToRestoresABackupNetconfdBackupVersionInvalid) {

  mock_file_editor_.content_ = R"(
network.version=42;01234
network.data=56789
network.dipswitch=xxx
XXX=123
   )";


  Status status = backup_restore_->Restore(path, network_data, dipswitch_data, restored_version);

  EXPECT_EQ("", network_data);
  EXPECT_EQ(0, restored_version);
  EXPECT_EQ(StatusCode::BACKUP_VERSION, status.GetStatusCode()) << status.ToString();
}



} /* namespace netconf */
