// SPDX-License-Identifier: LGPL-3.0-or-later
#pragma once

#include <string>
#include <utility>

#include "Status.hpp"
#include "DbusError.hpp"
#include "JsonConverter.hpp"

namespace netconf {

struct DbusResult {
  DbusResult() = default;
  explicit DbusResult(::std::string error): error_{MakeError(error)}{}
  explicit DbusResult(const DbusError& dbus_error) {
    if(dbus_error.IsSet()){
      error_.Set(StatusCode::DBUS, dbus_error.GetName(), dbus_error.GetMessage());
    }
  }

  DbusResult(::std::string value, ::std::string error): value_json_{::std::move(value)}, error_{MakeError(error)} {

  }

  static Status MakeError(const ::std::string& error_json){
      JsonConverter jc;
      Status e;
      auto convert_error = jc.FromJsonString(error_json, e);
      if(convert_error.IsNotOk()){
        e = convert_error;
      }
      return e;
  }

  ::std::string value_json_;
  Status error_;
};
} // namespace
