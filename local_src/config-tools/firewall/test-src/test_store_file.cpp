//------------------------------------------------------------------------------
// Copyright (c) WAGO Kontakttechnik GmbH & Co. KG
//
// PROPRIETARY RIGHTS are involved in the subject matter of this material. All
// manufacturing, reproduction, use and sales rights pertaining to this
// subject matter are governed by the license agreement. The recipient of this
// software implicitly accepts the terms of the license.
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
///  \file     test_store_file.cpp
///
///  \brief    Test xml file persistence.
///
///  \author   WAGO Kontakttechnik GmbH & Co. KG
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// include files
//------------------------------------------------------------------------------
#include "file_accessor.hpp"
#include <gtest/gtest.h>
#include <string>

#include "xmlhlp.hpp"
#include "error.hpp"
#include "test_utils.hpp"

using namespace wago;

class StoreFileTest : public ::testing::Test {
 public:
  std::string tmp_dir_;
  std::string params_;
  std::string ipcmn_;
  FileAccessor file_accessor_;

  StoreFileTest()
      :
      tmp_dir_(TestUtils::create_temp_dir("firewall_")),
      params_("../../../test-res/params.xml"),
      ipcmn_("../../../test-res/ipcmn.xml"),
      file_accessor_ (tmp_dir_) {

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

// -----------------------------------------------------------------------------

TEST_F(StoreFileTest, store_ipcmn_xml) {
  const std::string ipcmn_file = tmp_dir_ + "/etc/firewall/iptables/ipcmn.xml";

  ASSERT_TRUE(TestUtils::file_exists(ipcmn_file));

  xmldoc doc = file_accessor_.read_configuration("iptables", false);
  if (!doc.is_empty()) {
    file_accessor_.store_configuration("iptables", false, doc);
  }

  // Check expected file content.
  ASSERT_TRUE(TestUtils::content_is_equal("../../../test-res/ipcmn.xml", ipcmn_file));
}

TEST_F(StoreFileTest, store_ipcmn_xml_tmp_already_exists) {
  const std::string ipcmn_file = tmp_dir_ + "/etc/firewall/iptables/ipcmn.xml";
  const std::string ipcmn_file_tmp = tmp_dir_ + "/etc/firewall/iptables/ipcmn.xml.tmp";
  TestUtils::write_to_file(ipcmn_file_tmp, "foo");

  ASSERT_TRUE(TestUtils::file_exists(ipcmn_file));
  ASSERT_TRUE(TestUtils::file_exists(ipcmn_file_tmp));

  xmldoc doc = file_accessor_.read_configuration("iptables", false);
  if (!doc.is_empty()) {
    file_accessor_.store_configuration("iptables", false, doc);
  }

  ASSERT_FALSE(TestUtils::file_exists(ipcmn_file_tmp));

  // Check expected file content.
  ASSERT_TRUE(TestUtils::content_is_equal("../../../test-res/ipcmn.xml", ipcmn_file));
}

//---- End of source file ------------------------------------------------------

