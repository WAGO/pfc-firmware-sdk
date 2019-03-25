ledtestdir=/bin
ledtest_PROGRAMS = diagledtest

AM_CPPFLAGS = \
	-I$(top_srcdir)/include \
	-I$(top_builddir)/include \
	-D _GNU_SOURCE -rdynamic -funwind-tables \
	 $(LIBXML_CFLAGS) $(WAGO_DBUS_CFLAGS) 

#
# binary
#
diagledtest_SOURCES = 	main.c ../src/diagnostic_xml.c interactive.c auto.c ledmisc.c

diagledtest_LDFLAGS = -rdynamic -lrt $(LIBXML_LIBS) $(WAGO_DBUS_LIBS) ../src/diag_lib/libdiagnostic.la
