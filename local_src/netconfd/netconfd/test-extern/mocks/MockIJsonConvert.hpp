// SPDX-License-Identifier: GPL-2.0-or-later
//------------------------------------------------------------------------------
///  \file     MockIJsonConvert.hpp
///
///  \brief    <short description of the file contents>
///
///  \author   <author> : WAGO Kontakttechnik GmbH & Co. KG
//------------------------------------------------------------------------------
#pragma once

#include <gmock/gmock.h>
#include "IJsonConvert.hpp"

namespace netconfd {




template <class T>
class MockIJsonConvert : public IJsonConvert<T>
{
  MOCK_METHOD1_T(ToJsonString, ::std::string(const T&));
  MOCK_METHOD2_T(FromJsonString, Status(const ::std::string&, T&));
};

}  // namespace netconfd
//---- End of header file ------------------------------------------------------

