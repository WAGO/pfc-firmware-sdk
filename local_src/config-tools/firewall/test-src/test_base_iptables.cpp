//------------------------------------------------------------------------------
// Copyright (c) WAGO GmbH & Co. KG
//
// PROPRIETARY RIGHTS are involved in the subject matter of this material. All
// manufacturing, reproduction, use and sales rights pertaining to this
// subject matter are governed by the license agreement. The recipient of this
// software implicitly accepts the terms of the license.
//------------------------------------------------------------------------------

#include "test_base_iptables.hpp"
#include "test_utils.hpp"

using namespace wago::firewall;

namespace {
const ::std::string test_res_dir = "../../../test-res";
const ::std::string params_file = test_res_dir + "/params.xml";
const ::std::string ipcmn_file = test_res_dir + "/ipcmn.xml";
}

IptablesTestBase::IptablesTestBase()
    :
    tmp_dir_ { TestUtils::create_temp_dir("firewall_") },
    file_accessor_ { tmp_dir_ } {
}

void IptablesTestBase::SetUp() {
  TestUtils::create_dir(tmp_dir_ + "/etc/firewall/iptables");
  TestUtils::copy_file(params_file, tmp_dir_ + "/etc/firewall/params.xml");
  TestUtils::copy_file(ipcmn_file, tmp_dir_ + "/etc/firewall/iptables/ipcmn.xml");
}

void IptablesTestBase::TearDown() {
  TestUtils::remove_dir(tmp_dir_);
}
