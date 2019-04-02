# Copyright (c) WAGO Kontakttechnik GmbH & Co. KG
#
# PROPRIETARY RIGHTS are involved in the subject matter of this material. All
# manufacturing, reproduction, use, and sales rights pertaining to this
# subject matter are governed by the license agreement. The recipient of this
# software implicitly accepts the terms of the license.

TOOLCHAIN_PREFIX=/opt/OSELAS.Toolchain-2012.12.1-WAGO.3/arm-cortexa8-linux-gnueabihf/gcc-4.7.3-glibc-2.16.0-binutils-2.22-kernel-3.6-sanitized
CROSS_COMPILE=$(TOOLCHAIN_PREFIX)/bin/arm-cortexa8-linux-gnueabihf-
CC=$(CROSS_COMPILE)gcc
CFLAGS=-I. 
DEPS=
SRC_DIR=src
BIN_DIR=bin
# PFC ip address for 'make install' target. For development purposes only.
TARGET_IP=172.29.233.172
INSTALL_DIR=/usr/sbin/

EEPROM_XSECTION_FILES=$(SRC_DIR)/eeprom-xsection.o

ifdef _HW_PLATFORM_XSECTION

ifeq ($(_HW_PLATFORM_XSECTION), wago-pfcXXX)
EEPROM_XSECTION_FILES=$(SRC_DIR)/eeprom-xsection-pfcXXX.o
endif

endif

all: eeprom-xsection
%.o: %.c $(DEPS) dirs
	@$(CC) -c -o $@ $< $(CFLAGS)

dirs:
	@mkdir -p $(BIN_DIR)

eeprom-xsection: $(EEPROM_XSECTION_FILES) $(SRC_DIR)/sysfs_gpiolib.o
	@echo -n "Building $@ ($^) ... "
	@$(CC) -o $(BIN_DIR)/eeprom-xsection $^ $(CFLAGS)
	@echo "done."

clean: 
	@echo -n "Clean Up ... "
	@rm -f $(BIN_DIR)/* $(SRC_DIR)/*.o
	@echo "done."

install:
	@echo "Install eeprom-xsection to $(TARGET_IP):$(INSTALL_DIR) ..."
	@scp $(BIN_DIR)/eeprom-xsection root@$(TARGET_IP):$(INSTALL_DIR)
	@echo "done"

