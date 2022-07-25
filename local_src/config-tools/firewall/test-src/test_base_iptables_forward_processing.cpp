//------------------------------------------------------------------------------
// Copyright (c) WAGO GmbH & Co. KG
//
// PROPRIETARY RIGHTS are involved in the subject matter of this material. All
// manufacturing, reproduction, use and sales rights pertaining to this
// subject matter are governed by the license agreement. The recipient of this
// software implicitly accepts the terms of the license.
//------------------------------------------------------------------------------

#include "test_base_iptables_forward_processing.hpp"

#include "process.hpp"
#include "test_utils.hpp"

using namespace wago;

bool IptablesForwardProcessingTestBase::interface_exists(const xmldoc &doc, const ::std::string &ifname1,
                                                         const ::std::string &ifname2) {

  xmlctx ctx(get_ctx(doc));
  auto node = get_node(ctx, create_xpath_to_get(ifname1, ifname2), false);

  return !node.is_empty();
}

IptablesForwardProcessingData IptablesForwardProcessingTestBase::get_iptables_processing_data(
    const xmldoc &doc, const ::std::string &ifname1, const ::std::string &ifname2) {

  IptablesForwardProcessingData data;
  xmlctx ctx(get_ctx(doc));

  data.interface1_ = ifname1;
  data.interface2_ = ifname2;
  data.state_ = get_string(ctx, "string(" + create_xpath_to_get(ifname1, ifname2) + "/@state)", false);

  return data;
}

::std::string IptablesForwardProcessingTestBase::create_xpath_to_get(const ::std::string &ifname1,
                                                                  const ::std::string &ifname2) {
  return "/f:firewall/f:ipv4/f:forward/f:fw/f:link[@if1='" + ifname1 + "' and @if2='" + ifname2 + "']";
}

// Hint: Name has to be unique over all tests.
::std::string IptablesForwardProcessingTestBase::generate_test_name(
    testing::TestParamInfo<IptablesForwardProcessingData> data) {
  return data.param.interface1_ + data.param.interface2_ + data.param.state_ + data.param.second_interface1_
      + data.param.second_interface2_;
}
