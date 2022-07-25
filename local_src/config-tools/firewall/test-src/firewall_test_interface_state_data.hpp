//------------------------------------------------------------------------------
// Copyright (c) WAGO GmbH & Co. KG
//
// PROPRIETARY RIGHTS are involved in the subject matter of this material. All
// manufacturing, reproduction, use and sales rights pertaining to this
// subject matter are governed by the license agreement. The recipient of this
// software implicitly accepts the terms of the license.
//------------------------------------------------------------------------------

#ifndef WAGO_FIREWALL_TEST_BASE_IPTABLES_FORWARD_PROCESSING_HPP_
#define WAGO_FIREWALL_TEST_BASE_IPTABLES_FORWARD_PROCESSING_HPP_

#include <string>

namespace wago {
namespace firewall {

class FirewallTestInterfaceStateData {
 public:
  FirewallTestInterfaceStateData();
  FirewallTestInterfaceStateData(::std::string interface, ::std::string state, std::string second_interface);
  virtual ~FirewallTestInterfaceStateData() = default;
  FirewallTestInterfaceStateData(const FirewallTestInterfaceStateData &other) = default;
  FirewallTestInterfaceStateData(FirewallTestInterfaceStateData &&other) = default;
  FirewallTestInterfaceStateData& operator=(const FirewallTestInterfaceStateData &other) = default;
  FirewallTestInterfaceStateData& operator=(FirewallTestInterfaceStateData &&other) = default;

  ::std::string interface_;
  ::std::string state_;
  ::std::string second_interface_;

};

} // namespace firewall
} // namespace wago

#endif // WAGO_FIREWALL_TEST_BASE_IPTABLES_FORWARD_PROCESSING_HPP_
