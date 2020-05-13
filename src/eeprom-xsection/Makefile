# Copyright (c) WAGO Kontakttechnik GmbH & Co. KG
#
# PROPRIETARY RIGHTS are involved in the subject matter of this material. All
# manufacturing, reproduction, use, and sales rights pertaining to this
# subject matter are governed by the license agreement. The recipient of this
# software implicitly accepts the terms of the license.

CC=$(CROSS)gcc
SRC_DIR=src
CFLAGS += -O2 -Wall -Werror
INSTALL_DIR=$(DESTDIR)/usr/sbin/

APP  := eeprom-xsection
OBJS := eeprom-xsection.o sysfs_gpiolib.o

all: $(APP)

%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c -o $@ $<

$(APP): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

install: $(APP)
	install -d $(INSTALL_DIR)
	install -m 755 $^ $(INSTALL_DIR)

clean:
	rm -f $(APP) $(OBJS) .depend
