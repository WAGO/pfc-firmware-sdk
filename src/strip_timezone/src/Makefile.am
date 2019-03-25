bin_PROGRAMS=tzstrip
CFLAGS=-O0 -g3
tzstrip_SOURCES=tzstrip.c
tzstrip_LDADD = \
	$(GLIB_LIBS)

tzstrip_CPPFLAGS = \
	-I$(top_srcdir)/include \
	-I$(top_builddir)/include\
	-W -Wall -g3 -O0 \
	$(GLIB_CFLAGS)
