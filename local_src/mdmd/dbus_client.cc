// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// Copyright (c) 2018 WAGO Kontakttechnik GmbH & Co. KG

#include "dbus_client.h"
#include <gio/gio.h>

gint
DBusClient::call_sync(const char  *methodName,
                      GVariant    *parameters,
                      GVariant   **ppresult)
{
  if (_gDBusProxy != NULL)
  {
    GError *err = NULL;
    GVariant *result = g_dbus_proxy_call_sync(_gDBusProxy,
                                              methodName,
                                              parameters,
                                              G_DBUS_CALL_FLAGS_NONE,
                                              _timeout,
                                              0,
                                              &err);
    if (err == NULL)
    {
      *ppresult = result;
      return 0;
    }
    if (result != NULL)
    {
      g_variant_unref(result);
    }
    _lastError = err->message;
    g_error_free(err);
  }
  else
  {
    _lastError = "DBusProxy initialization failed";
  }
  return -1;
}

DBusClient::DBusClient( const gchar *name,
                        const gchar *obj_name,
                        const gchar *interface_name,
                        guint        timeout )
{
  _gDBusProxy = g_dbus_proxy_new_for_bus_sync( G_BUS_TYPE_SYSTEM,
                                               G_DBUS_PROXY_FLAGS_NONE,
                                               0,
                                               name,
                                               obj_name,
                                               interface_name,
                                               0,
                                               0);
  _timeout = timeout;
}

DBusClient::~DBusClient()
{
  if (_gDBusProxy != NULL)
  {
    g_object_unref(_gDBusProxy);
  }
}
