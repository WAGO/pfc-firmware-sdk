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

using namespace wago;

struct IptablesProcessingData {
  IptablesProcessingData()
      :
      interface_ { },
      policy_ { },
      limit_ { },
      burst_ { },
      second_interface_ { } {
  }

  IptablesProcessingData(::std::string interface, ::std::string policy, ::std::string limit, ::std::string burst,
                         ::std::string second_interface)
      :
      interface_ { interface },
      policy_ { policy },
      limit_ { limit },
      burst_ { burst },
      second_interface_ { second_interface } {
  }

  ::std::string interface_;
  ::std::string policy_;
  ::std::string limit_;
  ::std::string burst_;
  ::std::string second_interface_;
};

class IptablesProcessingTest : public IptablesTestBase, public testing::WithParamInterface<IptablesProcessingData> {

 public:

  bool interface_exists(const xmldoc &doc, const ::std::string &interface_name) {

    xmlctx ctx(get_ctx(doc));
    auto node = get_node(ctx, create_xpath_to_get(interface_name), false);

    return !node.is_empty();
  }

  IptablesProcessingData get_iptables_processing_data(const xmldoc &doc, const ::std::string interface_name) {

    IptablesProcessingData data;
    xmlctx ctx(get_ctx(doc));

    data.interface_ = interface_name;
    data.second_interface_ = "";

    data.policy_ = get_string(ctx, "string(" + create_xpath_to_get(interface_name) + "/@policy)", false);

    auto limit = get_string(ctx, "string(" + create_xpath_to_get(interface_name) + "/@limit)", false);

    data.limit_ = limit.substr(0, limit.find_first_of("/second"));

    data.burst_ = get_string(ctx, "string(" + create_xpath_to_get(interface_name) + "/@burst)", false);

    if (data.burst_ == "") {
      data.burst_ = "-";
    }

    return data;
  }

  ::std::string create_xpath_to_get(::std::string interface_name) {
    return "/f:firewall/f:ipv4/f:echo/f:request[@if='" + interface_name + "']";
  }
};

namespace {
// Name has to be unique over all tests.
::std::string generate_test_name(testing::TestParamInfo<IptablesProcessingData> data) {

  auto limit = (data.param.limit_ == "-") ? "0" : data.param.limit_;
  auto burst = (data.param.burst_ == "-") ? "0" : data.param.burst_;

  return data.param.interface_ + data.param.policy_ + limit + burst + data.param.second_interface_;
}
}
// @formatter:off
INSTANTIATE_TEST_CASE_P(InstantiationName, IptablesProcessingTest, testing::Values(
    IptablesProcessingData { "X1","drop","2","-","br0" },
    IptablesProcessingData { "X2","drop","2","-","br1" },
    IptablesProcessingData { "br2","drop","2","-","br2" },
    IptablesProcessingData { "br3","drop","2","-","br3" }
    ), generate_test_name);
// @formatter:on
//

TEST_P(IptablesProcessingTest, SetIcmpEchoProtectionForInterface) {
  auto data = GetParam();

  ::std::string limit = (data.limit_ != "-") ? data.limit_ + "/second" : "-";

  ::std::vector<::std::string> argv = { data.policy_, data.interface_, limit, data.burst_ };

  auto doc = file_accessor_.read_configuration("iptables", false);
  ASSERT_NO_THROW(iptables::impl::set_echo_if(doc, argv));

  IptablesProcessingData current_data = get_iptables_processing_data(doc, data.second_interface_);
  ASSERT_EQ(data.policy_, current_data.policy_);
  ASSERT_EQ(data.limit_, current_data.limit_);
  ASSERT_EQ(data.burst_, current_data.burst_);
}

TEST_P(IptablesProcessingTest, RemoveIcmpEchoProtectionForInterface) {
  auto data = GetParam();

  ::std::string limit = (data.limit_ != "-") ? data.limit_ + "/second" : "-";

  ::std::vector<::std::string> argv = { data.interface_ };

  auto doc = file_accessor_.read_configuration("iptables", false);
  ASSERT_NO_THROW(iptables::impl::rem_echo_if(doc, argv));

  IptablesProcessingData current_data = get_iptables_processing_data(doc, data.interface_);
  ASSERT_FALSE(interface_exists(doc, data.interface_));

  if (!data.second_interface_.empty()) {
    ASSERT_FALSE(interface_exists(doc, data.second_interface_));
  }
}
