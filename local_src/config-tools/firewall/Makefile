###############################################################################
#  Copyright (c) WAGO GmbH & Co. KG
#
#  PROPRIETARY RIGHTS are involved in the subject matter of this material.
#  All manufacturing, reproduction, use and sales rights pertaining to this
#  subject matter are governed by the license agreement. The recipient of this
#  software implicitly accepts the terms of the license.
###############################################################################
# Makefile for PTXdist package firewall
# This makefile uses the infrastructure provided by ct-build.


.SUFFIXES:

.PHONY: default
default: all

#######################################################################################################################
# Overridable project configuration

PROJECT_ROOT ?= .

PTXDIST_PACKAGE ?= FIREWALL_CONFIG
PTXDIST_PACKAGE_MK_FILE ?= $(PTXDIST_WORKSPACE)/rules/firewall-config.make

SRC_DIR = $(PROJECT_ROOT)

#######################################################################################################################
# Optional package/ platform feature configuration

-include $(SCRIPT_DIR)/platform_1.mk

#######################################################################################################################
# Custom toolchain configuration

LINT_RULE_FILES = \
$(LINT_CONFIG_DIR)/lint-rules/pfc.lnt \
$(wildcard $(PROJECT_ROOT)/suppress.lnt)

# FEW NOTES ABOUT THE MAKEFILE
#
# -O3 -g3 -gdwarf-4
# Since O3 optimization is way more aggresive than Os and O2 it may reveal some
# issues which could stay hidden otherwise. Of course this makes debugging quite
# hard, so for regural checks just change it into O0.
# Even with optimized program some debug capabilites may be in order. For end
# compilation this may be changed to -g0, but for development let it be.
#
# -std=c++0x
# Please be careful about using c++11 features. The toolchain which is in use is
# based on gcc 4.5.x which has only few of the c++11 capabilities. TEST if all
# code using it is actually compiling for the target platform.
#

# Unrem the line if error messages should be shown.
# CONFIG      += -DSHOW_ERRORS

# Set to value other than 0 in order to compile for (test) PC platform. This
# disables logging to common file which requires additional library.
DEBUGPC      = 0

ifeq ($(FIREWALL_CONFIG_BUILDCONFIG), Debug)
    OPTIMIZE = -O0 -g3 -gdwarf-4
else
    OPTIMIZE = -Os -g0 -DNDEBUG
endif

ifneq ($(DEBUGPC), 0)
    firewall.elf_LDFLAGS  += -I../
    firewall.elf_LDFLAGS  += -DDEBUGPC
    alltests.elf_LDFLAGS  += -I../
    alltests.elf_LDFLAGS  += -DDEBUGPC
endif

ifeq ($(DEBUGPC), 0)
    firewall.elf_LDFLAGS  += -L../liblog -lctlog
    alltests.elf_LDFLAGS  += -L../liblog -lctlog
endif

#######################################################################################################################
# Build target configuration

MAIN_BUILDTARGETS += \
libfirewall.a \
firewall.elf

TEST_BUILDTARGETS += \
alltests.elf

BUILDTARGETS += $(MAIN_BUILDTARGETS) $(TEST_BUILDTARGETS)

INSTALL_TARGETS += \
$(DESTDIR)/usr/bin/firewall.elf

CONFIGTOOLS_DIR = $(PROJECT_ROOT)/..

#######################################################################################################################
# Settings for build target firewall

libfirewall.a_DISABLEDWARNINGS +=
libfirewall.a_PREREQUISITES += 
libfirewall.a_PKG_CONFIGS += glib-2.0 gio-2.0 libxml-2.0
libfirewall.a_CPPFLAGS += $(call pkg_config_cppflags,$(libfirewall.a_PKG_CONFIGS))
libfirewall.a_CPPFLAGS += -I $(CONFIGTOOLS_DIR)
libfirewall.a_CFLAGS += $(call option_std,gnu99)
libfirewall.a_CFLAGS += $(OPTIMIZE)
libfirewall.a_CFLAGS += $(OPTION_PIC)
libfirewall.a_CFLAGS += $(call option_disable_warning,$(libfirewall.a_DISABLEDWARNINGS))
libfirewall.a_CXXFLAGS  += $(call option_std,c++14)
libfirewall.a_CXXFLAGS  += $(OPTIMIZE)
libfirewall.a_CXXFLAGS  += -Weffc++
libfirewall.a_CXXFLAGS += $(call option_disable_warning,$(libfirewall.a_DISABLEDWARNINGS))
libfirewall.a_CXXFLAGS += $(call option_disable_warning,abi-tag useless-cast suggest-attribute=pure)
libfirewall.a_SOURCES += $(call fglob_r,$(PROJECT_ROOT)/src/lib,$(SOURCE_FILE_EXTENSIONS))

#######################################################################################################################
# Settings for build target firewall

