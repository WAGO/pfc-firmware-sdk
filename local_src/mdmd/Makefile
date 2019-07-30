#######################################################################################################################
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.
#
# This file is part of project mdmd (PTXdist package mdmd).
#
# Copyright (c) 2017 WAGO Kontakttechnik GmbH & Co. KG
#
# Contributors:
#   JB: WAGO Kontakttechnik GmbH & Co. KG
#   PEn: WAGO Kontakttechnik GmbH & Co. KG
#   MMy: WAGO Kontakttechnik GmbH & Co. KG
#######################################################################################################################
# Makefile for tests in PTXdist package mdmd
# This makefile uses the infrastructure provided by ct-build.


.SUFFIXES:

.PHONY: default
default: all

#######################################################################################################################
# Overridable project configuration

#ENABLE_VERBOSE = T

PROJECT_ROOT ?= .

PTXDIST_PACKAGE ?= MDMD
PTXDIST_PACKAGE_MK_FILE ?= $(PTXDIST_WORKSPACE)/rules/wago-net-001-mdmd.make

SRC_DIR = $(PROJECT_ROOT)

#######################################################################################################################
# Optional package/ platform feature configuration

-include $(SCRIPT_DIR)/platform_1.mk

#######################################################################################################################
# Custom toolchain configuration

LINT_RULE_FILES = \
$(LINT_CONFIG_DIR)/lint-rules/pfc.lnt \
$(wildcard $(PROJECT_ROOT)/suppress.lnt)

#######################################################################################################################
# Build target configuration

MAIN_BUILDTARGETS += \
mdmd.elf \
mdm_cuse.elf 

TEST_BUILDTARGETS += \
alltests.elf

BUILDTARGETS += $(MAIN_BUILDTARGETS) $(TEST_BUILDTARGETS)

INSTALL_TARGETS += \
$(DESTDIR)/usr/bin/mdmd \
$(DESTDIR)/usr/bin/mdm_cuse \
$(DESTDIR)/etc/dbus-1/system.d/mdmd.conf \
$(DESTDIR)/etc/specific/mdmd_init.conf \
$(DESTDIR)/lib/systemd/system/mdmd.service \
$(DESTDIR)/lib/udev/rules.d/76-tty-modem.rules

CLANG_TIDY_RULESET = $(CLANG_TIDY_BASE_CHECKS)

#######################################################################################################################
# Settings shared between build targets

SHARED_CFLAGS   += $(call option_std,gnu99)
SHARED_CXXFLAGS += $(call option_std,gnu++11)

INCLUDES_OF_EXTERNAL_PACKAGES += -isystem $(SYSROOT)/usr/include/glib-2.0
INCLUDES_OF_EXTERNAL_PACKAGES += -isystem $(SYSROOT)/usr/include/fuse3

#######################################################################################################################
# Settings for build target mdmd

mdmd.elf_VERSION = $(MDMD_VERSION)
mdmd.elf_LIBS +=
mdmd.elf_STATICALLYLINKED +=
mdmd.elf_PKG_CONFIGS += glib-2.0 gio-2.0 dbus-1 fuse3 typelabel wago_diagnostic
mdmd.elf_PKG_CONFIG_LIBS += glib-2.0 gio-2.0 dbus-1 fuse3 typelabel diagnostic
mdmd.elf_DISABLEDWARNINGS += 
mdmd.elf_PREREQUISITES += $(call lib_buildtarget_raw, $(mdmd.elf_LIBS) $(mdmd.elf_PKG_CONFIG_LIBS) $(mdmd.elf_STATICALLYLINKED))
mdmd.elf_SOURCES += \
 $(SRC_DIR)/mdmd.cc \
 $(SRC_DIR)/serialport.cc \
 $(SRC_DIR)/events.cc \
 $(SRC_DIR)/timeout.cc \
 $(SRC_DIR)/sysfs_lib.cc \
 $(SRC_DIR)/mdm_statemachine.cc \
 $(SRC_DIR)/persistent_storage.cc \
 $(SRC_DIR)/dbus_server.cc \
 $(SRC_DIR)/mdm_dbusserver.cc \
 $(SRC_DIR)/dbus_client.cc \
 $(SRC_DIR)/mdm_cuse_text_data.cc \
 $(SRC_DIR)/mdm_cuse_worker.cc \
 $(SRC_DIR)/mdmd_log.cc \
 $(SRC_DIR)/mdm_diagnostic.cpp
