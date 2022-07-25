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

#include "test_base_iptables_forward_processing.hpp"
#include "process.hpp"
#include "process_iptables.hpp"
#include "test_utils.hpp"

using namespace wago;

class IptablesForwardProcessingTest : public IptablesForwardProcessingTestBase, public testing::WithParamInterface<
    IptablesForwardProcessingData> {
};

// @formatter:off
INSTANTIATE_TEST_CASE_P(InstantiationName, IptablesForwardProcessingTest, testing::Values(
    IptablesForwardProcessingData { "X1","X2","on","br0", "br1" },
    IptablesForwardProcessingData { "br0","br1","on","br0", "br1" },

    IptablesForwardProcessingData { "WAN","X1","on","WAN", "br0" },
    IptablesForwardProcessingData { "X1", "WAN", "on","br0", "WAN" },
    IptablesForwardProcessingData { "WAN","X2","on","WAN", "br1" },
    IptablesForwardProcessingData { "X2", "WAN", "on","br1", "WAN" }
    ), IptablesForwardProcessingTest::generate_test_name);
// @formatter:on
//

TEST_P(IptablesForwardProcessingTest, SetForwardState) {
  auto data = GetParam();

  ::std::vector<::std::string> argv = { data.state_, data.interface1_, data.interface2_ };

  auto doc = file_accessor_.read_configuration("iptables", false);
  ASSERT_NO_THROW(iptables::impl::set_forward_link(doc, argv));

  auto current_data = get_iptables_processing_data(doc, data.second_interface1_, data.second_interface2_);
  ASSERT_EQ(data.state_, current_data.state_);
}

TEST_P(IptablesForwardProcessingTest, RemoveForwardState) {
  auto data = GetParam();

  ::std::vector<::std::string> argv = { data.state_, data.interface1_, data.interface2_ };
  auto doc = file_accessor_.read_configuration("iptables", false);
  iptables::impl::set_forward_link(doc, argv);

  argv = { data.interface1_, data.interface2_ };

  doc = file_accessor_.read_configuration("iptables", false);
  ASSERT_NO_THROW(iptables::impl::rem_forward_link(doc, argv));

  ASSERT_FALSE(interface_exists(doc, data.interface1_, data.interface2_));
}
