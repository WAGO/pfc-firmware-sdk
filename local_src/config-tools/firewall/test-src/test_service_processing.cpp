//------------------------------------------------------------------------------
// Copyright (c) WAGO GmbH & Co. KG
//
// PROPRIETARY RIGHTS are involved in the subject matter of this material. All
// manufacturing, reproduction, use and sales rights pertaining to this
// subject matter are governed by the license agreement. The recipient of this
// software implicitly accepts the terms of the license.
//------------------------------------------------------------------------------

#include "file_accessor.hpp"
#include <gtest/gtest.h>

#include "process.hpp"
#include "process_service.hpp"
#include "test_utils.hpp"
#include "firewall_test_interface_state_data.hpp"

using namespace wago::firewall;

namespace {
const ::std::string test_res_dir = "../../../test-res";
const ::std::string params_file = test_res_dir + "/params.xml";
const ::std::string ipcmn_file = test_res_dir + "/ipcmn.xml";
const ::std::string dummy_service_file = test_res_dir + "/services/dummy_service.xml";
}

class SeviceProcessingTest : public ::testing::Test, public testing::WithParamInterface<FirewallTestInterfaceStateData> {

 public:
  ::std::string tmp_dir_;
  FileAccessor file_accessor_;

  SeviceProcessingTest()
      :
      tmp_dir_ { TestUtils::create_temp_dir("firewall_") },
      file_accessor_ { tmp_dir_ } {
  }

  void SetUp() override
  {
    TestUtils::create_dir(tmp_dir_ + "/etc/firewall/iptables");
    TestUtils::create_dir(tmp_dir_ + "/etc/firewall/services");
    TestUtils::copy_file(params_file, tmp_dir_ + "/etc/firewall/params.xml");
    TestUtils::copy_file(ipcmn_file, tmp_dir_ + "/etc/firewall/iptables/ipcmn.xml");
    TestUtils::copy_file(dummy_service_file, tmp_dir_ + "/etc/firewall/services/dummy_service.xml");
  }

  void TearDown() override
  {
    TestUtils::remove_dir(tmp_dir_);
  }

  ::std::string get_interface_state(const xmldoc &doc, const ::std::string &interface_name) {

    xmlctx ctx(get_ctx(doc));
    auto state = get_string(
        ctx, "string(/f:firewall/f:ipv4/f:service/f:interfaces/f:interface[@if='" + interface_name + "']/@state)",
        true);
    return state;
  }

  bool interface_exists(const xmldoc &doc, const ::std::string &interface_name) {

    xmlctx ctx(get_ctx(doc));
    auto node = get_node(ctx, "/f:firewall/f:ipv4/f:service/f:interfaces/f:interface[@if='" + interface_name + "']",
                         false);

    return !node.is_empty();
  }
};

namespace {
::std::string generate_test_name(testing::TestParamInfo<FirewallTestInterfaceStateData> data) {
  return data.param.interface_ + data.param.state_ + data.param.second_interface_;
}
}

// @formatter:off
INSTANTIATE_TEST_CASE_P(
    InstantiationName,
    SeviceProcessingTest,
    testing::Values(FirewallTestInterfaceStateData { "X1", "off", "br0" },
                    FirewallTestInterfaceStateData { "X2", "off", "br1" },
                    FirewallTestInterfaceStateData { "br2", "off", "br2" },
                    FirewallTestInterfaceStateData { "br3", "off", "br3" },
                    FirewallTestInterfaceStateData { "VPN", "off", "VPN" }),
    generate_test_name);
// @formatter:on
//

TEST_F(SeviceProcessingTest, AddNotExistingInterface) {
  ::std::vector<::std::string> argv = { "on", "dummy" };

  auto doc = file_accessor_.read_configuration("dummy_service", false);
  ASSERT_NO_THROW(service::impl::set_if(doc, argv));

  auto state = get_interface_state(doc, "dummy");
  ASSERT_EQ("on", state);
}

TEST_P(SeviceProcessingTest, EnableRuleForAnInterface) {
  auto data = GetParam();

  ::std::vector<::std::string> argv = { data.state_, data.interface_ };

  auto doc = file_accessor_.read_configuration("dummy_service", false);
  ASSERT_NO_THROW(service::impl::set_if(doc, argv));

  auto state = get_interface_state(doc, data.second_interface_);
  ASSERT_EQ(data.state_, state);
}

TEST_P(SeviceProcessingTest, RemoveRuleForAnInterface) {
  auto data = GetParam();

  ::std::vector<::std::string> argv = { data.interface_ };

  auto doc = file_accessor_.read_configuration("dummy_service", false);
  ASSERT_NO_THROW(service::impl::rem_if(doc, argv));

  ASSERT_FALSE(interface_exists(doc, data.interface_));
  ASSERT_FALSE(interface_exists(doc, data.second_interface_));
}

