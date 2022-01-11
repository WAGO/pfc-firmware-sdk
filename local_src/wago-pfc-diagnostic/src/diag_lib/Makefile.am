#CFLAGS = -g -W -Wall -Os

lib_LTLIBRARIES = \
	libdiagnostic.la libdiagxml.la

#library_includedir=$(includedir)/diagnostic
#library_include_HEADERS=diagnostic_API.h diagnostic_xml_API.h
pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = wago_diagnostic.pc wago_diagnostic_xml.pc

#
# binary
#
libdiagnostic_la_SOURCES = \
	diagnostic.c \
	../led_info/led_info_json.cpp
#	syslog.c 
	
libdiagnostic_la_LIBADD = \
	$(WAGO_DBUS_LIBS)
	
libdiagnostic_la_LDFLAGS = \
	-avoid-version -shared

libdiagnostic_la_CPPFLAGS = \
	-I$(top_srcdir)/include \
	-I$(top_builddir)/include \
	$(WAGO_DBUS_CFLAGS) \
	-D _GNU_SOURCE

libdiagnostic_la_CXXFLAGS = -std=c++14

#include_HEADERS = diagnostic_API.h

#
# binary
#
libdiagxml_la_SOURCES = \
	diagnostic_xml.c 	\
	../diagnostic_xml.c
	
libdiagxml_la_LIBADD = \
	$(LIBXML_LIBS) \
	$(WAGO_DBUS_LIBS)
	
libdiagxml_la_LDFLAGS = \
	-avoid-version -shared           


libdiagxml_la_CPPFLAGS = \
	-I$(top_srcdir)/include \
	-I$(top_builddir)/include \
	$(WAGO_DBUS_CFLAGS) $(LIBXML_CFLAGS) \
	-D _GNU_SOURCE

#include_HEADERS = diagnostic_xml_API.h