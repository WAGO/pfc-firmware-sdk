AM_CFLAGS = -O3 -Wall -Wstrict-prototypes -fno-strict-aliasing -g -rdynamic -fPIC
lib_LTLIBRARIES = libuio.la
libuio_la_SOURCES = libuio.c libUIO.h
include_HEADERS = libUIO.h
libuio_la_CFLAGS = $(AM_CFLAGS)

lib_LIBRARIES = libuio.a
libuio_a_SOURCES = $(libuio_la_SOURCES)