// SPDX-License-Identifier: GPL-2.0-or-later
//------------------------------------------------------------------------------
///  \file     InterfaceMonitor.h
///
///  \brief    Encapsulation of Netlink socket handling
///
///  \author   S.Schwalowsky : WAGO Kontakttechnik GmbH & Co. KG
//------------------------------------------------------------------------------
#ifndef PFCSPECIFIC_SRC_ALL_LOGICALDEVICE_INTERFACEMONITOR_H_
#define PFCSPECIFIC_SRC_ALL_LOGICALDEVICE_INTERFACEMONITOR_H_

//------------------------------------------------------------------------------
// include files
//------------------------------------------------------------------------------

#include "IInterfaceEvent.hpp"
#include "IInterfaceMonitor.hpp"
#include <memory>

//------------------------------------------------------------------------------
// defines; structure, enumeration and type definitions
//------------------------------------------------------------------------------

namespace netconfd
{

  class InterfaceMonitor :public IInterfaceMonitor
  {
   public:

    InterfaceMonitor();
    virtual ~InterfaceMonitor();
    InterfaceMonitor(const InterfaceMonitor&) = delete;
    InterfaceMonitor(InterfaceMonitor&&) = delete;

    void RegisterEventHandler(IInterfaceEvent& event_handler) override;
    void UnregisterEventHandler(IInterfaceEvent& event_handler) override;
    ::std::uint32_t GetIffFlags(::std::uint32_t if_index) override;
    ::std::int32_t GetAddressFamily(::std::uint32_t if_index) override;

   private:
    class Impl;
    ::std::unique_ptr<Impl> pimpl_;

  };


} //end namespace

#endif /* PFCSPECIFIC_SRC_ALL_LOGICALDEVICE_INTERFACEMONITOR_H_ */
//---- End of source file ------------------------------------------------------
