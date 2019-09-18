#######################################################################################################################
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.
#
# This file is part of project / PTXdist package modular-config-tools.
#
# Copyright (c) 2017 WAGO Kontakttechnik GmbH & Co. KG
#
# Contributors:
#   JB:  WAGO Kontakttechnik GmbH & Co. KG
#   PEn: WAGO Kontakttechnik GmbH & Co. KG
#######################################################################################################################
# This makefile uses the infrastructure provided by ct-build.


.SUFFIXES:

.PHONY: default doc
default: all doc


#######################################################################################################################
# Overridable project configuration

DISABLE_LTO = T
#ENABLE_VERBOSE = T

PROJECT_ROOT ?= .

PTXDIST_PACKAGE ?= MODULAR_CONFIG_TOOLS
PTXDIST_PACKAGE_MK_FILE ?= $(PTXDIST_WORKSPACE)/rules/wago-config-000-modular-config-tools.make

SRC_DIR = $(PROJECT_ROOT)/src


#######################################################################################################################
# Optional package/ platform feature configuration

-include $(SCRIPT_DIR)/platform_1.mk


#######################################################################################################################
# Custom toolchain configuration

LINT_RULE_FILES = \
$(LINT_CONFIG_DIR)/lint-rules/pfc.lnt


#######################################################################################################################
# Build target configuration

$(DISABLE_NOT_PTXDIST)MAIN_BUILDTARGETS += libconfigtoolutils.a
$(DISABLE_NOT_PTXDIST)MAIN_BUILDTARGETS += libconfigtoolfail.a
$(DISABLE_NOT_PTXDIST)MAIN_BUILDTARGETS += helloworld.elf
$(DISABLE_NOT_PTXDIST)MAIN_BUILDTARGETS += config_linux_user.elf
$(DISABLE_NOT_PTXDIST)MAIN_BUILDTARGETS += reserved_area.elf

$(DISABLE_NOT_PTXDIST)TEST_BUILDTARGETS += libconfigtoolutils_tests.elf
#$(DISABLE_NOT_PTXDIST)TEST_BUILDTARGETS += libconfigtoolcommonmain_tests.elf
$(DISABLE_NOT_PTXDIST)TEST_BUILDTARGETS += config_linux_user_tests.elf
$(DISABLE_NOT_PTXDIST)TEST_BUILDTARGETS += reserved_area_tests.elf

BUILDTARGETS += $(MAIN_BUILDTARGETS) $(TEST_BUILDTARGETS)

$(DISABLE_NOT_PTXDIST)INSTALL_TARGETS += libconfigtoolutils.a

TESTS_DISABLEDWARNINGS += suggest-override
TESTS_DISABLEDWARNINGS += useless-cast
TESTS_DISABLEDWARNINGS += abi-tag


#######################################################################################################################
# Settings for build target libconfigtoolutils.a

libconfigtoolutils.a_DYNAMICALLYREQUIRED += glib-2.0
libconfigtoolutils.a_DISABLEDWARNINGS += 
libconfigtoolutils.a_PREREQUISITES += 
libconfigtoolutils.a_CPPFLAGS += -D'CTUTIL_API=WC_API_EXPORT'
#libconfigtoolutils.a_CPPFLAGS += -D'DEBUGPRINT'
libconfigtoolutils.a_CPPFLAGS += -I$(PROJECT_ROOT)/inc
libconfigtoolutils.a_CPPFLAGS += -I$(PROJECT_ROOT)/src/libconfigtoolutils
libconfigtoolutils.a_CPPFLAGS += -I$(SYSROOT)/usr/include/wc
libconfigtoolutils.a_CPPFLAGS += -I$(SYSROOT)/usr/include/glib-2.0/
libconfigtoolutils.a_CPPFLAGS += -I$(SYSROOT)/usr/lib/glib-2.0/include
#libconfigtoolutils.a_CPPFLAGS += -I$(SYSROOT)/usr/include/glib-2.0/gobject
libconfigtoolutils.a_CCXXFLAGS += $(OPTION_PIC)
libconfigtoolutils.a_CCXXFLAGS += $(call option_disable_warning,$(libconfigtoolutils.a_DISABLEDWARNINGS))
libconfigtoolutils.a_CFLAGS += $(libconfigtoolutils.a_CCXXFLAGS)
libconfigtoolutils.a_CFLAGS += $(call option_std,gnu99)
libconfigtoolutils.a_CXXFLAGS += $(libconfigtoolutils.a_CCXXFLAGS)
libconfigtoolutils.a_CXXFLAGS += $(call option_std,gnu++11)
libconfigtoolutils.a_SOURCES += $(call fglob_r,$(PROJECT_ROOT)/src/libconfigtoolutils,$(SOURCE_FILE_EXTENSIONS))


