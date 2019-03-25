AM_CFLAGS = -O3 -Wall -Wstrict-prototypes -fno-strict-aliasing -g -rdynamic -D_GNU_SOURCE -I.
lib_LTLIBRARIES = libnvram.la
libnvram_la_SOURCES = nvram.c nvram.h
libnvram_la_CFLAGS = $(AM_CFLAGS)
lib_LIBRARIES = libnvram.a
libnvram_a_SOURCES = $(libnvram_la_SOURCES)

include_HEADERS = nvram.h