// SPDX-License-Identifier: GPL-2.0-or-later
//------------------------------------------------------------------------------
///  \file     IInterfaceEventHandler.h
///
///  \brief    <short description of the file contents>
///
///  \author   <author> : WAGO Kontakttechnik GmbH & Co. KG
//------------------------------------------------------------------------------
#ifndef PFCSPECIFIC_SRC_ALL_LOGICALDEVICE_IINTERFACEMONITOR_H_
#define PFCSPECIFIC_SRC_ALL_LOGICALDEVICE_IINTERFACEMONITOR_H_

//------------------------------------------------------------------------------
// include files
//------------------------------------------------------------------------------
#include <cstdint>
#include "IInterfaceEvent.hpp"

//------------------------------------------------------------------------------
// defines; structure, enumeration and type definitions
//------------------------------------------------------------------------------

namespace netconfd {

class IInterfaceMonitor {
 public:
  virtual ~IInterfaceMonitor() = default;

  virtual void RegisterEventHandler(IInterfaceEvent& event_handler) = 0;
  virtual void UnregisterEventHandler(IInterfaceEvent& event_handler) = 0;

  virtual ::std::uint32_t GetIffFlags(::std::uint32_t if_index) = 0;
  virtual ::std::int32_t GetAddressFamily(::std::uint32_t if_index) = 0;

};


}  // namespace pfcspecific


#endif /* PFCSPECIFIC_SRC_ALL_LOGICALDEVICE_IINTERFACEMONITOR_H_ */
//---- End of source file ------------------------------------------------------