#######################################################################################################################
# Settings for build target libconfigtoolfail.a

libconfigtoolfail.a_DISABLEDWARNINGS += 
libconfigtoolfail.a_PREREQUISITES += 
libconfigtoolfail.a_CPPFLAGS += -I$(PROJECT_ROOT)/inc
libconfigtoolfail.a_CPPFLAGS += -I$(PROJECT_ROOT)/src/ctparts
libconfigtoolfail.a_CPPFLAGS += -I$(SYSROOT)/usr/include/wc
libconfigtoolfail.a_CCXXFLAGS += $(OPTION_PIC)
libconfigtoolfail.a_CCXXFLAGS += $(call option_disable_warning,$(libconfigtoolfail.a_DISABLEDWARNINGS))
libconfigtoolfail.a_CFLAGS += $(libconfigtoolfail.a_CCXXFLAGS)
libconfigtoolfail.a_CFLAGS += $(call option_std,gnu99)
libconfigtoolfail.a_CXXFLAGS += $(libconfigtoolfail.a_CCXXFLAGS)
libconfigtoolfail.a_CXXFLAGS += $(call option_std,gnu++11)
libconfigtoolfail.a_SOURCES += $(PROJECT_ROOT)/src/common/fail.c


#######################################################################################################################
# Settings for build target libconfigtoolutils_tests.elf

libconfigtoolutils_tests.elf_STATICALLYLINKED += configtoolutils gmock_main gmock
libconfigtoolutils_tests.elf_LIBS += configtoolutils gmock gmock_main
libconfigtoolutils_tests.elf_LIBS += $(libconfigtoolutils.a_DYNAMICALLYREQUIRED)
libconfigtoolutils_tests.elf_PKG_CONFIGS += 
libconfigtoolutils_tests.elf_DISABLEDWARNINGS += 
libconfigtoolutils_tests.elf_PREREQUISITES += $(call lib_buildtarget,$(libconfigtoolutils_tests.elf_LIBS),libconfigtoolutils_tests.elf)
libconfigtoolutils_tests.elf_CPPFLAGS += -I$(PROJECT_ROOT)/inc
libconfigtoolutils_tests.elf_CPPFLAGS += -I$(PROJECT_ROOT)/test-inc
libconfigtoolutils_tests.elf_CPPFLAGS += $(call pkg_config_cppflags,$(libconfigtoolutils_tests.elf_PKG_CONFIGS))
libconfigtoolutils_tests.elf_CCXXFLAGS += $(OPTION_PIC)
libconfigtoolutils_tests.elf_CCXXFLAGS += $(call option_disable_warning,$(libconfigtoolutils_tests.elf_DISABLEDWARNINGS))
libconfigtoolutils_tests.elf_CCXXFLAGS += $(call pkg_config_cflags,$(libconfigtoolutils_tests.elf_PKG_CONFIGS))
libconfigtoolutils_tests.elf_CFLAGS += $(libconfigtoolutils_tests.elf_CCXXFLAGS)
libconfigtoolutils_tests.elf_CFLAGS += $(call option_std,gnu99)
libconfigtoolutils_tests.elf_CXXFLAGS += $(libconfigtoolutils_tests.elf_CCXXFLAGS)
libconfigtoolutils_tests.elf_CXXFLAGS += $(call option_std,gnu++11)
libconfigtoolutils_tests.elf_CXXFLAGS += $(call option_disable_warning,$(TESTS_DISABLEDWARNINGS))
libconfigtoolutils_tests.elf_LDFLAGS += $(call option_lib,$(libconfigtoolutils_tests.elf_LIBS),libconfigtoolutils_tests.elf)
libconfigtoolutils_tests.elf_LDFLAGS += $(call pkg_config_ldflags,$(libconfigtoolutils_tests.elf_PKG_CONFIGS))
libconfigtoolutils_tests.elf_SOURCES += $(call fglob_r,$(PROJECT_ROOT)/test-src/common,$(SOURCE_FILE_EXTENSIONS))
libconfigtoolutils_tests.elf_SOURCES += $(call fglob_r,$(PROJECT_ROOT)/test-src/libconfigtoolutils,$(SOURCE_FILE_EXTENSIONS))


