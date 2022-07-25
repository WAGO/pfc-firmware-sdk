//------------------------------------------------------------------------------
// Copyright (c) WAGO GmbH & Co. KG
//
// PROPRIETARY RIGHTS are involved in the subject matter of this material. All
// manufacturing, reproduction, use and sales rights pertaining to this
// subject matter are governed by the license agreement. The recipient of this
// software implicitly accepts the terms of the license.
//------------------------------------------------------------------------------

#include "test_base_ebtables.hpp"
#include "test_utils.hpp"

using namespace wago::firewall;

namespace {
 const ::std::string test_res_dir = "../../../test-res";
 const ::std::string params_file = test_res_dir + "/params.xml";
 const ::std::string ebwlist_file = test_res_dir + "/ebwlist.xml";
}

EbtablesTestBase::EbtablesTestBase()
    :
    tmp_dir_ { TestUtils::create_temp_dir("firewall_") },
    file_accessor_ { tmp_dir_ } {
}

void EbtablesTestBase::SetUp() {
  TestUtils::create_dir(tmp_dir_ + "/etc/firewall/ebtables");
  TestUtils::copy_file(params_file, tmp_dir_ + "/etc/firewall/params.xml");
  TestUtils::copy_file(ebwlist_file, tmp_dir_ + "/etc/firewall/ebtables/ebwlist.xml");
}

void EbtablesTestBase::TearDown() {
  TestUtils::remove_dir(tmp_dir_);
}

