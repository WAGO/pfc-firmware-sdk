// SPDX-License-Identifier: LGPL-3.0-or-later

#include "DbusError.hpp"

namespace netconf {

DbusError::DbusError() {
  dbus_error_init(&err);
}

DbusError::~DbusError() {
  if (dbus_error_is_set(&err) != 0) {
    dbus_error_free(&err);
  }
}


bool DbusError::IsSet() const noexcept{
  return dbus_error_is_set(&err) != 0;
}

const char* DbusError::GetName() const noexcept {
  return err.name;
}

const char* DbusError::GetMessage() const noexcept {
  return err.message;
}

}  // namespace netconf