#######################################################################################################################
# Settings for build target libconfigtoolcommonmain_tests.elf

libconfigtoolcommonmain_tests.elf_STATICALLYLINKED += configtoolutils gmock_main gmock
libconfigtoolcommonmain_tests.elf_LIBS += configtoolutils gmock_main gmock
libconfigtoolcommonmain_tests.elf_LIBS += $(libconfigtoolutils.a_DYNAMICALLYREQUIRED)
libconfigtoolcommonmain_tests.elf_PKG_CONFIGS += 
libconfigtoolcommonmain_tests.elf_DISABLEDWARNINGS += 
libconfigtoolcommonmain_tests.elf_PREREQUISITES += $(call lib_buildtarget,$(libconfigtoolcommonmain_tests.elf_LIBS),libconfigtoolcommonmain_tests.elf)
libconfigtoolcommonmain_tests.elf_CPPFLAGS += -I$(PROJECT_ROOT)/inc
libconfigtoolcommonmain_tests.elf_CPPFLAGS += -I$(PROJECT_ROOT)/test-inc
libconfigtoolcommonmain_tests.elf_CPPFLAGS += $(call pkg_config_cppflags,$(libconfigtoolcommonmain_tests.elf_PKG_CONFIGS))
libconfigtoolcommonmain_tests.elf_CCXXFLAGS += $(OPTION_PIC)
libconfigtoolcommonmain_tests.elf_CCXXFLAGS += $(call option_disable_warning,$(libconfigtoolcommonmain_tests.elf_DISABLEDWARNINGS))
libconfigtoolcommonmain_tests.elf_CCXXFLAGS += $(call pkg_config_cflags,$(libconfigtoolcommonmain_tests.elf_PKG_CONFIGS))
libconfigtoolcommonmain_tests.elf_CFLAGS += $(libconfigtoolcommonmain_tests.elf_CCXXFLAGS)
libconfigtoolcommonmain_tests.elf_CFLAGS += $(call option_std,gnu99)
libconfigtoolcommonmain_tests.elf_CXXFLAGS += $(libconfigtoolcommonmain_tests.elf_CCXXFLAGS)
libconfigtoolcommonmain_tests.elf_CXXFLAGS += $(call option_std,gnu++11)
libconfigtoolcommonmain_tests.elf_CXXFLAGS += $(call option_disable_warning,$(TESTS_DISABLEDWARNINGS))
libconfigtoolcommonmain_tests.elf_LDFLAGS += $(call option_lib,$(libconfigtoolcommonmain_tests.elf_LIBS),libconfigtoolcommonmain_tests.elf)
libconfigtoolcommonmain_tests.elf_LDFLAGS += $(call pkg_config_ldflags,$(libconfigtoolcommonmain_tests.elf_PKG_CONFIGS))
libconfigtoolcommonmain_tests.elf_SOURCES += $(call fglob_r,$(PROJECT_ROOT)/test-src/common,$(SOURCE_FILE_EXTENSIONS))
libconfigtoolcommonmain_tests.elf_SOURCES += $(PROJECT_ROOT)/test-src/libconfigtoolutils/test_CommonMain.cc


#######################################################################################################################
# Settings for build target helloworld.elf (example config tool)

