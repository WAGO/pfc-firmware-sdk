#CFLAGS =  -W -Wall -Os

AM_CPPFLAGS = \
	-I$(top_srcdir)/include \
	-I$(top_builddir)/include \
	$(WAGO_DBUS_CFLAGS) $(LIBXML_CFLAGS)

eactingboxdir=/bin
eactingbox_PROGRAMS=eactingbox
dbusservicedir=/usr/share/dbus-1/system-services
dbusservice_DATA=wago.diagnostic.service
#
# binary
#
eactingbox_SOURCES = \
	  parse_log.c ../diagnostic_xml.c eventmsg.c \
	 getidstate.c decodeid.c getledstate.c logforward.c addcstdiag.c main_eactingbox.c
	 
eactingbox_LDADD = $(WAGO_DBUS_LIBS) $(LIBXML_LIBS) ../diag_lib/libdiagxml.la ../diag_lib/libdiagnostic.la
eactingbox_CPPFLAGS = \
	-I$(top_srcdir)/include \
	-I$(top_builddir)/include \
	$(WAGO_DBUS_CFLAGS) $(LIBXML_CFLAGS)




