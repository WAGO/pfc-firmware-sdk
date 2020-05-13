//------------------------------------------------------------------------------
// Copyright (c) WAGO Kontakttechnik GmbH & Co. KG
//
// PROPRIETARY RIGHTS are involved in the subject matter of this material. All
// manufacturing, reproduction, use and sales rights pertaining to this
// subject matter are governed by the license agreement. The recipient of this
// software implicitly accepts the terms of the license.
//------------------------------------------------------------------------------

#include <gtest/gtest.h>

#include "file_accessor.hpp"
#include "test_utils.hpp"

using namespace wago;

static ::std::string test_res_dir = "../../../test-res";
static ::std::string params_file = test_res_dir + "/params.xml";
static ::std::string ebwlist_file = test_res_dir + "/ebwlist.xml";
static ::std::string dummy_service_file = test_res_dir + "/services/dummy_service.xml";
static ::std::string ipcmn_file = test_res_dir + "/ipcmn_all_entries.xml";

class FileAccessorTest : public ::testing::Test {

 public:

  ::std::string tmp_dir_;
  FileAccessor file_accessor_;

  FileAccessorTest()
      :
      tmp_dir_ { TestUtils::create_temp_dir("firewall_") },
      file_accessor_ { tmp_dir_ } {
  }

  void SetUp() override {
    TestUtils::create_dir(tmp_dir_ + "/etc/firewall/ebtables");
    TestUtils::copy_file(ebwlist_file, tmp_dir_ + "/etc/firewall/ebtables/ebwlist.xml");

    TestUtils::create_dir(tmp_dir_ + "/etc/firewall/iptables");
    TestUtils::copy_file(ipcmn_file, tmp_dir_ + "/etc/firewall/iptables/ipcmn.xml");

    TestUtils::create_dir(tmp_dir_ + "/etc/firewall/services");
    TestUtils::copy_file(dummy_service_file, tmp_dir_ + "/etc/firewall/services/dummy_service.xml");

    TestUtils::copy_file(params_file, tmp_dir_ + "/etc/firewall/params.xml");
  }

  void TearDown() override {
    TestUtils::remove_dir(tmp_dir_);
  }

  ::std::string get_file_content(const ::std::string &fname) {
    ::std::stringstream out;
    file_accessor_.print_file(file_accessor_.get_config_fname(fname), out);
    ::std::string outString = out.str();
    return outString;
  }

  ::std::string get_file_content_ng(const ::std::string &fname) {
    ::std::stringstream out;
    file_accessor_.print_file_ng(file_accessor_.get_config_fname(fname), out);
    ::std::string outString = out.str();
    return outString;
  }
};

TEST_F(FileAccessorTest, printIpTablesConfig) {

  auto outString = get_file_content("iptables");
  EXPECT_EQ(outString.find("<request policy=\"accept\" if=\"br0\" limit=\"2/second\"/>"), ::std::string::npos);
  EXPECT_EQ(outString.find("<request policy=\"accept\" if=\"br1\" limit=\"2/second\"/>"), ::std::string::npos);

  EXPECT_NE(outString.find("<request policy=\"accept\" if=\"X1\" limit=\"2/second\"/>"), ::std::string::npos);
  EXPECT_NE(outString.find("<request policy=\"accept\" if=\"X2\" limit=\"2/second\"/>"), ::std::string::npos);
  EXPECT_NE(outString.find("<request policy=\"accept\" if=\"br2\" limit=\"2/second\"/>"), ::std::string::npos);
  EXPECT_NE(outString.find("<request policy=\"accept\" if=\"br3\" limit=\"2/second\"/>"), ::std::string::npos);
  EXPECT_NE(outString.find("<request policy=\"accept\" if=\"VPN\" limit=\"2/second\"/>"), ::std::string::npos);
  EXPECT_NE(outString.find("<request policy=\"drop\" if=\"WAN\" limit=\"2/second\"/>"), ::std::string::npos);

  EXPECT_EQ(outString.find("<request policy=\"accept\" if=\"X3\" limit=\"2/second\"/>"), ::std::string::npos);
}

TEST_F(FileAccessorTest, printEbTablesConfig) {

  auto outString = get_file_content("ebtables");
  EXPECT_EQ(outString.find("<interface if=\"br0\" state=\"open\"/>"), ::std::string::npos);
  EXPECT_EQ(outString.find("<interface if=\"br1\" state=\"open\"/>"), ::std::string::npos);

  EXPECT_NE(outString.find("<interface if=\"X1\" state=\"open\"/>"), ::std::string::npos);
  EXPECT_NE(outString.find("<interface if=\"X2\" state=\"open\"/>"), ::std::string::npos);

  EXPECT_EQ(outString.find("<interface if=\"X3\" state=\"open\"/>"), ::std::string::npos);
}

