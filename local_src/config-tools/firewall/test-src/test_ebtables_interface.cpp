//------------------------------------------------------------------------------
// Copyright (c) WAGO GmbH & Co. KG
//
// PROPRIETARY RIGHTS are involved in the subject matter of this material. All
// manufacturing, reproduction, use and sales rights pertaining to this
// subject matter are governed by the license agreement. The recipient of this
// software implicitly accepts the terms of the license.
//------------------------------------------------------------------------------

#include <gtest/gtest.h>

#include "test_base_ebtables.hpp"
#include "process.hpp"
#include "process_ebtables.hpp"
#include "firewall_test_interface_state_data.hpp"

using namespace wago;

class EbtablesInterfaceTest : public EbtablesTestBase,
    public testing::WithParamInterface<FirewallTestInterfaceStateData> {

 public:

  bool interface_exists(const xmldoc &doc, const ::std::string &interface_name) {

    xmlctx ctx(get_ctx(doc));
    auto node = get_node(ctx, create_xpath_to_get(interface_name), false);

    return !node.is_empty();
  }

  FirewallTestInterfaceStateData get_ebtables_processing_data(const xmldoc &doc, const ::std::string interface_name) {

    FirewallTestInterfaceStateData data;
    xmlctx ctx(get_ctx(doc));

    data.interface_ = interface_name;
    data.second_interface_ = "";

    data.state_ = get_string(ctx, "string(" + create_xpath_to_get(interface_name) + "/@state)", false);

    return data;
  }

  ::std::string create_xpath_to_get(::std::string interface_name) {
    return "/f:firewall/f:ethernet/f:interfaces/f:interface[@if='" + interface_name + "']";
  }
};

namespace {

// Name has to be unique over all tests.
::std::string generate_test_name(testing::TestParamInfo<FirewallTestInterfaceStateData> data) {

  return data.param.interface_ + data.param.state_ + data.param.second_interface_;
}

}
// @formatter:off
INSTANTIATE_TEST_CASE_P(InstantiationName, EbtablesInterfaceTest, testing::Values(
    FirewallTestInterfaceStateData { "X1","filtered","br0" },
    FirewallTestInterfaceStateData { "X2","filtered","br1" },
    FirewallTestInterfaceStateData { "br0","filtered","br0" },
    FirewallTestInterfaceStateData { "br1","filtered","br1" },
    FirewallTestInterfaceStateData { "br2","filtered","br2" },
    FirewallTestInterfaceStateData { "br3","filtered","br3" }
    ), generate_test_name);
// @formatter:on
//

TEST_P(EbtablesInterfaceTest, SetInterface) {
  auto data = GetParam();

  ::std::vector<::std::string> argv = { data.state_, data.interface_ };

  auto doc = file_accessor_.read_configuration("ebtables", false);
  ASSERT_NO_THROW(ebtables::impl::set_if(doc, argv));

  auto current_data = get_ebtables_processing_data(doc, data.second_interface_);
  ASSERT_EQ(data.state_, current_data.state_);
}

TEST_P(EbtablesInterfaceTest, RemInterface) {
  auto data = GetParam();

  ::std::vector<::std::string> argv = { data.interface_ };

  auto doc = file_accessor_.read_configuration("ebtables", false);
  ASSERT_NO_THROW(ebtables::impl::rem_if(doc, argv));

  auto current_data = get_ebtables_processing_data(doc, data.second_interface_);
  ASSERT_FALSE(interface_exists(doc, data.second_interface_));
}
