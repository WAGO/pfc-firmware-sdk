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
  enum class Action{
    UNSPEC,
    NEW,
    DEL,
    GET,
    SET,
    CHANGE,
  };

  const ::std::string ActionToString(Action a){
    switch(a){
      case Action::UNSPEC: return "UNSPEC";
      case Action::NEW: return "NEW";
      case Action::DEL: return "DEL";
      case Action::GET: return "GET";
      case Action::SET: return "SET";
      case Action::CHANGE: return "CHANGE";
      default: return "";
    }
  }

  virtual ~IInterfaceEvent() = default;

  virtual void LinkChange(::std::uint32_t if_index, ::std::uint32_t new_state, Action action) = 0;

};

}  // namespace pfcspecific



