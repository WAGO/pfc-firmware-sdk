// SPDX-License-Identifier: GPL-2.0-or-later
#pragma once

#include <dbus/dbus.h>

namespace netconf {

class DbusError {
 public:
  DbusError();
  ~DbusError();

  DBusError* operator&() {
    return &err;
  }

 private:
  DBusError err = {};
};

}  // namespace netconf
