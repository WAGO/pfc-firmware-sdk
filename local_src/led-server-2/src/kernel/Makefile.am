noinst_LTLIBRARIES = libledkernel.la

#
# binary
#

LED_KERN=

LED_KERN += led_multi.c
LED_KERN += led_blink_sequential.c
LED_KERN += led_schedule.c

libledkernel_la_SOURCES = $(LED_KERN)
libledkernel_la_LDFLAGS = -lrt -lpthread
libledkernel_la_CPPFLAGS = \
	-I$(top_srcdir)/include \
	-I$(top_builddir)/include \
	-I../header \
	-D _GNU_SOURCE
	
#libledkernel_la_LIBADD =   ../sequence/libsequencehandler.la


