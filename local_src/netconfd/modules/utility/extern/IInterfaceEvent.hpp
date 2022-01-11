// SPDX-License-Identifier: LGPL-3.0-or-later
#pragma once

//------------------------------------------------------------------------------
// include files
//------------------------------------------------------------------------------
#include <cstdint>
#include "NetworkInterfaceConstants.hpp"
//------------------------------------------------------------------------------
// defines; structure, enumeration and type definitions
//------------------------------------------------------------------------------

namespace netconf {

class IInterfaceEvent{

 public:
  virtual ~IInterfaceEvent() = default;

  virtual void LinkChange(::std::uint32_t if_index, ::std::uint32_t new_state) = 0;

};

}  // namespace pfcspecific



