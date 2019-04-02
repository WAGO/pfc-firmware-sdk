bin_PROGRAMS = \
	swconfig

lib_LTLIBRARIES = \
	libswconfig.la

AM_CPPFLAGS = \
	-I$(top_srcdir)/include \
	-I$(top_builddir)/include
#	$(some_CFLAGS)

EXTRA_DIST = \
	autogen.sh

MAINTAINERCLEANFILES = \
	configure \
	aclocal.m4 \
	Makefile.in
#
# shared library
#

libswconfig_la_SOURCES = \
	swlib.c

#
# binary
#
swconfig_SOURCES = \
	cli.c \
	uci.c

swconfig_LDADD = \
	-lnl-3 -lnl-genl-3 -luci libswconfig.la

swconfig_LDFLAGS = -shared

include_HEADERS = swlib.h
