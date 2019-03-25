#######################################################################################################################
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.
#
# Copyright (c) 2018 WAGO Kontakttechnik GmbH & Co. KG
#
#######################################################################################################################
LIBXML_CFLAGS=$$(pkg-config --cflags libxml-2.0)
LIBXML_LDLIBS=$$(pkg-config --libs libxml-2.0)

LIBNL_CFLAGS=$$(pkg-config --cflags libnl-3.0)
LIBNL_LDLIBS=$$(pkg-config --libs libnl-3.0 libnl-route-3.0)

LIBXSLT_CFLAGS=$$(pkg-config --cflags libxslt)
LIBXSLT_LDLIBS=$$(pkg-config --libs libxslt)

GLIB_CFLAGS=$$(pkg-config --cflags glib-2.0)
GLIB_LDLIBS=$$(pkg-config --libs glib-2.0)

DEBUG= -O0 -ggdb2

OBJ= ct_liblog.o

GPROF=

override CFLAGS+= -Wall -Wextra -Werror -Wshadow -fPIC $(LIBXML_CFLAGS) $(LIBNL_CFLAGS) $(LIBXSLT_CFLAGS) $(GLIB_CFLAGS) $(DEBUG) $(GPROF)
override LDLIBS+= $(LIBXML_LDLIBS) $(LIBNL_LDLIBS) $(LIBXSLT_LDLIBS) $(GLIB_LDLIBS)

all: libctlog.so
	
libctlog.so: $(OBJ)
	$(CC) $(LDFLAGS) -Wl,--as-needed -o $@ $^  $(LDLIBS$(LDLIBS-$(@))) -shared

clean:
	rm -rf *.o
