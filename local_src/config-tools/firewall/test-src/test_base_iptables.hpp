//------------------------------------------------------------------------------
// Copyright (c) WAGO GmbH & Co. KG
//
// PROPRIETARY RIGHTS are involved in the subject matter of this material. All
// manufacturing, reproduction, use and sales rights pertaining to this
// subject matter are governed by the license agreement. The recipient of this
// software implicitly accepts the terms of the license.
//------------------------------------------------------------------------------

#include <gtest/gtest.h>

#include "file_accessor.hpp"

using namespace wago::firewall;

class IptablesTestBase : public ::testing::Test {

 public:
  ::std::string tmp_dir_;
  FileAccessor file_accessor_;

  IptablesTestBase();

  void SetUp() override;
  void TearDown() override;
};
