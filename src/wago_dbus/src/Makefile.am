lib_LTLIBRARIES = \
	libwago_dbus.la

#
# binary
#
libwago_dbus_la_SOURCES = \
	 communication_general.c \
	 communication_message.c \
	 communication_server.c \
	 communication_direct.c \
	 communication_internal.h\
	 communication_API.h

libwago_dbus_la_LIBADD = \
	$(DBUS_GLIB_LIBS)

libwago_dbus_la_LDFLAGS = \
	-avoid-version -shared -lrt

libwago_dbus_la_CPPFLAGS = \
	-I$(top_srcdir)/include \
	-I$(top_builddir)/include\
	-W -Wall -Os \
	$(DBUS_GLIB_CFLAGS)
