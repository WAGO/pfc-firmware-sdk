// SPDX-License-Identifier: GPL-2.0-or-later
//------------------------------------------------------------------------------
///  \file     IEthernetInterfaceFactory.hpp
///
///  \brief    <short description of the file contents>
///
///  \author   <author> : WAGO Kontakttechnik GmbH & Co. KG
//------------------------------------------------------------------------------
#pragma once
#include <memory>
#include "IEthernetInterface.hpp"

namespace netconf {

class IEthernetInterfaceFactory {
 public:
  virtual ~IEthernetInterfaceFactory() = default;

  virtual ::std::unique_ptr<IEthernetInterface> getEthernetInterface(std::string name) = 0;
};

}  // namespace netconf

//---- End of header file ------------------------------------------------------

