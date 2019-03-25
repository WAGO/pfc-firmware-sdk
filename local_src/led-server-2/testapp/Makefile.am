ledtestdir=/bin
ledtest_PROGRAMS = ledtest

AM_CPPFLAGS = \
	-I$(top_srcdir)/include \
	-I$(top_builddir)/include \
	-D _GNU_SOURCE -rdynamic -funwind-tables

#
# binary
#
ledtest_SOURCES = 	main.c interactive.c auto.c ledmisc.c

#ledtest_LDADD = libledserver2.so
ledtest_LDFLAGS = -rdynamic -lrt -L../src/.libs -lledserver2
