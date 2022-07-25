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
#include "test_utils.hpp"
#include <boost/algorithm/string.hpp>

using namespace wago::firewall;

namespace {
const ::std::string test_res_dir = "../../../test-res";
const ::std::string params_file = test_res_dir + "/params.xml";
}

class TestFiles {
 public:
  TestFiles();
  TestFiles(::std::string ebtables, ::std::string iptables, std::string service)
      :
      ebtables_ { ebtables },
      iptables_ { iptables },
      service_ { service } {
  }
  virtual ~TestFiles() = default;
  TestFiles(const TestFiles &other) = default;
  TestFiles(TestFiles &&other) = default;
  TestFiles& operator=(const TestFiles &other) = default;
  TestFiles& operator=(TestFiles &&other) = default;

  ::std::string ebtables_;
  ::std::string iptables_;
  ::std::string service_;
};

class FileAccessorTest : public ::testing::Test, public testing::WithParamInterface<TestFiles> {

 public:

  ::std::string tmp_dir_;
  ::std::vector<::std::string> ipcmn_files_;
  FileAccessor file_accessor_;

  FileAccessorTest()
      :
      tmp_dir_ { TestUtils::create_temp_dir("firewall_") },
      ipcmn_files_ { test_res_dir + "/ipcmn_all_entries_in_one_line.xml", test_res_dir + "/ipcmn_all_entries.xml" },
      file_accessor_ { tmp_dir_ } {
  }

