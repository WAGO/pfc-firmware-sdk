// SPDX-License-Identifier: GPL-2.0-or-later
//------------------------------------------------------------------------------
///  \file     MockIMacController.hpp
///
///  \brief    <short description of the file contents>
///
///  \author   <author> : WAGO Kontakttechnik GmbH & Co. KG
//------------------------------------------------------------------------------
#pragma once

#include "IMacController.hpp"
#include <gmock/gmock.h>

namespace netconf {

class MockIMacController : public IMacController {
 public:
  MOCK_METHOD2(SetMac,
      void(MacAddress const& mac, ::std::string const& interface));
};

}  // namespace netconf

//---- End of header file ------------------------------------------------------

