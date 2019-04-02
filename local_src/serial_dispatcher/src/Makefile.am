bin_PROGRAMS = \
				ttydispatcherd

AM_CPPFLAGS = \
	-I$(top_srcdir)/include \
	-I$(top_builddir)/include \
	-D_GNU_SOURCE=1
#	$(some_CFLAGS)

#
# binary
#
ttydispatcherd_SOURCES = \
					serial_dispatcher.c \
					wago.c \
					login_shell.c

ttydispatcherd_LDADD = -lutil -lrt
#	$(some_LIBS)

	