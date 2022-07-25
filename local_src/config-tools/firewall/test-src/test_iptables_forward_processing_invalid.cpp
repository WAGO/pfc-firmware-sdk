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
#include "error.hpp"

using namespace wago;

class IptablesForwardProcessingTestInvalid : public IptablesForwardProcessingTestBase, public testing::WithParamInterface<
    IptablesForwardProcessingData> {
};

// @formatter:off
INSTANTIATE_TEST_CASE_P(InstantiationName, IptablesForwardProcessingTestInvalid, testing::Values(
    IptablesForwardProcessingData { "br0", "br0", "on", "", "" },
    IptablesForwardProcessingData { "br1", "br1", "on", "", "" },
    IptablesForwardProcessingData { "br2", "br2", "on", "", "" },
    IptablesForwardProcessingData { "X1", "br0", "on", "", "" },
    IptablesForwardProcessingData { "X1", "br1", "on", "", "" },
    IptablesForwardProcessingData { "X2", "br0", "on", "", "" },
    IptablesForwardProcessingData { "X2", "br1", "on", "", "" }
    ), IptablesForwardProcessingTestInvalid::generate_test_name);
// @formatter:on
//

TEST_P(IptablesForwardProcessingTestInvalid, SetForwardState) {
  auto data = GetParam();

  ::std::vector<::std::string> argv = { data.state_, data.interface1_, data.interface2_ };

  auto doc = file_accessor_.read_configuration("iptables", false);
  ASSERT_THROW(iptables::impl::set_forward_link(doc, argv), invalid_param_error);
}

TEST_P(IptablesForwardProcessingTestInvalid, RemoveForwardState) {
  auto data = GetParam();

  ::std::vector<::std::string> argv = { data.interface1_, data.interface2_ };

  auto doc = file_accessor_.read_configuration("iptables", false);
  ASSERT_THROW(iptables::impl::rem_forward_link(doc, argv), invalid_param_error);
}