mdmd.elf_CPPFLAGS += -D_FILE_OFFSET_BITS=64
mdmd.elf_CPPFLAGS += -D_MDMD_VERSION='"$(MDMD_VERSION)"'
mdmd.elf_CPPFLAGS += $(INCLUDES_OF_EXTERNAL_PACKAGES)
mdmd.elf_CPPFLAGS += $(call pkg_config_cppflags, $(mdmd.elf_PKG_CONFIGS))
mdmd.elf_CCXXFLAGS += $(call option_disable_warning, $(mdmd.elf_DISABLEDWARNINGS))
mdmd.elf_CFLAGS += $(SHARED_CFLAGS)
mdmd.elf_CFLAGS += $(mdmd.elf_CCXXFLAGS)
mdmd.elf_CFLAGS += $(call pkg_config_cflags, $(mdmd.elf_PKG_CONFIGS))
mdmd.elf_CXXFLAGS += $(SHARED_CXXFLAGS)
mdmd.elf_CXXFLAGS += $(mdmd.elf_CCXXFLAGS)
mdmd.elf_LDFLAGS += $(call option_lib, $(mdmd.elf_LIBS, mdmd.elf))
mdmd.elf_LDFLAGS += $(call pkg_config_ldflags, $(mdmd.elf_PKG_CONFIGS))

#######################################################################################################################
# Settings for build target mdm_cuse

mdm_cuse.elf_VERSION = $(MDMD_VERSION)
mdm_cuse.elf_LIBS +=
mdm_cuse.elf_STATICALLYLINKED +=
mdm_cuse.elf_PKG_CONFIGS += glib-2.0 gio-2.0 dbus-1 fuse3 typelabel
mdm_cuse.elf_PKG_CONFIG_LIBS += glib-2.0 gio-2.0 dbus-1 fuse3 typelabel
mdm_cuse.elf_DISABLEDWARNINGS +=
mdm_cuse.elf_PREREQUISITES += $(call lib_buildtarget_raw,$(mdm_cuse.elf_LIBS) $(mdm_cuse.elf_PKG_CONFIG_LIBS) $(mdmd.elf_STATICALLYLINKED))
mdm_cuse.elf_SOURCES += $(SRC_DIR)/mdm_cuse.cc
mdm_cuse.elf_CPPFLAGS += -D_FILE_OFFSET_BITS=64
mdm_cuse.elf_CPPFLAGS += $(call pkg_config_cppflags,$(mdm_cuse.elf_PKG_CONFIGS))
mdm_cuse.elf_CPPFLAGS += $(INCLUDES_OF_EXTERNAL_PACKAGES)
mdm_cuse.elf_CCXXFLAGS += $(call option_disable_warning,$(mdm_cuse.elf_DISABLEDWARNINGS))
mdm_cuse.elf_CFLAGS += $(SHARED_CFLAGS)
mdm_cuse.elf_CFLAGS += $(mdm_cuse.elf_CCXXFLAGS)
mdm_cuse.elf_CFLAGS += $(call pkg_config_cflags, $(mdm_cuse.elf_PKG_CONFIGS))
mdm_cuse.elf_CXXFLAGS += $(SHARED_CXXFLAGS)
mdm_cuse.elf_CXXFLAGS += $(mdm_cuse.elf_CCXXFLAGS)
mdm_cuse.elf_CXXFLAGS += $(call pkg_config_cxxflags,$(mdm_cuse.elf_PKG_CONFIGS))
mdm_cuse.elf_LDFLAGS += $(call option_lib,$(mdm_cuse.elf_LIBS) ,mdmd)
mdm_cuse.elf_LDFLAGS += $(call pkg_config_ldflags, $(mdm_cuse.elf_PKG_CONFIGS))

