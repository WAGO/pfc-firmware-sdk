ledserverddir=/bin
ledserverd_PROGRAMS = ledserverd

AM_CPPFLAGS = \
	-I$(top_srcdir)/include \
	-I$(top_builddir)/include \
	$(WAGO_DBUS_CFLAGS) $(LIBXML_CFLAGS)  \
	-D _GNU_SOURCE -rdynamic -funwind-tables -mapcs-frame

AM_CXXFLAGS = -std=c++14
#
# binary
#
ledserverd_SOURCES = ../diagnostic_xml.c ../led_info/led_info_json.cpp main.c

ledserverd_LDADD = $(LEDSERVER2_LIBS) $(WAGO_DBUS_LIBS) $(LIBXML_LIBS)  ../diag_lib/libdiagxml.la ../diag_lib/libdiagnostic.la
ledserverd_LDFLAGS = -rdynamic -lrt

