/*
 * DBusServer.h
 *
 *  Created on: 04.06.2019
 *      Author: u014487
 */

#ifndef DBUS_SRC_SERVER_H_
#define DBUS_SRC_SERVER_H_

#include <gio/gio.h>
#include <cstdlib>
#include <memory>
#include <boost/thread/scoped_thread.hpp>

#ifdef G_OS_UNIX
#include <gio/gunixfdlist.h>
/* For STDOUT_FILENO */
#include <unistd.h>
#endif

namespace netconfd {

namespace dbus {

class IDBusHandlerRegistry;

class Server {
 public:
  Server();
  ~Server();

  Server(const Server&) = delete;
  Server& operator=(const Server&) = delete;
  Server(Server&&) = delete;
  Server& operator=(Server&&) = delete;


  void AddInterface(IDBusHandlerRegistry& interface);

 private:

  static void OnNameAcquired(GDBusConnection *connection, const gchar *name, gpointer user_data);
  static void OnBusAcquired(GDBusConnection *connection, const gchar *name, gpointer user_data);

  guint owner_id;
  GDBusConnection *connection;
  GDBusObjectManagerServer *manager;
};

}  // namespace dbus
} /* namespace netconfd */

#endif /* DBUS_SRC_SERVER_H_ */
