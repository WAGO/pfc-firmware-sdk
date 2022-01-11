// SPDX-License-Identifier: LGPL-3.0-or-later
#pragma once

#include <string>

#include "Status.hpp"
#include "JsonConverter.hpp"

namespace netconf {
namespace api {

inline Status MakeStatus(const ::std::string& status){
  JsonConverter jc;
  Status e;
  auto convert_status = jc.FromJsonString(status, e);
  if(convert_status.IsNotOk()){
    e = convert_status;
  }
  return e;
}

} // namespace api
} // namespace netconf