TEST_F(FileAccessorTest, printServiceConfig) {

  auto outString = get_file_content("dummy_service");
  EXPECT_EQ(outString.find("<interface state=\"on\"  if=\"br0\"/>"), ::std::string::npos);
  EXPECT_EQ(outString.find("<interface state=\"on\"  if=\"br1\"/>"), ::std::string::npos);

  EXPECT_NE(outString.find("<interface state=\"on\"  if=\"X1\"/>"), ::std::string::npos);
  EXPECT_NE(outString.find("<interface state=\"on\"  if=\"X2\"/>"), ::std::string::npos);
  EXPECT_NE(outString.find("<interface state=\"on\"  if=\"br2\"/>"), ::std::string::npos);
  EXPECT_NE(outString.find("<interface state=\"on\"  if=\"br3\"/>"), ::std::string::npos);
  EXPECT_NE(outString.find("<interface state=\"off\" if=\"WAN\"/>"), ::std::string::npos);
  EXPECT_NE(outString.find("<interface state=\"on\"  if=\"VPN\"/>"), ::std::string::npos);
}


TEST_F(FileAccessorTest, printIpTablesConfigNextGeneration) {

  auto outString = get_file_content_ng("iptables");
  EXPECT_EQ(outString.find("<request policy=\"accept\" if=\"X1\" limit=\"2/second\"/>"), ::std::string::npos);
  EXPECT_EQ(outString.find("<request policy=\"accept\" if=\"X2\" limit=\"2/second\"/>"), ::std::string::npos);

  EXPECT_NE(outString.find("<request policy=\"accept\" if=\"br0\" limit=\"2/second\"/>"), ::std::string::npos);
  EXPECT_NE(outString.find("<request policy=\"accept\" if=\"br1\" limit=\"2/second\"/>"), ::std::string::npos);
  EXPECT_NE(outString.find("<request policy=\"accept\" if=\"br2\" limit=\"2/second\"/>"), ::std::string::npos);
  EXPECT_NE(outString.find("<request policy=\"accept\" if=\"br3\" limit=\"2/second\"/>"), ::std::string::npos);
  EXPECT_NE(outString.find("<request policy=\"accept\" if=\"VPN\" limit=\"2/second\"/>"), ::std::string::npos);
  EXPECT_NE(outString.find("<request policy=\"drop\" if=\"WAN\" limit=\"2/second\"/>"), ::std::string::npos);

  EXPECT_EQ(outString.find("<request policy=\"accept\" if=\"X3\" limit=\"2/second\"/>"), ::std::string::npos);
}

TEST_F(FileAccessorTest, printEbTablesConfigNextGeneration) {

  auto outString = get_file_content_ng("ebtables");
  EXPECT_EQ(outString.find("<interface if=\"X1\" state=\"open\"/>"), ::std::string::npos);
  EXPECT_EQ(outString.find("<interface if=\"X2\" state=\"open\"/>"), ::std::string::npos);

  EXPECT_NE(outString.find("<interface if=\"br0\" state=\"open\"/>"), ::std::string::npos);
  EXPECT_NE(outString.find("<interface if=\"br1\" state=\"open\"/>"), ::std::string::npos);

  EXPECT_EQ(outString.find("<interface if=\"X3\" state=\"open\"/>"), ::std::string::npos);
}

TEST_F(FileAccessorTest, printServiceConfigNextGeneration) {

  auto outString = get_file_content_ng("dummy_service");
  EXPECT_EQ(outString.find("<interface state=\"on\"  if=\"X1\"/>"), ::std::string::npos);
  EXPECT_EQ(outString.find("<interface state=\"on\"  if=\"X2\"/>"), ::std::string::npos);

  EXPECT_NE(outString.find("<interface state=\"on\"  if=\"br0\"/>"), ::std::string::npos);
  EXPECT_NE(outString.find("<interface state=\"on\"  if=\"br1\"/>"), ::std::string::npos);
  EXPECT_NE(outString.find("<interface state=\"on\"  if=\"br2\"/>"), ::std::string::npos);
  EXPECT_NE(outString.find("<interface state=\"on\"  if=\"br3\"/>"), ::std::string::npos);
  EXPECT_NE(outString.find("<interface state=\"off\" if=\"WAN\"/>"), ::std::string::npos);
  EXPECT_NE(outString.find("<interface state=\"on\"  if=\"VPN\"/>"), ::std::string::npos);
}
