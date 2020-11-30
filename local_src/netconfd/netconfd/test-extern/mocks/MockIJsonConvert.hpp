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

namespace netconf {




template <class T>
class MockIJsonConvert : public IJsonConvert<T>
{

  MOCK_CONST_METHOD2_T(ToJsonStringImpl, ::std::string(const T&, JsonFormat));
  MOCK_CONST_METHOD2_T(FromJsonStringImpl, Error(const ::std::string&, T&));

  virtual ::std::string ToJsonString(const T& t, JsonFormat format) const noexcept override
  {
    return ToJsonStringImpl(t, format);
  }

  virtual Error FromJsonString(const ::std::string& s, T& t) const noexcept override
  {
    return FromJsonStringImpl(s, t);
  }



};

}  // namespace netconf

