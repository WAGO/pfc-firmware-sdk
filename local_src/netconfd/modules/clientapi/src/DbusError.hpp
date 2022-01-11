// SPDX-License-Identifier: LGPL-3.0-or-later
#pragma once

#include <dbus/dbus.h>

namespace netconf {

class DbusError {
 public:
  DbusError();
  ~DbusError();

  bool IsSet() const noexcept;
  const char* GetName() const noexcept;
  const char* GetMessage() const noexcept;

  DBusError* operator&() noexcept{
    return &err;
  }

 private:
  DBusError err = {};
};

}  // namespace netconf