helloworld.elf_VERSION = 1.0.0
helloworld.elf_STATICALLYLINKED += configtoolutils configtoolfail
helloworld.elf_LIBS += configtoolutils configtoolfail
helloworld.elf_LIBS += $(libconfigtoolutils.a_DYNAMICALLYREQUIRED)
helloworld.elf_PKG_CONFIGS += 
helloworld.elf_PKG_CONFIG_LIBS += 
helloworld.elf_DISABLEDWARNINGS += 
helloworld.elf_PREREQUISITES += $(call lib_buildtarget,$(helloworld.elf_LIBS),helloworld.elf)
helloworld.elf_CPPFLAGS += -I$(PROJECT_ROOT)/inc
helloworld.elf_CPPFLAGS += -I$(PROJECT_ROOT)/src/exampleHelloWorld
helloworld.elf_CPPFLAGS += $(call pkg_config_cppflags,$(helloworld.elf_PKG_CONFIGS))
helloworld.elf_CCXXFLAGS += $(OPTION_PIC)
helloworld.elf_CCXXFLAGS += $(call option_disable_warning,$(helloworld.elf_DISABLEDWARNINGS))
helloworld.elf_CCXXFLAGS += $(call pkg_config_cflags,$(helloworld.elf_PKG_CONFIGS))
helloworld.elf_CFLAGS += $(helloworld.elf_CCXXFLAGS)
helloworld.elf_CFLAGS += $(call option_std,gnu99)
helloworld.elf_CXXFLAGS += $(helloworld.elf_CCXXFLAGS)
helloworld.elf_CXXFLAGS += $(call option_std,gnu++11)
helloworld.elf_LDFLAGS += $(call option_lib,$(helloworld.elf_LIBS),helloworld.elf)
helloworld.elf_LDFLAGS += $(call pkg_config_ldflags,$(helloworld.elf_PKG_CONFIGS))
helloworld.elf_SOURCES += $(call fglob_r,$(PROJECT_ROOT)/src/exampleHelloWorld,$(SOURCE_FILE_EXTENSIONS))
helloworld.elf_SOURCES += $(PROJECT_ROOT)/src/common/main.c


#######################################################################################################################
# Settings for build target config_linux_user.elf

config_linux_user.elf_VERSION = 1.0.0
config_linux_user.elf_STATICALLYLINKED += configtoolutils configtoolfail
config_linux_user.elf_LIBS += configtoolutils configtoolfail crypt
config_linux_user.elf_LIBS += $(libconfigtoolutils.a_DYNAMICALLYREQUIRED)
config_linux_user.elf_PKG_CONFIGS += 
config_linux_user.elf_PKG_CONFIG_LIBS += 
config_linux_user.elf_DISABLEDWARNINGS += 
config_linux_user.elf_PREREQUISITES += $(call lib_buildtarget,$(config_linux_user.elf_LIBS),config_linux_user.elf)
config_linux_user.elf_CPPFLAGS += -I$(PROJECT_ROOT)/inc
config_linux_user.elf_CPPFLAGS += -I$(PROJECT_ROOT)/src/config_linux_user
config_linux_user.elf_CPPFLAGS += -I$(SYSROOT)/usr/include/glib-2.0/
config_linux_user.elf_CPPFLAGS += -I$(SYSROOT)/usr/lib/glib-2.0/include
config_linux_user.elf_CPPFLAGS += $(call pkg_config_cppflags,$(config_linux_user.elf_PKG_CONFIGS))
config_linux_user.elf_CCXXFLAGS += $(OPTION_PIC)
config_linux_user.elf_CCXXFLAGS += $(call option_disable_warning,$(config_linux_user.elf_DISABLEDWARNINGS))
config_linux_user.elf_CCXXFLAGS += $(call pkg_config_cflags,$(config_linux_user.elf_PKG_CONFIGS))
config_linux_user.elf_CFLAGS += $(config_linux_user.elf_CCXXFLAGS)
config_linux_user.elf_CFLAGS += $(call option_std,gnu99)
config_linux_user.elf_CXXFLAGS += $(config_linux_user.elf_CCXXFLAGS)
config_linux_user.elf_CXXFLAGS += $(call option_std,gnu++11)
config_linux_user.elf_LDFLAGS += $(call option_lib,$(config_linux_user.elf_LIBS),config_linux_user.elf)
config_linux_user.elf_LDFLAGS += $(call pkg_config_ldflags,$(config_linux_user.elf_PKG_CONFIGS))
config_linux_user.elf_SOURCES += $(call fglob_r,$(PROJECT_ROOT)/src/config_linux_user,$(SOURCE_FILE_EXTENSIONS))
config_linux_user.elf_SOURCES += $(PROJECT_ROOT)/src/common/main.c


#######################################################################################################################
# Settings for build target config_linux_user_tests.elf

