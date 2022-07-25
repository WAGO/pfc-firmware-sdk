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

struct IptablesMasqueradAndForwardData {
  ::std::string interface_;
  ::std::string second_interface_;
};

class IptablesMasqueradingAndForwardingTest : public IptablesTestBase, public testing::WithParamInterface<IptablesMasqueradAndForwardData> {

 public:

  bool interface_exists(const xmldoc &doc, const ::std::string &interface_name) {

    xmlctx ctx(get_ctx(doc));
    auto node = get_node(ctx, "/f:firewall/f:ipv4/f:forward/f:snat/f:masquerade[@if='" + interface_name + "']", false);

    return !node.is_empty();
  }

  bool forwarding_interface_exists(const xmldoc &doc, const ::std::string &interface_name) {

    xmlctx ctx(get_ctx(doc));
    auto node = get_node(ctx, "/f:firewall/f:ipv4/f:forward/f:dnat/f:fw_port[@if='" + interface_name + "']", false);

    return !node.is_empty();
  }

};

namespace {
// Name has to be unique over all tests.
::std::string generate_test_name(testing::TestParamInfo<IptablesMasqueradAndForwardData> data) {
  return data.param.interface_ + data.param.second_interface_;
}
}

// @formatter:off
INSTANTIATE_TEST_CASE_P(InstantiationName, IptablesMasqueradingAndForwardingTest, testing::Values(
    IptablesMasqueradAndForwardData { "X1","br0" },
    IptablesMasqueradAndForwardData { "X2","br1" },
    IptablesMasqueradAndForwardData { "br1","br1" },
    IptablesMasqueradAndForwardData { "br2","br2" },
    IptablesMasqueradAndForwardData { "foo","foo" }
    ), generate_test_name);
// @formatter:on
//

TEST_P(IptablesMasqueradingAndForwardingTest, SetMasqueradingForInterface) {
  auto data = GetParam();
  ::std::vector<::std::string> argv = { data.interface_ };

  auto doc = file_accessor_.read_configuration("iptables", false);
  ASSERT_NO_THROW(iptables::impl::set_masq(doc, argv));

  ASSERT_TRUE(interface_exists(doc, data.second_interface_));
}

TEST_P(IptablesMasqueradingAndForwardingTest, RemMasqueradingForInterface) {
  auto data = GetParam();
  ::std::vector<::std::string> argv = { data.interface_ };

  auto doc = file_accessor_.read_configuration("iptables", false);
  iptables::impl::set_masq(doc, argv);

  doc = file_accessor_.read_configuration("iptables", false);
  ASSERT_NO_THROW(iptables::impl::rem_masq(doc, argv));

  ASSERT_FALSE(interface_exists(doc, data.second_interface_));
}

TEST_P(IptablesMasqueradingAndForwardingTest, SetPortForwardingForInterface) {
  auto data = GetParam();
  ::std::vector<::std::string> argv = { "off", data.interface_, "tcp", "192.168.1.1", "1234", "192.168.2.2", "4321" };

  auto doc = file_accessor_.read_configuration("iptables", false);
  ASSERT_NO_THROW(iptables::impl::add_pfw(doc, argv));

  ASSERT_TRUE(forwarding_interface_exists(doc, data.second_interface_));
}

TEST_P(IptablesMasqueradingAndForwardingTest, UpdatePortForwardingForInterface) {
  auto data = GetParam();
  ::std::vector<::std::string> argv = { "off", data.interface_, "tcp", "192.168.1.1", "1234", "192.168.2.2", "4321" };
  auto doc = file_accessor_.read_configuration("iptables", false);
  iptables::impl::add_pfw(doc, argv);

  argv = { "1", "on", data.interface_, "tcp", "192.168.1.1", "1234", "192.168.2.2", "4321" };
  ASSERT_NO_THROW(iptables::impl::upd_pfw(doc, argv));

  ASSERT_TRUE(forwarding_interface_exists(doc, data.second_interface_));
}

