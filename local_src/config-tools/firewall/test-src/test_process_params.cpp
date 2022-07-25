//------------------------------------------------------------------------------
// Copyright (c) WAGO GmbH & Co. KG
//
// PROPRIETARY RIGHTS are involved in the subject matter of this material. All
// manufacturing, reproduction, use and sales rights pertaining to this
// subject matter are governed by the license agreement. The recipient of this
// software implicitly accepts the terms of the license.
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
///  \file     test_process_params.cpp
///
///  \brief    Test function processing params configuration file.
///
///  \author   WAGO GmbH & Co. KG
//------------------------------------------------------------------------------

#include "file_accessor.hpp"
#include <gtest/gtest.h>
#include <cstdio>

#include "process_params.hpp"
#include "error.hpp"
#include "test_utils.hpp"

using namespace wago::firewall;

class ProcessParametersTest : public ::testing::Test {
 public:
  std::string tmp_dir_;
  std::string params_;
  std::string ipcmn_;
  FileAccessor file_accessor_;

  ProcessParametersTest()
      :
      tmp_dir_ { TestUtils::create_temp_dir("firewall_") },
      params_ { "../../../test-res/params.xml" },
      ipcmn_ { "../../../test-res/ipcmn_params_gen.xml" },
      file_accessor_ {tmp_dir_}
  {

  }

  void SetUp() override
  {
    TestUtils::create_dir(tmp_dir_ + "/etc/firewall/iptables");
    TestUtils::copy_file(params_, tmp_dir_ + "/etc/firewall/params.xml");
    TestUtils::copy_file(ipcmn_, tmp_dir_ + "/etc/firewall/iptables/ipcmn.xml");
  }

  void TearDown() override
  {
    TestUtils::remove_dir(tmp_dir_);
  }

};

TEST_F(ProcessParametersTest, update_xml_params) {
  xmldoc params_gen;
  xmldoc ipcmn;

  const std::string params_gen_file = tmp_dir_ + "/etc/firewall/params_gen.xml";

  ASSERT_FALSE(TestUtils::file_exists(params_gen_file));

  update_network_interface_name_mapping(file_accessor_);

  ASSERT_TRUE(TestUtils::file_exists(params_gen_file));

  // Check expected file content.
  ASSERT_TRUE(TestUtils::is_content_equal("../../../test-res/params_gen.xml", params_gen_file));
}

TEST_F(ProcessParametersTest, params_xml_empty) {
  xmldoc params_gen;
  xmldoc ipcmn;

  // Set empty params.xml file
  TestUtils::write_to_file(tmp_dir_ + "/etc/firewall/params.xml", "");

  const std::string params_gen_file = tmp_dir_ + "/etc/firewall/params_gen.xml";

  ASSERT_FALSE(TestUtils::file_exists(params_gen_file));

  ASSERT_THROW(update_network_interface_name_mapping(file_accessor_), std::runtime_error);

  ASSERT_FALSE(TestUtils::file_exists(params_gen_file));
}

TEST_F(ProcessParametersTest, ipcmn_xml_empty) {
  xmldoc params_gen;
  xmldoc ipcmn;

  // Set empty params.xml file
  TestUtils::write_to_file(tmp_dir_ + "/etc/firewall/iptables/ipcmn.xml", "");

  const std::string params_gen_file = tmp_dir_ + "/etc/firewall/params_gen.xml";

  ASSERT_FALSE(TestUtils::file_exists(params_gen_file));

  update_network_interface_name_mapping(file_accessor_);

  ASSERT_TRUE(TestUtils::file_exists(params_gen_file));

  // Check expected file content.
  ASSERT_TRUE(TestUtils::is_content_equal("../../../test-res/params.xml", params_gen_file));
}

TEST_F(ProcessParametersTest, ipcmn_xml_no_nat_entries) {
  xmldoc params_gen;
  xmldoc ipcmn;

  params_ = "../../../test-res/params.xml";
  ipcmn_ = "../../../test-res/ipcmn_no_nat.xml";
  TestUtils:: copy_file(params_, tmp_dir_ + "/etc/firewall/params.xml");
  TestUtils::copy_file(ipcmn_, tmp_dir_ + "/etc/firewall/iptables/ipcmn.xml");

  const std::string params_gen_file = tmp_dir_ + "/etc/firewall/params_gen.xml";

  ASSERT_FALSE(TestUtils::file_exists(params_gen_file));

  update_network_interface_name_mapping(file_accessor_);

  ASSERT_TRUE(TestUtils::file_exists(params_gen_file));

  // Check expected file content.
  ASSERT_TRUE(TestUtils::is_content_equal("../../../test-res/params.xml", params_gen_file));
}

//---- End of source file ------------------------------------------------------

