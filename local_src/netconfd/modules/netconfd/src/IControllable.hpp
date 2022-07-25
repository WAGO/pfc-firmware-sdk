// SPDX-License-Identifier: GPL-2.0-or-later
//------------------------------------------------------------------------------
///  \file     IControllable.hpp
///
///  \brief    <short description of the file contents>
///
///  \author   <author> : WAGO GmbH & Co. KG
//------------------------------------------------------------------------------
#pragma once

#include "Status.hpp"

namespace netconf {

class IControllable {
 public:
  virtual ~IControllable() = default;

  virtual void Start() = 0;

};

}  // namespace netconf

//---- End of header file ------------------------------------------------------

