//------------------------------------------------------------------------------
// Copyright (c) WAGO GmbH & Co. KG
//
// PROPRIETARY RIGHTS are involved in the subject matter of this material. All
// manufacturing, reproduction, use and sales rights pertaining to this
// subject matter are governed by the license agreement. The recipient of this
// software implicitly accepts the terms of the license.
//------------------------------------------------------------------------------

#include <gtest/gtest.h>
#include <rule_file_editor.hpp>
#include <string>

#include "error.hpp"
#include "test_utils.hpp"

using namespace wago::firewall;

struct TestData {
  ::std::string input_;
  ::std::string expected_output_;
};

class RuleFileEditorTest : public ::testing::Test, public testing::WithParamInterface<TestData> {
 public:
  std::string tempDir_;
  RuleFileEditor rule_file_editor_;

  RuleFileEditorTest()
      :
      tempDir_ { "" },
      rule_file_editor_ { } {
  }

  void SetUp() override
  {
    tempDir_ = TestUtils::create_temp_dir("firewall_test");
  }

  void TearDown() override
  {
    TestUtils::remove_dir(tempDir_);
  }
};

INSTANTIATE_TEST_CASE_P(InstantiationName, RuleFileEditorTest,
                        testing::Values(TestData { "", "\n" },
                                        TestData { "foo bar baz", "foo bar baz\n" },
                                        TestData { "foo\nbar\nbaz", "foo\nbar\nbaz\n" },
                                        TestData { "foo\nfoo\nbaz", "foo\nbaz\n" },
                                        TestData { "foo\nbar\nfoo", "foo\nbar\n" },
                                        TestData { "foo\nbar\nbar", "foo\nbar\n" },
                                        TestData { "foo\n\nbar", "foo\nbar\n" },
                                        TestData { "foo\n\n\nbar", "foo\nbar\n" },
                                        TestData { "foo\nbar\nCOMMIT\nbaz\nfoo\nCOMMIT\n", "foo\nbar\nCOMMIT\nbaz\nCOMMIT\n" }));

TEST_F(RuleFileEditorTest, RuleFileDoesNotExist) {

  ASSERT_THROW(rule_file_editor_.remove_duplicate_lines("foo"), wago::firewall::file_open_error);
}

TEST_P(RuleFileEditorTest, RemoveDuplicateLines) {

  auto data = GetParam();

  ::std::string file_path = tempDir_ + "/data.rls";
  TestUtils::write_to_file(file_path, data.input_);

  rule_file_editor_.remove_duplicate_lines(file_path);

  ::std::string actual_data;
  TestUtils::read_from_file(file_path, actual_data);

  ASSERT_EQ(data.expected_output_, actual_data);
}

