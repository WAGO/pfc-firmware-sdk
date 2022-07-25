#######################################################################################################################
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.
#
# Copyright (c) 2018-2022 WAGO GmbH & Co. KG
#
#######################################################################################################################
LIBXML_CFLAGS=$$($(PKG_CONFIG) --cflags libxml-2.0)
LIBXML_LDLIBS=$$($(PKG_CONFIG) --libs libxml-2.0)

LIBNL_CFLAGS=$$($(PKG_CONFIG) --cflags libnl-3.0)
LIBNL_LDLIBS=$$($(PKG_CONFIG) --libs libnl-3.0 libnl-route-3.0 libnl-genl-3.0)

LIBXSLT_CFLAGS=$$($(PKG_CONFIG) --cflags libxslt)
LIBXSLT_LDLIBS=$$($(PKG_CONFIG) --libs libxslt)

GLIB_CFLAGS=$$($(PKG_CONFIG) --cflags glib-2.0)
GLIB_LDLIBS=$$($(PKG_CONFIG) --libs glib-2.0)

NETCONF_LDFLAGS:= $$($(PKG_CONFIG) --libs libnetconf)
NETCONF_CFLAGS:= $$($(PKG_CONFIG) --cflags libnetconf)

DEBUG= -O0 -ggdb2

OBJ= ct_xml.o ct_netlink.o ct_xml_xpath.o ct_libnet.o ct_xslt.o ct_ethtool.o

GPROF=

# Add no sign compare option to handle linux kernel 4.9 ethtool.h warning.
override CFLAGS+= -Wall -Wextra -Wno-sign-compare -Werror -Wshadow -fPIC $(LIBXML_CFLAGS) $(LIBNL_CFLAGS) $(LIBXSLT_CFLAGS) $(GLIB_CFLAGS) $(NETCONF_CFLAGS)
override CFLAGS+= -Os -g
override LDLIBS+= $(LIBXML_LDLIBS) $(LIBNL_LDLIBS) $(LIBXSLT_LDLIBS) $(GLIB_LDLIBS) $(NETCONF_LDFLAGS) -lstdc++
override CXXFLAGS += $(CFLAGS) -std=c++17

ifeq ($(WITH_SWCONFIG),yes)
override LDLIBS += -lswconfig
OBJ+= ct_swconfig.o
endif

override LDLIBS += -ltypelabel

.PHONY: tests

all: libctnetwork.so #tests

tests:
	make -C ./unit-tests _SYSROOT=$(_SYSROOT) CPPUTEST_HOME=$(CPPUTEST_HOME) CFLAGS="$(CFLAGS)" LDLIBS="$(LDLIBS)" all_no_tests

libctnetwork.so: $(OBJ)
	$(CC) $(LDFLAGS) -o $@ $^  $(LDLIBS$(LDLIBS-$(@))) -shared

clean:
	rm -rf *.o
