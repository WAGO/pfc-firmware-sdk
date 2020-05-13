// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// Copyright (c) 2018 WAGO Kontakttechnik GmbH & Co. KG



#include "dbus_server.h"
#include "gerror_exception.h"

const GDBusInterfaceVTable DBusServer::interface_vtable =
{
    handle_method_call,
    handle_get_property,
    handle_set_property,
    //padding
    {0}
};

void
DBusServer::handle_method_call (GDBusConnection       *connection,
	const gchar           *sender,
	const gchar           *object_path,
	const gchar           *interface_name,
	const gchar           *method_name,
	GVariant              *parameters,
	GDBusMethodInvocation *invocation,
	gpointer               user_data)
{
    (void)connection; (void)sender; (void)object_path; (void)interface_name; (void)method_name; (void)parameters; //unused parameter
    DBusServer *s = static_cast<DBusServer *>(user_data);
    s->method_call(MethodInvocation(invocation));
}

GVariant *
DBusServer::handle_get_property (GDBusConnection  *connection,
	const gchar      *sender,
	const gchar      *object_path,
	const gchar      *interface_name,
	const gchar      *property_name,
	GError          **error,
	gpointer          user_data)
{
    DBusServer *s = static_cast<DBusServer *>(user_data);
    return s->get_property(connection, sender, object_path, interface_name, property_name, error);
}

gboolean
DBusServer::handle_set_property (GDBusConnection  *connection,
	const gchar      *sender,
	const gchar      *object_path,
	const gchar      *interface_name,
	const gchar      *property_name,
	GVariant         *value,
	GError          **error,
	gpointer          user_data)
{
    DBusServer *s = static_cast<DBusServer *>(user_data);
    return s->set_property(connection, sender, object_path, interface_name, property_name, value, error);
}

void
DBusServer::on_bus_acquired (GDBusConnection *connection,
	const gchar     *name,
	gpointer         user_data)
{
    DBusServer *s = static_cast<DBusServer *>(user_data);
    std::string strName = name?name:"";
    s->bus_acquired(connection, strName);
}


void
DBusServer::on_name_acquired (GDBusConnection *connection,
	const gchar     *name,
	gpointer         user_data)
{
    DBusServer *s = static_cast<DBusServer *>(user_data);
    std::string strName = name?name:"";
    s->name_acquired(connection, strName);
}

void
DBusServer::on_name_lost (GDBusConnection *connection,
	const gchar     *name,
	gpointer         user_data)
{
    DBusServer *s = static_cast<DBusServer *>(user_data);
    std::string strName = name?name:"";
    s->name_lost(connection, strName);
}

DBusServer::DBusServer( const std::string &bus_name, const std::string &object_name, const std::string &introspection_xml )
    : _bus_name(bus_name)
    , _object_name(object_name)
    , _online(false)
{
    _connection = 0;
    _introspection_data = g_dbus_node_info_new_for_xml (introspection_xml.c_str(), nullptr);
    _owner_id = g_bus_own_name(G_BUS_TYPE_SYSTEM,
                               _bus_name.c_str(),
                               G_BUS_NAME_OWNER_FLAGS_NONE,
                               on_bus_acquired,
                               on_name_acquired,
                               on_name_lost,
                               this,
                               0 );
}

DBusServer::~DBusServer()
{
    if (_owner_id != 0)
    {
        g_bus_unown_name(_owner_id);
    }
    if (_introspection_data != 0)
    {
        g_dbus_node_info_unref(_introspection_data);
    }
}


void
DBusServer::bus_acquired(GDBusConnection *connection, const std::string &name)
{
  (void)name; //unused parameter
    _connection = connection;

    if (_introspection_data != 0)
    {
        for (GDBusInterfaceInfo ** i=_introspection_data->interfaces; (i!=0) && (*i!=0); i++)
        {
            _registration_id = g_dbus_connection_register_object (connection,
                                                                  _object_name.c_str(),
                                                                  *i,
                                                                  &interface_vtable,
                                                                  this,
                                                                  nullptr,  /* user_data_free_func */
                                                                  nullptr); /* GError** */
            g_assert (_registration_id > 0);
        }
    }
}

void
DBusServer::name_acquired(GDBusConnection *connection, const std::string &name)
{
  (void)connection; (void)name; //unused parameter
    _online = true;
}

void
DBusServer::name_lost(GDBusConnection *connection, const std::string &name)
{
  (void)name; //unused parameter
    _online = false;
	g_dbus_connection_unregister_object(connection, _registration_id);
}

void
DBusServer::emit_signal( const std::string &destination_bus_name,
	const std::string &object_path,
	const std::string &interface_name,
	const std::string &signal_name,
	GVariant *parameters)
{
    GError *err = 0;

    g_dbus_connection_emit_signal( _connection,
	    destination_bus_name.c_str(),
	    object_path.c_str(),
	    interface_name.c_str(),
	    signal_name.c_str(),
	    parameters,
	    &err);

    if (err)
      g_error_free(err);
}

void
DBusServer::broadcast_signal( const std::string &object_path,
			      const std::string &interface_name,
			      const std::string &signal_name,
			      GVariant *parameters)
{
    GError *err = 0;

    g_dbus_connection_emit_signal( _connection,
	    0,
	    object_path.c_str(),
	    interface_name.c_str(),
	    signal_name.c_str(),
	    parameters,
	    &err);

    if (err)
      g_error_free(err);
}