config_linux_user_tests.elf_STATICALLYLINKED += configtoolutils gmock_main gmock
config_linux_user_tests.elf_LIBS += configtoolutils crypt gmock_main gmock
config_linux_user_tests.elf_LIBS += $(libconfigtoolutils.a_DYNAMICALLYREQUIRED)
config_linux_user_tests.elf_PKG_CONFIGS += 
config_linux_user_tests.elf_DISABLEDWARNINGS += 
config_linux_user_tests.elf_PREREQUISITES += $(call lib_buildtarget,$(config_linux_user_tests.elf_LIBS),config_linux_user_tests.elf)
config_linux_user_tests.elf_CPPFLAGS += -I$(PROJECT_ROOT)/inc
config_linux_user_tests.elf_CPPFLAGS += -I$(PROJECT_ROOT)/test-inc
config_linux_user_tests.elf_CPPFLAGS += -I$(PROJECT_ROOT)/src/config_linux_user
config_linux_user_tests.elf_CPPFLAGS += -I$(SYSROOT)/usr/include/glib-2.0/
config_linux_user_tests.elf_CPPFLAGS += -I$(SYSROOT)/usr/lib/glib-2.0/include
config_linux_user_tests.elf_CPPFLAGS += $(call pkg_config_cppflags, $(config_linux_user_tests.elf_PKG_CONFIGS))
config_linux_user_tests.elf_CCXXFLAGS += $(OPTION_PIC)
config_linux_user_tests.elf_CCXXFLAGS += $(call option_disable_warning,$(config_linux_user_tests.elf_DISABLEDWARNINGS))
config_linux_user_tests.elf_CCXXFLAGS += $(call pkg_config_cflags,$(config_linux_user_tests.elf_PKG_CONFIGS))
config_linux_user_tests.elf_CFLAGS += $(config_linux_user_tests.elf_CCXXFLAGS)
config_linux_user_tests.elf_CFLAGS += $(call option_std,gnu99)
config_linux_user_tests.elf_CXXFLAGS += $(config_linux_user_tests.elf_CCXXFLAGS)
config_linux_user_tests.elf_CXXFLAGS += $(call option_std,gnu++11)
config_linux_user_tests.elf_CXXFLAGS += $(call option_disable_warning,$(TESTS_DISABLEDWARNINGS))
config_linux_user_tests.elf_LDFLAGS += $(call option_lib,$(config_linux_user_tests.elf_LIBS),config_linux_user_tests.elf)
config_linux_user_tests.elf_LDFLAGS += $(call pkg_config_ldflags,$(config_linux_user_tests.elf_PKG_CONFIGS))
config_linux_user_tests.elf_SOURCES += $(call fglob_r,$(PROJECT_ROOT)/test-src/common,$(SOURCE_FILE_EXTENSIONS))
config_linux_user_tests.elf_SOURCES += $(call fglob_r,$(PROJECT_ROOT)/test-src/config_linux_user,$(SOURCE_FILE_EXTENSIONS))
config_linux_user_tests.elf_SOURCES += $(call fglob_r,$(PROJECT_ROOT)/src/config_linux_user,$(SOURCE_FILE_EXTENSIONS))
# TODO: Replace this filter by a better solution (not overwriting the original gtest filter settings)
config_linux_user_tests.elf_VALGRIND_HOST_FLAGS += '--gtest_filter=*:-*_NoQemuValgrind*.*:*.*_NoQemuValgrind*'


#######################################################################################################################
# Settings for build target reserved_area.elf

reserved_area.elf_VERSION = 1.0.0
reserved_area.elf_STATICALLYLINKED += configtoolutils configtoolfail
reserved_area.elf_LIBS += configtoolutils configtoolfail
reserved_area.elf_LIBS += $(libconfigtoolutils.a_DYNAMICALLYREQUIRED)
reserved_area.elf_PKG_CONFIGS += 
reserved_area.elf_PKG_CONFIG_LIBS += 
reserved_area.elf_DISABLEDWARNINGS += 
reserved_area.elf_PREREQUISITES += $(call lib_buildtarget,$(reserved_area.elf_LIBS),reserved_area.elf)
reserved_area.elf_CPPFLAGS += -I$(PROJECT_ROOT)/inc
reserved_area.elf_CPPFLAGS += -I$(PROJECT_ROOT)/src/reserved_area
reserved_area.elf_CPPFLAGS += $(call pkg_config_cppflags,$(reserved_area.elf_PKG_CONFIGS))
reserved_area.elf_CCXXFLAGS += $(OPTION_PIC)
reserved_area.elf_CCXXFLAGS += $(call option_disable_warning,$(reserved_area.elf_DISABLEDWARNINGS))
reserved_area.elf_CCXXFLAGS += $(call pkg_config_cflags,$(reserved_area.elf_PKG_CONFIGS))
reserved_area.elf_CFLAGS += $(reserved_area.elf_CCXXFLAGS)
reserved_area.elf_CFLAGS += $(call option_std,gnu99)
reserved_area.elf_CFLAGS += -D__STDC_FORMAT_MACROS
reserved_area.elf_CXXFLAGS += $(reserved_area.elf_CCXXFLAGS)
reserved_area.elf_CXXFLAGS += $(call option_std,gnu++11)
reserved_area.elf_LDFLAGS += $(call option_lib,$(reserved_area.elf_LIBS),reserved_area.elf)
reserved_area.elf_LDFLAGS += $(call pkg_config_ldflags,$(reserved_area.elf_PKG_CONFIGS))
reserved_area.elf_SOURCES += $(call fglob_r,$(PROJECT_ROOT)/src/reserved_area,$(SOURCE_FILE_EXTENSIONS))
reserved_area.elf_SOURCES += $(PROJECT_ROOT)/src/common/main.c