firewall.elf_STATICALLYLINKED += firewall
firewall.elf_LIBS += $(firewall.elf_STATICALLYLINKED)
firewall.elf_PKG_CONFIGS += $(libfirewall.a_PKG_CONFIGS)
firewall.elf_PKG_CONFIG_LIBS +=
firewall.elf_DISABLEDWARNINGS +=
firewall.elf_PREREQUISITES += $(call lib_buildtarget_raw,$(firewall.elf_LIBS) $(firewall.elf_PKG_CONFIG_LIBS),$(firewall.elf_STATICALLYLINKED))
firewall.elf_CPPFLAGS += $(call pkg_config_cppflags,$(firewall.elf_PKG_CONFIGS))
firewall.elf_CPPFLAGS += -I $(CONFIGTOOLS_DIR)
firewall.elf_CPPFLAGS += -I $(PROJECT_ROOT)/src/lib
firewall.elf_CFLAGS += $(call option_std,gnu99)
firewall.elf_CFLAGS += $(OPTIMIZE)
firewall.elf_CFLAGS += $(OPTION_PIC)
firewall.elf_CFLAGS += $(call option_disable_warning,$(firewall.elf_DISABLEDWARNINGS))
firewall.elf_CFLAGS += $(call pkg_config_cflags,$(firewall.elf_PKG_CONFIGS))
firewall.elf_CXXFLAGS  += $(call option_std,c++14)
firewall.elf_CXXFLAGS  += $(OPTIMIZE)
firewall.elf_CXXFLAGS  += -Weffc++
firewall.elf_CXXFLAGS  += $(call pkg_config_cflags,$(firewall.elf_PKG_CONFIGS))
firewall.elf_CXXFLAGS += $(call option_disable_warning,$(firewall.elf_DISABLEDWARNINGS))
firewall.elf_CXXFLAGS += $(call option_disable_warning,abi-tag useless-cast suggest-attribute=pure)
firewall.elf_LDFLAGS += $(call option_lib,$(firewall.elf_LIBS),firewall.elf)
firewall.elf_LDFLAGS += $(call pkg_config_ldflags,$(firewall.elf_PKG_CONFIGS))
firewall.elf_SOURCES += $(call fglob_r,$(PROJECT_ROOT)/src/app,$(SOURCE_FILE_EXTENSIONS))

#######################################################################################################################
# Settings for build target alltests.elf

alltests.elf_STATICALLYLINKED += firewall gmock_main gmock gtest
alltests.elf_LIBS += $(alltests.elf_STATICALLYLINKED) boost_filesystem
alltests.elf_PKG_CONFIGS += $(libfirewall.a_PKG_CONFIGS)
alltests.elf_PKG_CONFIG_LIBS +=
alltests.elf_DISABLEDWARNINGS +=
alltests.elf_PREREQUISITES += $(call lib_buildtarget_raw,$(alltests.elf_LIBS) $(alltests.elf_PKG_CONFIG_LIBS),$(alltests.elf_STATICALLYLINKED))
alltests.elf_CPPFLAGS += $(call pkg_config_cppflags,$(alltests.elf_PKG_CONFIGS))
alltests.elf_CPPFLAGS += -I $(CONFIGTOOLS_DIR)
alltests.elf_CPPFLAGS += -I $(PROJECT_ROOT)/src/lib
alltests.elf_CFLAGS += $(call option_std,gnu99)
alltests.elf_CFLAGS += $(OPTION_PIC)
alltests.elf_CFLAGS += $(call option_disable_warning,$(alltests.elf_DISABLEDWARNINGS))
alltests.elf_CFLAGS += $(call pkg_config_cflags,$(alltests.elf_PKG_CONFIGS))
alltests.elf_CXXFLAGS  += $(call option_std,c++14)
alltests.elf_CXXFLAGS += $(OPTION_PIC)
alltests.elf_CXXFLAGS  += -Weffc++
alltests.elf_CXXFLAGS  += $(call pkg_config_cflags,$(alltests.elf_PKG_CONFIGS))
alltests.elf_CXXFLAGS += $(call option_disable_warning,$(alltests.elf_DISABLEDWARNINGS))
alltests.elf_CXXFLAGS += $(call option_disable_warning,abi-tag useless-cast suggest-attribute=pure)
alltests.elf_LDFLAGS += $(call option_lib,$(alltests.elf_LIBS),alltests.elf)
alltests.elf_LDFLAGS += $(call pkg_config_ldflags,$(alltests.elf_PKG_CONFIGS))
alltests.elf_SOURCES += $(call fglob_r,$(PROJECT_ROOT)/test-src,$(SOURCE_FILE_EXTENSIONS))

#######################################################################################################################
# Build infrastructure

-include $(SCRIPT_DIR)/buildclang_1.mk

#######################################################################################################################
# Custom rules

#$(DESTDIR)/usr/lib/pkgconfig/%.pc: $(PROJECT_ROOT)/%.pc | $(DESTDIR)/usr/lib/pkgconfig
#	cp $< $@

#$(DESTDIR)/usr/include/%.h: $(PROJECT_ROOT)/%.h | $(DESTDIR)/usr/include
#	cp $< $@

#######################################################################################################################
# Bootstrapping

$(SCRIPT_DIR)/%.mk :
	$(error build scripts unavailable ($@) - set script dir (SCRIPT_DIR=$(SCRIPT_DIR)))
