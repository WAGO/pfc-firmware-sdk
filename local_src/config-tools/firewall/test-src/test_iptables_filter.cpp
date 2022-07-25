//------------------------------------------------------------------------------
// Copyright (c) WAGO GmbH & Co. KG
//
// PROPRIETARY RIGHTS are involved in the subject matter of this material. All
// manufacturing, reproduction, use and sales rights pertaining to this
// subject matter are governed by the license agreement. The recipient of this
// software implicitly accepts the terms of the license.
//------------------------------------------------------------------------------

#include <gtest/gtest.h>

#include "test_base_iptables.hpp"
#include "process.hpp"
#include "process_iptables.hpp"
#include "firewall_test_interface_state_data.hpp"

using namespace wago;

class IptablesFilterTest : public IptablesTestBase, public testing::WithParamInterface<FirewallTestInterfaceStateData> {

 public:

  bool interface_exists(const xmldoc &doc, const ::std::string &interface_name) {

    xmlctx ctx(get_ctx(doc));
    auto node = get_node(ctx, create_xpath_to_get(interface_name), false);

    return !node.is_empty();
  }

  FirewallTestInterfaceStateData get_iptables_processing_data(const xmldoc &doc, const ::std::string interface_name) {

    FirewallTestInterfaceStateData data;
    xmlctx ctx(get_ctx(doc));

    data.interface_ = interface_name;
    data.second_interface_ = "";

    data.state_ = get_string(ctx, "string(" + create_xpath_to_get(interface_name) + "/@state)", false);

    return data;
  }

  ::std::string create_xpath_to_get(::std::string interface_name) {
    return "/f:firewall/f:ipv4/f:input/f:filter/f:rule[@if='" + interface_name + "']";
  }
};

namespace {
// Name has to be unique over all tests.
::std::string generate_test_name(testing::TestParamInfo<FirewallTestInterfaceStateData> data) {

  return data.param.interface_ + data.param.state_ + data.param.second_interface_;
}
}

// @formatter:off
INSTANTIATE_TEST_CASE_P(InstantiationName, IptablesFilterTest, testing::Values(
    FirewallTestInterfaceStateData { "X1","on","br0" },
    FirewallTestInterfaceStateData { "X2","on","br1" },
    FirewallTestInterfaceStateData { "br0","on","br0" },
    FirewallTestInterfaceStateData { "br1","on","br1" },
    FirewallTestInterfaceStateData { "br2","on","br2" },
    FirewallTestInterfaceStateData { "br3","on","br3" }
    ), generate_test_name);
// @formatter:on
//

TEST_P(IptablesFilterTest, SetOpenInterface) {
  auto data = GetParam();

  ::std::vector<::std::string> argv = { data.state_, data.interface_, "tcp", "-", "-", "12345", "-", "-", "54321",
      "accept" };

  auto doc = file_accessor_.read_configuration("iptables", false);
  ASSERT_NO_THROW(iptables::impl::add_filter(doc, argv));

  auto current_data = get_iptables_processing_data(doc, data.second_interface_);
  ASSERT_EQ(data.state_, current_data.state_);
}

TEST_P(IptablesFilterTest, UpdateOpenInterface) {
  auto data = GetParam();

  ::std::vector<::std::string> argv = { data.state_, data.interface_, "tcp", "-", "-", "12345", "-", "-", "54321",
      "accept" };
  auto doc = file_accessor_.read_configuration("iptables", false);
  iptables::impl::add_filter(doc, argv);

  auto changed_state = (data.state_ == "on") ? "off" : "on";
  argv = { "1", changed_state, data.interface_, "tcp", "-", "-", "12345", "-", "-", "54321" };

  ASSERT_NO_THROW(iptables::impl::upd_filter(doc, argv));
  auto current_data = get_iptables_processing_data(doc, data.second_interface_);
  ASSERT_EQ(changed_state, current_data.state_);
}
