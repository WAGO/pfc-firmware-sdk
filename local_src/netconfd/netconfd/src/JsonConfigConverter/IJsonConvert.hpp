// SPDX-License-Identifier: GPL-2.0-or-later
//------------------------------------------------------------------------------
///  \file     IJsonConvert.hpp
///
///  \brief    <short description of the file contents>
///
///  \author   <author> : WAGO Kontakttechnik GmbH & Co. KG
//------------------------------------------------------------------------------
#pragma once

#include <string>
#include "Status.hpp"

namespace netconfd {

template<class DataType>
class IJsonConvert {
 public:
  virtual ~IJsonConvert() = default;

  virtual ::std::string ToJsonString(const DataType& obj) = 0;
  virtual Status FromJsonString(const ::std::string& str, DataType& out_obj) = 0;

};

}  // namespace netconfd

//---- End of header file ------------------------------------------------------

