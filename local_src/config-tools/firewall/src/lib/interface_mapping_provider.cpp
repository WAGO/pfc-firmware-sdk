//------------------------------------------------------------------------------
// Copyright (c) WAGO GmbH & Co. KG
//
// PROPRIETARY RIGHTS are involved in the subject matter of this material. All
// manufacturing, reproduction, use, and sales rights pertaining to this
// subject matter are governed by the license agreement. The recipient of this
// software implicitly accepts the terms of the license.
//------------------------------------------------------------------------------

#include "interface_mapping_provider.hpp"
#include "error.hpp"

namespace wago {
namespace firewall {

namespace {

bool is_mixed_pair(const ::std::string interface1, const ::std::string interface2) {
  return ((interface1 == "X1" && interface2 == "br1") || (interface1 == "X2" && interface2 == "br0")
      || (interface1 == "br1" && interface2 == "X1") || (interface1 == "br0" && interface2 == "X2")
      || (interface1 == "X1" && interface2 == "br0") || (interface1 == "X2" && interface2 == "br1")
      || (interface1 == "br0" && interface2 == "X1") || (interface1 == "br1" && interface2 == "X2"));
}

}

::std::string InterfaceMappingProvider::get_interface(const ::std::string &interface) const {

  ::std::string corresponding_interface;

  if (interface == "X1") {
    corresponding_interface = "br0";
  } else if (interface == "X2") {
    corresponding_interface = "br1";
  } else {
    corresponding_interface = interface;
  }

  return corresponding_interface;
}

::std::pair<::std::string, ::std::string> InterfaceMappingProvider::get_interface(
      const ::std::string &interface1,
      const ::std::string &interface2) const {

  if (interface1 == interface2) {
    throw invalid_param_error("interface1 and interface2 are equal: " + interface1);
  }

  // Do not mix old and new interface names.
  if (is_mixed_pair(interface1, interface2)) {
    throw invalid_param_error("Found mixed interface input.");
  }

  return ::std::make_pair(
      get_interface(interface1),
      get_interface(interface2));
}

} // namespace firewall
} // namespace wago
