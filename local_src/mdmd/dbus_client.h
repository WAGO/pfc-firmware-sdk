// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// Copyright (c) 2018-2022 WAGO GmbH & Co. KG

#ifndef DBUS_CLIENT_H
#define DBUS_CLIENT_H

#include <glib.h>
#include <gio/gio.h>
#include <string>

class DBusClient
{
  private:
    GDBusProxy *_gDBusProxy;
    guint32     _timeout;
    std::string _lastError;
  protected:

  public:
    DBusClient(const gchar *name,
               const gchar *obj_name,
               const gchar *interface_name,
               guint        timeout);
    ~DBusClient();

    gint call_sync(const char  *methodName,
                   GVariant    *parameters,
                   GVariant   **result);
    const std::string &GetLastError() { return _lastError; }
};

#endif
