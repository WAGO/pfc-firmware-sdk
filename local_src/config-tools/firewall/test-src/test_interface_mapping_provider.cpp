//------------------------------------------------------------------------------
// Copyright (c) WAGO GmbH & Co. KG
//
// PROPRIETARY RIGHTS are involved in the subject matter of this material. All
// manufacturing, reproduction, use and sales rights pertaining to this
// subject matter are governed by the license agreement. The recipient of this
// software implicitly accepts the terms of the license.
//------------------------------------------------------------------------------


#include <gtest/gtest.h>
#include "interface_mapping_provider.hpp"
#include "test_utils.hpp"

using namespace wago::firewall;

TEST(InterfaceMappingProviderTest, GetInterfacesTheFirewallRuleHasToBeAppliedFor) {

  ::std::string interface;
  InterfaceMappingProvider mapping;

  interface = mapping.get_interface("br0");
  ASSERT_EQ(interface, "br0");

  interface = mapping.get_interface("X1");
  ASSERT_EQ(interface, "br0");

  interface = mapping.get_interface("X2");
  ASSERT_EQ(interface, "br1");

  interface = mapping.get_interface("br1");
  ASSERT_EQ(interface, "br1");

  interface = mapping.get_interface("br3");
  ASSERT_EQ(interface, "br3");

}
