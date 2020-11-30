// SPDX-License-Identifier: LGPL-3.0-or-later
#pragma once

#include <string>

#include "Error.hpp"
#include "JsonConverter.hpp"

namespace netconf {
namespace api {

inline Error MakeError(const ::std::string& error){
  JsonConverter jc;
  Error e;
  auto convert_error = jc.FromJsonString(error, e);
  if(convert_error.IsNotOk()){
    e = convert_error;
  }
  return e;
}

} // namespace api
} // namespace netconf