#######################################################################################################################
# Settings for build target reserved_area_tests.elf

reserved_area_tests.elf_STATICALLYLINKED += configtoolutils gmock_main gmock
reserved_area_tests.elf_LIBS += configtoolutils gmock_main gmock
reserved_area_tests.elf_LIBS += $(libconfigtoolutils.a_DYNAMICALLYREQUIRED)
reserved_area_tests.elf_PKG_CONFIGS += 
reserved_area_tests.elf_DISABLEDWARNINGS += 
reserved_area_tests.elf_PREREQUISITES += $(call lib_buildtarget,$(reserved_area_tests.elf_LIBS),reserved_area_tests.elf)
reserved_area_tests.elf_CPPFLAGS += -I$(PROJECT_ROOT)/inc
reserved_area_tests.elf_CPPFLAGS += -I$(PROJECT_ROOT)/test-inc
reserved_area_tests.elf_CPPFLAGS += -I$(PROJECT_ROOT)/src/reserved_area
reserved_area_tests.elf_CPPFLAGS += $(call pkg_config_cppflags, $(reserved_area_tests.elf_PKG_CONFIGS))
reserved_area_tests.elf_CCXXFLAGS += $(OPTION_PIC)
reserved_area_tests.elf_CCXXFLAGS += $(call option_disable_warning,$(reserved_area_tests.elf_DISABLEDWARNINGS))
reserved_area_tests.elf_CCXXFLAGS += $(call pkg_config_cflags,$(reserved_area_tests.elf_PKG_CONFIGS))
reserved_area_tests.elf_CFLAGS += $(reserved_area_tests.elf_CCXXFLAGS)
reserved_area_tests.elf_CFLAGS += $(call option_std,gnu99)
reserved_area_tests.elf_CFLAGS += -D__STDC_FORMAT_MACROS
reserved_area_tests.elf_CXXFLAGS += $(reserved_area_tests.elf_CCXXFLAGS)
reserved_area_tests.elf_CXXFLAGS += $(call option_std,gnu++11)
reserved_area_tests.elf_CXXFLAGS += $(call option_disable_warning,$(TESTS_DISABLEDWARNINGS))
reserved_area_tests.elf_LDFLAGS += $(call option_lib,$(reserved_area_tests.elf_LIBS),reserved_area_tests.elf)
reserved_area_tests.elf_LDFLAGS += $(call pkg_config_ldflags,$(reserved_area_tests.elf_PKG_CONFIGS))
reserved_area_tests.elf_SOURCES += $(call fglob_r,$(PROJECT_ROOT)/test-src/common,$(SOURCE_FILE_EXTENSIONS))
reserved_area_tests.elf_SOURCES += $(call fglob_r,$(PROJECT_ROOT)/test-src/reserved_area,$(SOURCE_FILE_EXTENSIONS))
reserved_area_tests.elf_SOURCES += $(call fglob_r,$(PROJECT_ROOT)/src/reserved_area,$(SOURCE_FILE_EXTENSIONS))


#######################################################################################################################
# Build infrastructure

-include $(SCRIPT_DIR)/buildclang_1.mk

#######################################################################################################################
# Custom rules

doc:
	rm -rf out/doc/
	# explicitly use line buffering for doxygen to avoid stdout being interrupted by stderr mid-line
	stdbuf --output=L doxygen doc/doxygen.config


#######################################################################################################################
# Bootstrapping

$(SCRIPT_DIR)/%.mk:
	$(error build scripts unavailable - set script dir (SCRIPT_DIR=$(SCRIPT_DIR)), checkout or import projects)
