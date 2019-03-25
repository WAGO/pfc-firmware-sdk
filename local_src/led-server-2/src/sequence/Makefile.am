BLINK_LIBS = 
BLINK_LIBS += libsequenceblink.la
BLINK_LIBS += libsequenceflash.la
BLINK_LIBS += libsequencecan.la
BLINK_LIBS += libsequence750.la
BLINK_LIBS += libsequencerocket.la

noinst_LTLIBRARIES = $(BLINK_LIBS)
noinst_LTLIBRARIES += libsequencehandler.la

SEQUENCE_CPPFLAGS =  \
	-I$(top_srcdir)/include \
	-I$(top_builddir)/include \
	-I../header \
	-D _GNU_SOURCE
	
SEQUENCE_LDFLAGS=

#
# sequence blink
#
libsequenceblink_la_SOURCES = sequence_blink.c
libsequenceblink_la_LDFLAGS = $(SEQUENCE_LDFLAGS)
libsequenceblink_la_CPPFLAGS =  $(SEQUENCE_CPPFLAGS)

#
# sequence flash
#
libsequenceflash_la_SOURCES = sequence_flash.c
libsequenceflash_la_LDFLAGS = $(SEQUENCE_LDFLAGS)
libsequenceflash_la_CPPFLAGS =  $(SEQUENCE_CPPFLAGS)

#
# sequence CAN
#
libsequencecan_la_SOURCES = sequence_can.c
libsequencecan_la_LDFLAGS = $(SEQUENCE_LDFLAGS)
libsequencecan_la_CPPFLAGS =  $(SEQUENCE_CPPFLAGS)

#
# sequence 750 error code
#
libsequence750_la_SOURCES = sequence_750.c
libsequence750_la_LDFLAGS = $(SEQUENCE_LDFLAGS)
libsequence750_la_CPPFLAGS =  $(SEQUENCE_CPPFLAGS)

#
# sequence rocket error codes
#
libsequencerocket_la_SOURCES = sequence_rocket.c
libsequencerocket_la_LDFLAGS = $(SEQUENCE_LDFLAGS)
libsequencerocket_la_CPPFLAGS =  $(SEQUENCE_CPPFLAGS)


#
# sequence Handler
#
libsequencehandler_la_SOURCES = sequence_handler.c
libsequencehandler_la_LDFLAGS = $(SEQUENCE_LDFLAGS)
libsequencehandler_la_CPPFLAGS =  $(SEQUENCE_CPPFLAGS)
libsequencehandler_la_LIBADD =  $(BLINK_LIBS)



