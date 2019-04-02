#######################################################################################################################
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.
#
# Copyright (c) 2018 WAGO Kontakttechnik GmbH & Co. KG
#
#######################################################################################################################

all: mdmd mdm_cuse

check:
	$(MAKE) -f tests.mk $(MFLAGS) check

CXX=$(CROSS_COMPILE)g++
WARNINGS     = -Werror -Wall -Wextra -pedantic -Wshadow -Wno-missing-field-initializers

LDFLAGS += `pkg-config --libs glib-2.0 gio-2.0 dbus-1 fuse3 typelabel`

CXXFLAGS += `pkg-config --cflags glib-2.0 gio-2.0 dbus-1 fuse3 typelabel`
CXXFLAGS += -g
CXXFLAGS += --std=c++0x
CXXFLAGS += -I$(SYSROOT)/usr/include
CXXFLAGS += -I$(SYSROOT)/usr/include/fuse
CXXFLAGS += -D_FILE_OFFSET_BITS=64
CXXFLAGS += -D_MDMD_VERSION='"$(MDMD_VERSION)"' 
CXXFLAGS += $(WARNINGS)

#-------------------------------------------------------------------------------
#	Linker flags
#-------------------------------------------------------------------------------

mdmd: serialport.o mdmd.o pty.o events.o mdm_statemachine.o persistent_storage.o timeout.o dbus_server.o mdm_dbusserver.o sysfs_lib.o dbus_client.o mdm_cuse_text_data.o mdm_cuse_worker.o mdmd_log.o
	$(CXX) -o mdmd $(LDFLAGS) mdmd.o serialport.o pty.o events.o timeout.o sysfs_lib.o mdm_statemachine.o persistent_storage.o dbus_server.o mdm_dbusserver.o dbus_client.o mdm_cuse_text_data.o mdm_cuse_worker.o mdmd_log.o

mdm_cuse: mdm_cuse.o
	$(CXX) -o mdm_cuse $(LDFLAGS) mdm_cuse.o

.PHONY: install

clean:
	rm -rf *.o
	rm -rf mdmd
	rm -rf mdm_cuse

install: mdmd mdm_cuse mdmd.conf mdmd_init.conf
	mkdir -p $(DESTDIR)/usr/bin/
	mkdir -p $(DESTDIR)/etc/specific
	mkdir -p $(DESTDIR)/etc/dbus-1/system.d/
	mkdir -p $(DESTDIR)/lib/systemd/system
	mkdir -p $(DESTDIR)/lib/udev/rules.d/
	cp mdmd $(DESTDIR)/usr/bin/
	cp mdm_cuse $(DESTDIR)/usr/bin/
	cp mdmd.conf $(DESTDIR)/etc/dbus-1/system.d/
	cp mdmd.service $(DESTDIR)/lib/systemd/system/
	cp mdmd_init.conf $(DESTDIR)/etc/specific/
	cp 76-tty-modem.rules $(DESTDIR)/lib/udev/rules.d/

