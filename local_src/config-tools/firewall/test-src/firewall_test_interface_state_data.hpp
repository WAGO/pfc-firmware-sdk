//------------------------------------------------------------------------------
// Copyright (c) WAGO Kontakttechnik GmbH & Co. KG
//
// PROPRIETARY RIGHTS are involved in the subject matter of this material. All
// manufacturing, reproduction, use and sales rights pertaining to this
// subject matter are governed by the license agreement. The recipient of this
// software implicitly accepts the terms of the license.
//------------------------------------------------------------------------------

#pragma once

#include <string>

namespace wago {

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

} // namespace wago
