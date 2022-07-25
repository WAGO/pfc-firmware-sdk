// SPDX-License-Identifier: GPL-2.0-or-later
//------------------------------------------------------------------------------
///  \file     MockIPersistence.hpp
///
///  \brief    <short description of the file contents>
///
///  \author   <author> : WAGO GmbH & Co. KG
//------------------------------------------------------------------------------
#pragma once

#include <gmock/gmock.h>
#include "IPersistence.hpp"
#include "Types.hpp"

namespace netconf {


class MockIPersistencePortConfigs : public IPersistence<InterfaceConfigs> {
 public:
  MOCK_METHOD1(Write, Status(const InterfaceConfigs& d) );
  MOCK_METHOD1(Read, Status(InterfaceConfigs& d) );

};

}  // namespace netconf

//---- End of header file ------------------------------------------------------

