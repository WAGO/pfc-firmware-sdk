bin_PROGRAMS = user1 user2 user3 user4 multiworker multiworker_client1 multiworker_client2

#wagoDbusTestdir=/usr/bin/

AM_CPPFLAGS = \
	-I$(top_srcdir)/include \
	-I$(top_builddir)/include \
	$(DBUS_GLIB_CFLAGS)  \
	-D _GNU_SOURCE

#
# binary
#
user1_SOURCES = \
	 user1.c

user1_LDADD = \
	$(DBUS_GLIB_LIBS) \
	../src/libwago_dbus.la

user2_SOURCES = \
	 user2.c

user2_LDADD = \
	$(DBUS_GLIB_LIBS) \
	../src/libwago_dbus.la
	
user3_SOURCES = \
	 user3.c

user3_LDADD = \
	$(DBUS_GLIB_LIBS) \
	../src/libwago_dbus.la
	
user4_SOURCES = \
	 user4.c

user4_LDADD = \
	$(DBUS_GLIB_LIBS) \
	../src/libwago_dbus.la
	
multiworker_SOURCES = \
	 multiworker.c

multiworker_LDADD = \
	$(DBUS_GLIB_LIBS) \
	../src/libwago_dbus.la
	
multiworker_client1_SOURCES = \
	 multiworker_client1.c

multiworker_client1_LDADD = \
	$(DBUS_GLIB_LIBS) \
	../src/libwago_dbus.la
	
multiworker_client2_SOURCES = \
	 multiworker_client2.c

multiworker_client2_LDADD = \
	$(DBUS_GLIB_LIBS) \
	../src/libwago_dbus.la
	