  void SetUp() override {
    TestUtils::create_dir(tmp_dir_ + "/etc/firewall/ebtables");
    TestUtils::create_dir(tmp_dir_ + "/etc/firewall/iptables");
    TestUtils::create_dir(tmp_dir_ + "/etc/firewall/services");

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

  void expect_contains(const ::std::string needle, const ::std::string haystack) {
    EXPECT_NE(haystack.find(needle), ::std::string::npos) << "Must contain: " << needle;
  }

  void expect_does_not_contain(const ::std::string needle, const ::std::string haystack) {
    EXPECT_EQ(haystack.find(needle), ::std::string::npos) << "Must not contain: " << needle;
  }
};

namespace {

::std::string remove_dot_and_underscore(const ::std::string &name) {
  ::std::string cleared_name = name;
  ::boost::algorithm::replace_all(cleared_name, ".", "");
  ::boost::algorithm::replace_all(cleared_name, "_", "");

  return cleared_name;
}

// Name has to be unique over all tests.
::std::string generate_test_name(testing::TestParamInfo<TestFiles> data) {

  return remove_dot_and_underscore(data.param.ebtables_) + "I" + remove_dot_and_underscore(data.param.iptables_) + "I"
      + remove_dot_and_underscore(data.param.service_);
}
}
// @formatter:off
INSTANTIATE_TEST_CASE_P(InstantiationName, FileAccessorTest, testing::Values(
    TestFiles { "ebwlist.xml",
                "ipcmn_all_entries.xml",
                "dummy_service.xml" },
    TestFiles { "ebwlist_single_line.xml",
                "ipcmn_all_entries_single_line.xml",
                "dummy_service_single_line.xml" }
    ), generate_test_name);
// @formatter:on
//

TEST_P(FileAccessorTest, printIpTablesConfig) {

  TestUtils::copy_file(test_res_dir + "/" + GetParam().iptables_, tmp_dir_ + "/etc/firewall/iptables/ipcmn.xml");

  auto outString = get_file_content("iptables");
  expect_does_not_contain("<request policy=\"accept\" if=\"br0\" limit=\"2/second\"/>", outString);
  expect_does_not_contain("<request policy=\"accept\" if=\"br1\" limit=\"2/second\"/>", outString);

  expect_contains("<request policy=\"accept\" if=\"X1\" limit=\"2/second\"/>", outString);
  expect_contains("<request policy=\"accept\" if=\"X2\" limit=\"2/second\"/>", outString);
  expect_contains("<request policy=\"accept\" if=\"br2\" limit=\"2/second\"/>", outString);
  expect_contains("<request policy=\"accept\" if=\"br3\" limit=\"2/second\"/>", outString);
  expect_contains("<request policy=\"accept\" if=\"VPN\" limit=\"2/second\"/>", outString);
  expect_contains("<request policy=\"drop\" if=\"WAN\" limit=\"2/second\"/>", outString);

  expect_does_not_contain("<request policy=\"accept\" if=\"X3\" limit=\"2/second\"/>", outString);

  // Check forwarding
  expect_does_not_contain("<link state=\"on\" if1=\"br0\" if2=\"br1\"/>", outString);
  expect_does_not_contain("<link state=\"on\" if1=\"br0\" if2=\"WAN\"/>", outString);

  expect_contains("<link state=\"on\" if1=\"X1\" if2=\"X2\"/>", outString);
  expect_contains("<link state=\"on\" if1=\"X1\" if2=\"WAN\"/>", outString);
}

TEST_P(FileAccessorTest, printEbTablesConfig) {

  TestUtils::copy_file(test_res_dir + "/" + GetParam().ebtables_, tmp_dir_ + "/etc/firewall/ebtables/ebwlist.xml");

  auto outString = get_file_content("ebtables");
  expect_does_not_contain("<interface if=\"br0\" state=\"open\"/>", outString);
  expect_does_not_contain("<interface if=\"br1\" state=\"open\"/>", outString);

  expect_contains("<interface if=\"X1\" state=\"open\"/>", outString);
  expect_contains("<interface if=\"X2\" state=\"open\"/>", outString);

  expect_does_not_contain("<interface if=\"X3\" state=\"open\"/>", outString);
}

TEST_P(FileAccessorTest, printServiceConfig) {

  TestUtils::copy_file(test_res_dir + "/services/" + GetParam().service_,
                       tmp_dir_ + "/etc/firewall/services/dummy_service.xml");

  auto outString = get_file_content("dummy_service");
  expect_does_not_contain("<interface state=\"on\"  if=\"br0\"/>", outString);
  expect_does_not_contain("<interface state=\"on\"  if=\"br1\"/>", outString);

  expect_contains("<interface state=\"on\"  if=\"X1\"/>", outString);
  expect_contains("<interface state=\"on\"  if=\"X2\"/>", outString);
  expect_contains("<interface state=\"on\"  if=\"br2\"/>", outString);
  expect_contains("<interface state=\"on\"  if=\"br3\"/>", outString);
  expect_contains("<interface state=\"off\" if=\"WAN\"/>", outString);
  expect_contains("<interface state=\"on\"  if=\"VPN\"/>", outString);
}

TEST_P(FileAccessorTest, printIpTablesConfigNextGeneration) {

  TestUtils::copy_file(test_res_dir + "/" + GetParam().iptables_, tmp_dir_ + "/etc/firewall/iptables/ipcmn.xml");

  auto outString = get_file_content_ng("iptables");
  expect_does_not_contain("<request policy=\"accept\" if=\"X1\" limit=\"2/second\"/>", outString);
  expect_does_not_contain("<request policy=\"accept\" if=\"X2\" limit=\"2/second\"/>", outString);

  expect_contains("<request policy=\"accept\" if=\"br0\" limit=\"2/second\"/>", outString);
  expect_contains("<request policy=\"accept\" if=\"br1\" limit=\"2/second\"/>", outString);
  expect_contains("<request policy=\"accept\" if=\"br2\" limit=\"2/second\"/>", outString);
  expect_contains("<request policy=\"accept\" if=\"br3\" limit=\"2/second\"/>", outString);
  expect_contains("<request policy=\"accept\" if=\"VPN\" limit=\"2/second\"/>", outString);
  expect_contains("<request policy=\"drop\" if=\"WAN\" limit=\"2/second\"/>", outString);

  expect_does_not_contain("<request policy=\"accept\" if=\"X3\" limit=\"2/second\"/>", outString);

}

TEST_P(FileAccessorTest, printEbTablesConfigNextGeneration) {

  TestUtils::copy_file(test_res_dir + "/" + GetParam().ebtables_, tmp_dir_ + "/etc/firewall/ebtables/ebwlist.xml");

  auto outString = get_file_content_ng("ebtables");
  expect_does_not_contain("<interface if=\"X1\" state=\"open\"/>", outString);
  expect_does_not_contain("<interface if=\"X2\" state=\"open\"/>", outString);

  expect_contains("<interface if=\"br0\" state=\"open\"/>", outString);
  expect_contains("<interface if=\"br1\" state=\"open\"/>", outString);

  expect_does_not_contain("<interface if=\"X3\" state=\"open\"/>", outString);
}

TEST_P(FileAccessorTest, printServiceConfigNextGeneration) {

  TestUtils::copy_file(test_res_dir + "/services/" + GetParam().service_,
                       tmp_dir_ + "/etc/firewall/services/dummy_service.xml");

  auto outString = get_file_content_ng("dummy_service");
  expect_does_not_contain("<interface state=\"on\"  if=\"X1\"/>", outString);
  expect_does_not_contain("<interface state=\"on\"  if=\"X2\"/>", outString);

  expect_contains("<interface state=\"on\"  if=\"br0\"/>", outString);
  expect_contains("<interface state=\"on\"  if=\"br1\"/>", outString);
  expect_contains("<interface state=\"on\"  if=\"br2\"/>", outString);
  expect_contains("<interface state=\"on\"  if=\"br3\"/>", outString);
  expect_contains("<interface state=\"off\" if=\"WAN\"/>", outString);
  expect_contains("<interface state=\"on\"  if=\"VPN\"/>", outString);
}
