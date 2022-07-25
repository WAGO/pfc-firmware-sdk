#######################################################################################################################
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.
#
# Copyright (c) 2018-2022 WAGO GmbH & Co. KG
#
#######################################################################################################################
LIBXML_CFLAGS= $(shell $(PKG_CONFIG) --cflags libxml-2.0)
LIBXML_LDLIBS= $(shell $(PKG_CONFIG) --libs libxml-2.0)

LIBNL_CFLAGS= $(shell $(PKG_CONFIG) --cflags libnl-3.0)
LIBNL_LDLIBS= $(shell $(PKG_CONFIG) --libs libnl-3.0 libnl-route-3.0)

LIBXSLT_CFLAGS= $(shell $(PKG_CONFIG) --cflags libxslt)
LIBXSLT_LDLIBS= $(shell $(PKG_CONFIG) --libs libxslt)

GLIB_CFLAGS= $(shell $(PKG_CONFIG) --cflags glib-2.0)
GLIB_LDLIBS= $(shell $(PKG_CONFIG) --libs glib-2.0)

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
