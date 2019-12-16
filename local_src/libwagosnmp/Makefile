###############################################################################
#  Copyright (c) WAGO Kontakttechnik GmbH & Co. KG
#
#  PROPRIETARY RIGHTS are involved in the subject matter of this material.
#  All manufacturing, reproduction, use and sales rights pertaining to this
#  subject matter are governed by the license agreement. The recipient of this
#  software implicitly accepts the terms of the license.
###############################################################################
#
#######################################################################################################################
# Makefile for PTXdist package libwagosnmp
# This makefile uses the infrastructure provided by ct-build.


.SUFFIXES:

.PHONY: default
default: all

#######################################################################################################################
# Overridable project configuration

PROJECT_ROOT ?= .

PTXDIST_PACKAGE ?= LIBWAGOSNMP
PTXDIST_PACKAGE_MK_FILE ?= $(PTXDIST_WORKSPACE)/rules/wago-libs-024-libwagosnmp.make

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
libwagosnmp.so

TEST_BUILDTARGETS += \
alltests.elf

BUILDTARGETS += $(MAIN_BUILDTARGETS) $(TEST_BUILDTARGETS)

INSTALL_TARGETS += \
$(DESTDIR)/usr/lib/pkgconfig/wagosnmp.pc \
$(DESTDIR)/usr/lib/libwagosnmp.so.$(libwagosnmp.so_VERSION) \
$(DESTDIR)/usr/include/wagosnmp_API.h

#######################################################################################################################
# Settings shared between build targets

SHARED_CFLAGS += $(call option_std,gnu99)
SHARED_CXXFLAGS += $(call option_std,gnu++11)

#######################################################################################################################
# Settings for build target libwagosnmp.so

libwagosnmp.so_VERSION = $(LIBWAGOSNMP_VERSION)
libwagosnmp.so_STATICALLYLINKED += 
libwagosnmp.so_LIBS += netsnmp rt
libwagosnmp.so_PKG_CONFIGS += 
libwagosnmp.so_PKG_CONFIG_LIBS += 
libwagosnmp.so_DISABLEDWARNINGS += 
libwagosnmp.so_PREREQUISITES += $(call lib_buildtarget_raw,$(libwagosnmp.so_LIBS) $(libwagosnmp.so_PKG_CONFIG_LIBS),$(libwagosnmp.so_STATICALLYLINKED))
libwagosnmp.so_SOURCES += $(call glob_r,$(addprefix $(SRC_DIR)/*.,$(SOURCE_FILE_EXTENSIONS)))
libwagosnmp.so_CPPFLAGS += -DNETSNMP_NO_INLINE
libwagosnmp.so_CPPFLAGS += $(call pkg_config_cppflags,$(libwagosnmp.so_PKG_CONFIGS))
libwagosnmp.so_CCXXFLAGS += $(OPTION_PIC)
libwagosnmp.so_CCXXFLAGS += $(call option_disable_warning,$(libwagosnmp.so_DISABLEDWARNINGS))
libwagosnmp.so_CFLAGS += $(SHARED_CFLAGS)
libwagosnmp.so_CFLAGS += $(libwagosnmp.so_CCXXFLAGS)
libwagosnmp.so_CFLAGS += $(call pkg_config_cflags,$(libwagosnmp.so_PKG_CONFIGS))
libwagosnmp.so_CXXFLAGS += $(SHARED_CXXFLAGS)
libwagosnmp.so_CXXFLAGS += $(libwagosnmp.so_CCXXFLAGS)
libwagosnmp.so_CXXFLAGS += $(call pkg_config_cxxflags,$(libwagosnmp.so_PKG_CONFIGS))
libwagosnmp.so_LDFLAGS += $(call option_lib,$(libwagosnmp.so_LIBS),libwagosnmp.so)
libwagosnmp.so_LDFLAGS += $(call pkg_config_ldflags,$(libwagosnmp.so_PKG_CONFIGS))

#######################################################################################################################
# Settings for build target alltests.elf

alltests.elf_STATICALLYLINKED += gmock_main gmock gtest
alltests.elf_LIBS += wagosnmp gmock_main gmock gtest
alltests.elf_PKG_CONFIGS += 
alltests.elf_PKG_CONFIG_LIBS += 
alltests.elf_DISABLEDWARNINGS += 
alltests.elf_PREREQUISITES += $(call lib_buildtarget,$(alltests.elf_LIBS),alltests.elf)
alltests.elf_SOURCES += $(call fglob_r,$(PROJECT_ROOT)/test-src,$(SOURCE_FILE_EXTENSIONS))
alltests.elf_CPPFLAGS += -iquote'$(SRC_DIR)'
alltests.elf_CPPFLAGS += $(call pkg_config_cppflags,$(alltests.elf_PKG_CONFIGS))
alltests.elf_CCXXFLAGS += $(call option_disable_warning,$(alltests.elf_DISABLEDWARNINGS))
alltests.elf_CFLAGS += $(SHARED_CFLAGS)
alltests.elf_CFLAGS += $(alltests.elf_CCXXFLAGS)
alltests.elf_CFLAGS += $(call pkg_config_cflags,$(alltests.elf_PKG_CONFIGS))
alltests.elf_CXXFLAGS += $(SHARED_CXXFLAGS)
alltests.elf_CXXFLAGS += $(alltests.elf_CCXXFLAGS)
alltests.elf_CXXFLAGS += $(call pkg_config_cxxflags,$(alltests.elf_PKG_CONFIGS))
alltests.elf_LDFLAGS += $(call option_lib,$(alltests.elf_LIBS),alltests.elf)
alltests.elf_LDFLAGS += $(call pkg_config_ldflags,$(alltests.elf_PKG_CONFIGS))


#######################################################################################################################
# Build infrastructure

include $(SCRIPT_DIR)/buildclang_1.mk
-include $(PTXDIST_WORKSPACE)/../wago_intern/debugging/scripts/valgrind-target.mk

#######################################################################################################################
# Custom rules

$(DESTDIR)/usr/lib/pkgconfig/%.pc: $(PROJECT_ROOT)/%.pc | $(DESTDIR)/usr/lib/pkgconfig
	cp $< $@

$(DESTDIR)/usr/include/%.h: $(PROJECT_ROOT)/%.h | $(DESTDIR)/usr/include
	cp $< $@

#######################################################################################################################
# Bootstrapping

$(SCRIPT_DIR)/%.mk:
	$(error build scripts unavailable ($@) - set script dir (SCRIPT_DIR=$(SCRIPT_DIR)))