#######################################################################################################################
# Settings for build target alltests.elf

alltests.elf_STATICALLYLINKED += gmock_main gmock
alltests.elf_LIBS += gmock gmock_main
alltests.elf_PKG_CONFIGS += oslinux glib-2.0 gio-2.0 dbus-1 fuse3 typelabel wago_diagnostic
alltests.elf_PKG_CONFIG_LIBS += oslinux glib-2.0 gio-2.0 dbus-1 fuse3 typelabel diagnostic
alltests.elf_DISABLEDWARNINGS += abi-tag useless-cast
alltests.elf_PREREQUISITES += $(call lib_buildtarget,$(alltests.elf_LIBS),alltests.elf)
alltests.elf_CPPFLAGS += -I$(PROJECT_ROOT)/test-inc
alltests.elf_CPPFLAGS += -D_FILE_OFFSET_BITS=64
alltests.elf_CPPFLAGS += -include $(PROJECT_ROOT)/test-src/preinclude.h
alltests.elf_CPPFLAGS += $(call pkg_config_cppflags,$(alltests.elf_PKG_CONFIGS))
alltests.elf_CPPFLAGS += $(INCLUDES_OF_EXTERNAL_PACKAGES)
alltests.elf_CXXFLAGS += $(call option_disable_warning,$(alltests.elf_DISABLEDWARNINGS))
alltests.elf_CXXFLAGS += $(call pkg_config_cxxflags,$(alltests.elf_PKG_CONFIGS))
alltests.elf_CXXFLAGS += $(call option_std,gnu++11)
alltests.elf_LDFLAGS += $(call option_lib,$(alltests.elf_LIBS),alltests.elf)
alltests.elf_LDFLAGS += $(call pkg_config_ldflags,$(alltests.elf_PKG_CONFIGS))
alltests.elf_SOURCES += $(call fglob_r,$(PROJECT_ROOT)/test-src,$(SOURCE_FILE_EXTENSIONS))
alltests.elf_SOURCES += $(PROJECT_ROOT)/persistent_storage.cc
alltests.elf_SOURCES += $(PROJECT_ROOT)/mdm_cuse_text_data.cc
alltests.elf_SOURCES += $(PROJECT_ROOT)/mdmd_log.cc
alltests.elf_SOURCES += $(PROJECT_ROOT)/GKeyFileStorage.cpp
alltests.elf_SOURCES += $(PROJECT_ROOT)/MdmParameterStorage.cpp
alltests.elf_SOURCES += $(PROJECT_ROOT)/mdm_diagnostic.cpp
alltests.elf_CLANG_TIDY_CHECKS += $(GTEST_CLANG_TIDY_CHECKS)

#######################################################################################################################
# Build infrastructure

include $(SCRIPT_DIR)/buildclang_1.mk

#######################################################################################################################
# Custom rules

$(DESTDIR)/usr/bin/%: $(BIN_DIR)/%.elf
	@install -D $< $@

$(DESTDIR)/etc/dbus-1/system.d/%: $(PROJECT_ROOT)/%
	@install -D $< $@

$(DESTDIR)/etc/specific/%: $(PROJECT_ROOT)/%
	@install -D $< $@

$(DESTDIR)/lib/systemd/system/%: $(PROJECT_ROOT)/%
	@install -D $< $@

$(DESTDIR)/lib/udev/rules.d/%: $(PROJECT_ROOT)/%
	@install -D $< $@

#######################################################################################################################
# Bootstrapping

$(SCRIPT_DIR)/%.mk:
	$(error build scripts unavailable ($@) - set script dir (SCRIPT_DIR=$(SCRIPT_DIR)))
