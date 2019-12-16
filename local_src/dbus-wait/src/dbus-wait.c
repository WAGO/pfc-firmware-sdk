/*
Unit2OpenRC - dbus-service-wait

Simple executable that waits until service with name passed as argument
is available and exits with exit code 0.

If anything fails, exits with code 1.
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>   // alarm function
#include <dbus/dbus.h>
#include <stdint.h>


typedef struct {
  /** Holds name of service to wait for */
  char *excepted_name;
  /** Holds pending ListNames method call */
  DBusPendingCall *pending;
  /** Holds executable name (argv[0]) */
  char *arg0;
} ProgramData;


/**
 * Filters recieved messages and calls request_service_list to (re)check list
 * of available services every time when NameOwnerChanged event is detected.
 */
static DBusHandlerResult check_nameowner_changed (DBusConnection *bus, DBusMessage *message, void *user_data);

/**
 * Sends ListNames method call to request list of all available services.
 */
static void request_service_list(DBusConnection *bus, ProgramData *data);

/**
 * Parses list of services recieved as response to method call
 * sent by request_service_list.
 */
static void parse_service_list(ProgramData *data);

/** Called by SIGALRM if service is not found in given time */
static void timeout_reached(int sig);

/** Prints error message. Returns 1 */
static int help(FILE *fd, ProgramData *data);


int main (int argc, char **argv) {
  ProgramData data = { NULL, NULL, argv[0] };
  uint32_t timeout = 5;

  // Parse arguments
  if ((argc < 2) || (argc > 4))
    // Invalid argument count
    return help(stderr, &data);
  data.excepted_name = argv[1];
  if (strcmp(argv[1], "-t") == 0) {
    // Timeout specified
    if (argc != 4)
      // Invalid argument count
      return help(stderr, &data);
    timeout = atoi(argv[2]);
    if (timeout < 1)
      // Invalid timeout
      return help(stderr, &data);
    data.excepted_name = argv[3];
  }
  if (strcmp(argv[1], "--help") == 0)
    // Display help
    return help(stdout, &data) && 0;
  if (strcmp(argv[1], "-h") == 0)
    // Display help
    return help(stdout, &data) && 0;

  DBusError error = DBUS_ERROR_INIT;
  DBusConnection *bus;

  // Setup timeout
  signal (SIGALRM, timeout_reached);
  alarm (timeout);

  // Connect to DBus
  bus = dbus_bus_get (DBUS_BUS_SYSTEM, &error);
  if (!bus) {
    fprintf (stderr, "%s: Failed to acquire system bus: %s\n", argv[0], error.message);
    dbus_error_free (&error);
    return 1;
  }
  
  // Setup filtering
  char* filter = "interface='org.freedesktop.DBus',member='NameOwnerChanged'";
  dbus_bus_add_match (bus, filter, &error);
  if (dbus_error_is_set (&error)) {
    fprintf (stderr, "%s: Failed to setup filter: %s\n", argv[0], error.message);
    dbus_error_free (&error);
    return 1;
  }

  // Enable calling to check_nameowner_changed
  dbus_connection_add_filter (bus, check_nameowner_changed, &data, NULL);
  request_service_list(bus, &data);

  // Loop until finished
  while (dbus_connection_read_write_dispatch (bus, -1)) {
    if ((data.pending != NULL) && dbus_pending_call_get_completed(data.pending)) {
      parse_service_list(&data);
      dbus_pending_call_unref(data.pending);
      data.pending = NULL;
    }
  }

  dbus_connection_unref (bus);
  return 0;
}


static void timeout_reached(int s) {
  fprintf (stderr, "Timeout reached\n");
  exit(1);
}


static void request_service_list(DBusConnection *bus, ProgramData* data) {
  DBusMessage* message;

  message = dbus_message_new_method_call("org.freedesktop.DBus",
    "/org/freedesktop/DBus", "org.freedesktop.DBus", "ListNames");
  if (message == NULL) {
    fprintf (stderr, "%s: Failed to allocate dbus message\n", data->arg0);
    exit(1);
  }

  if (!dbus_connection_send_with_reply (bus, message, &(data->pending), -1)) {
    fprintf (stderr, "%s: Failed to send dbus message\n", data->arg0);
    exit(1);
  }
  dbus_connection_flush(bus);

  if (NULL == data->pending) {
    fprintf (stderr, "%s: Failed to allocate response for ListNames call\n", data->arg0);
    exit(1);
  }

  dbus_message_unref(message);
}


static void parse_service_list(ProgramData *data) {
  DBusMessageIter iter;
  DBusMessageIter value;
  DBusMessage *message = dbus_pending_call_steal_reply(data->pending);

  dbus_message_iter_init(message, &iter);

  if (dbus_message_iter_get_arg_type(&iter) == DBUS_TYPE_ARRAY) {
    dbus_message_iter_recurse(&iter, &value);
    while (dbus_message_iter_get_arg_type(&value) == DBUS_TYPE_STRING) {
      const char *name;
      dbus_message_iter_get_basic(&value, &name);
      if (strcmp(data->excepted_name, name) == 0) {
        exit(0);
      }
      dbus_message_iter_next(&value);
    }
  }

  dbus_message_unref(message);

}


static DBusHandlerResult check_nameowner_changed (DBusConnection *bus, DBusMessage *message, void* user_data) {
  ProgramData *data = (ProgramData*)user_data;
  DBusMessageIter iter;
  int i = 0;
  char* str = NULL;
  
  // Watch only for NameOwnerChanged messages
  if (dbus_message_has_member(message, "NameOwnerChanged")) {
    if (data->pending == NULL) {
      request_service_list(bus, data);
    }
  }
  return DBUS_HANDLER_RESULT_HANDLED;
}


static int help(FILE *fd, ProgramData *data) {
  fprintf (fd, "Usage: %s [-t timeout] BusName\n", data->arg0);
  fprintf (fd, "   or: %s [-h|--help]\n", data->arg0);
  fprintf (fd, "\n");
  fprintf (fd, "Waits until specified the D-Bus bus name is acquired.\n");
  fprintf (fd, "Options:\n");
  fprintf (fd, "  -t            Specifies timeout in seconds. Default 5\n");
  fprintf (fd, "  -h, --help    Display this help output\n");
  fprintf (fd, "\n");
  fprintf (fd, "Exits codes:\n");
  fprintf (fd, "  0 - name is acquired\n");
  fprintf (fd, "  1 - timeout was reached or other failure.\n");

  return 1;
}
