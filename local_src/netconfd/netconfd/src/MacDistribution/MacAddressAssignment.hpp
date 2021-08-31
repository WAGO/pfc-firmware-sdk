// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include "IDeviceInterfaceProvider.hpp"
#include "IDeviceProperties.hpp"

namespace netconf {

//Description mac assignment mode
//Single      e.g.  2Port device 1MAC
//Multiple    e.g.                        4Port device 5MACs
//Full        e.g.  2Port device 3MACs  , 4Port device 6MACs

/*----------------------------------------------------------------------------------+
 | example typelable MAC_ID_INC=7, Device interfaces (X1,X2,X11,X12,br0..br4,usb)    |
 |                                                                                   |
 |  Typelable value's                                                                |
 |  MAC=XX:XX:XX:XX:XX:01                                                            |
 |  MAC_ID_INC=7                                                                     |
 |                                                                                   |
 |      br     br   X1    X2   X11   X12   USB                                       |
 |     :01   :02   :03   :04   :05   :06   :07                                       |
 |                                                                                   |
 +----------------------------------------------------------------------------------*/

class MacAddressAssignment {

 public:

  enum class Mode {
    Undefined,
    Single,
    Multiple,
    Full
  };

  MacAddressAssignment(IDeviceProperties &properties_provider);

  Mode GetMode();

  ::std::string GetBaseMac() const;

 private:

  IDeviceProperties &properties_provider_;

  Mode mode_;

  uint32_t OtherMacs(const Interfaces &interfaces) const;
  bool IsMacAddressAssignmentSingle(uint32_t mac_count) const;
  bool IsMacAddressAssignmentMultiple(uint32_t mac_count, uint32_t port_count) const;
  bool IsMacAddressAssignmentFull(uint32_t mac_count, uint32_t port_count) const;

};

}
