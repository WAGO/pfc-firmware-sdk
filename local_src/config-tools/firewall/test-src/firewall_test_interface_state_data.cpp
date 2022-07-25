//------------------------------------------------------------------------------
// Copyright (c) WAGO GmbH & Co. KG
//
// PROPRIETARY RIGHTS are involved in the subject matter of this material. All
// manufacturing, reproduction, use and sales rights pertaining to this
// subject matter are governed by the license agreement. The recipient of this
// software implicitly accepts the terms of the license.
//------------------------------------------------------------------------------

#include "firewall_test_interface_state_data.hpp"

namespace wago {
namespace firewall {

FirewallTestInterfaceStateData::FirewallTestInterfaceStateData()
    :
    interface_ { },
    state_ { },
    second_interface_ { } {
}

FirewallTestInterfaceStateData::FirewallTestInterfaceStateData(::std::string interface, ::std::string state,
                                                               std::string second_interface)
    :
    interface_ { interface },
    state_ { state },
    second_interface_ { second_interface } {
}

} // namespace firewall
} // namespace wago
