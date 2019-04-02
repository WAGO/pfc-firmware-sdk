if HAL_PCA955X
HAL_PREFIX=hal/pca955x
else 
if HAL_GPIO
HAL_PREFIX=hal/gpio
endif
endif

LEDS_C=$(HAL_PREFIX)/leds.c

SUBDIRS = kernel
SUBDIRS += sequence



#ledserverddir=/bin
#ledserverd_PROGRAMS = ledserverd

#AM_CPPFLAGS = \
#	-I$(top_srcdir)/include \
#	-I$(top_builddir)/include \
#	$(WAGO_DBUS_CFLAGS) $(LIBXML_CFLAGS)  \
#	-D _GNU_SOURCE -rdynamic -funwind-tables

#AM_CPPFLAGS = \
#	-I$(top_srcdir)/include \
#	-I$(top_builddir)/include 

#
# binary
#
#ledserverd_SOURCES = \
#	$(LEDS_C)  led_multi.c led_blink_periodic.c sequence_handler.c sequence_flash.c sequence_can.c sequence_750.c led_blink_sequential.c led_schedule.c \
#	diagnostic_xml.c \
#	main.c

#ledserverd_LDADD = $(WAGO_DBUS_LIBS) $(LIBXML_LIBS)  $(WAGO_DIAG_LIBS) $(WAGO_DIAG_XML_LIBS)
#ledserverd_LDFLAGS = -rdynamic -lrt

lib_LTLIBRARIES = \
	libledserver2.la

libledserver2_la_SOURCES = $(LEDS_C)
libledserver2_la_LDFLAGS = -avoid-version -shared -rdynamic -lrt
libledserver2_la_CPPFLAGS = \
	-I$(top_srcdir)/include \
	-I$(top_builddir)/include\
	-Iheader 
	
libledserver2_la_LIBADD = kernel/libledkernel.la sequence/libsequencehandler.la