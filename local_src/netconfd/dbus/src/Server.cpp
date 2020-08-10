// SPDX-License-Identifier: GPL-2.0-or-later

#include "IDBusHandlerRegistry.h"
#include "Server.h"

namespace netconf {
namespace dbus {

Server::Server()
{
  owner_id = g_bus_own_name(G_BUS_TYPE_SYSTEM, "de.wago.netconfd", G_BUS_NAME_OWNER_FLAGS_DO_NOT_QUEUE, &Server::OnBusAcquired, &Server::OnNameAcquired, NULL, this, NULL);
  if (owner_id == 0)
    throw ::std::runtime_error("DBus-Server initialization failed");

  manager = g_dbus_object_manager_server_new ("/de/wago/netconfd");
}

Server::~Server()
{
  g_object_unref (manager);
}

void Server::AddInterface(IDBusHandlerRegistry& interface)
{
  g_dbus_object_manager_server_export (manager, interface.GetInterfaceObject());
  g_dbus_object_manager_server_export (manager, interface.GetIPObject());
  g_dbus_object_manager_server_export (manager, interface.GetBackupObject());

}

void Server::OnBusAcquired(GDBusConnection *connection, const gchar *name, gpointer user_data)
{
  Server* self = reinterpret_cast<Server*>(user_data);
  g_dbus_object_manager_server_set_connection(self->manager, connection);
  self->connection = connection;
}

[[gnu::const]]
void Server::OnNameAcquired(GDBusConnection *connection, const gchar *name, gpointer user_data)
{
}


} /* namespace dbus */
} /* namespace netconf */
