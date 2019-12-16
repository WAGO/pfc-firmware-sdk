// SPDX-License-Identifier: GPL-2.0-or-later
//------------------------------------------------------------------------------
///  \file     IInterfaceEvent.h
///
///  \brief    <short description of the file contents>
///
///  \author   <author> : WAGO Kontakttechnik GmbH & Co. KG
//------------------------------------------------------------------------------
#ifndef PFCSPECIFIC_SRC_ALL_LOGICALDEVICE_IINTERFACEEVENT_H_
#define PFCSPECIFIC_SRC_ALL_LOGICALDEVICE_IINTERFACEEVENT_H_

//------------------------------------------------------------------------------
// include files
//------------------------------------------------------------------------------
#include <cstdint>
#include "NetworkInterfaceConstants.hpp"
//------------------------------------------------------------------------------
// defines; structure, enumeration and type definitions
//------------------------------------------------------------------------------

namespace netconfd {

class IInterfaceEvent{

 public:
  virtual ~IInterfaceEvent() = default;

  virtual void LinkChange(::std::uint32_t if_index, eth::InterfaceLinkState new_state) = 0;

};

}  // namespace pfcspecific


#endif /* PFCSPECIFIC_SRC_ALL_LOGICALDEVICE_IINTERFACEEVENT_H_ */
//---- End of source file ------------------------------------------------------

