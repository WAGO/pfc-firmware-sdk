lib_LTLIBRARIES = \
	liboms.la
	
library_includedir=$(includedir)/
#library_include_HEADERS=wago_oms_API.h
#include_HEADERS = wago_oms_API.h

#
# binary
#	

#shared library
liboms_la_SOURCES = \
	liboms_statemaschine.c \
	liboms.c \
	liboms.h

#liboms_la_LIBADD = \
#	$(some_LIBS)

liboms_la_LDFLAGS = \
	-avoid-version -shared

liboms_la_CPPFLAGS = \
	-I$(top_srcdir)/include \
	-I$(top_builddir)/include

	