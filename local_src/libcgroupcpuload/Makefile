###############################################################################
#  Copyright (c) WAGO Kontakttechnik GmbH & Co. KG
#
#  PROPRIETARY RIGHTS are involved in the subject matter of this material.
#  All manufacturing, reproduction, use and sales rights pertaining to this
#  subject matter are governed by the license agreement. The recipient of this
#  software implicitly accepts the terms of the license.
###############################################################################
.SUFFIXES:
 
.PHONY: default
default: all

PROJECT_ROOT ?= .

PTXDIST_PACKAGE ?= LIBCGROUPCPULOAD
PTXDIST_PACKAGE_MK_FILE ?= $(PTXDIST_WORKSPACE)/rules/libcgroupcpuload.make

SRC_DIR = $(PROJECT_ROOT)/src

#######################################################################################################################
# Optional package/ platform feature configuration

include $(SCRIPT_DIR)/platform_1.mk

#######################################################################################################################
# Custom toolchain configuration

LINT_RULE_FILES = $(LINT_CONFIG_DIR)/lint-rules/pfc.lnt

#######################################################################################################################
# Build target configuration

MAIN_BUILDTARGETS += \
	libcgroupcpuload.so \
	loadcompare.elf \
	checkcpu.elf \
	bellysmile.elf \
	testapp.elf \
	cpuloadmonitor.elf

# This package does not contain any tests yet.
#TEST_BUILDTARGETS += \
	alltests.elf

BUILDTARGETS += \
	$(MAIN_BUILDTARGETS) \
	$(TEST_BUILDTARGETS)

INSTALL_TARGETS += \
	$(DESTDIR)/usr/lib/libcgroupcpuload.so.$(libcgroupcpuload.so_VERSION) \
	$(DESTDIR)/usr/include/cgroupcpuload.h

SHARED_DISABLEDWARNINGS += unused-function#missing-prototypes missing-declarations

SHARED_CFLAGS += $(call option_std,gnu99)
SHARED_CFLAGS	+= -D_GNU_SOURCE

libcgroupcpuload.so_VERSION           = $(LIBCGROUPCPULOAD_VERSION)

libcgroupcpuload.so_SOURCES						+= $(call glob_r,$(addprefix $(SRC_DIR)/cgroupcpuload/**/*.,$(SOURCE_FILE_EXTENSIONS)))
libcgroupcpuload.so_PKG_LIBS					+= dl rt
libcgroupcpuload.so_CFLAGS						+= $(call pkg_config_cflags,  $(libcgroupcpuload.so_PKG_CONFIGS))
libcgroupcpuload.so_CFLAGS						+= -fPIC -fvisibility=hidden
libcgroupcpuload.so_CFLAGS						+= -D'DLL_DECL=__attribute__ ((visibility("default")))'
libcgroupcpuload.so_CFLAGS						+= $(SHARED_CFLAGS)
libcgroupcpuload.so_CFLAGS						+= -fno-common -fmessage-length=0 -fno-omit-frame-pointer
libcgroupcpuload.so_LDFLAGS						+= $(call pkg_config_ldflags, $(libcgroupcpuload.so_PKG_CONFIGS))

loadcompare.elf_SOURCES								+= $(call glob_r,$(addprefix $(SRC_DIR)/loadcompare/**/*.,$(SOURCE_FILE_EXTENSIONS)))
loadcompare.elf_LIBS									+= cgroupcpuload
loadcompare.elf_CFLAGS								+= $(call option_disable_warning, unused-function)
loadcompare.elf_CFLAGS								+= -I $(SRC_DIR)/cgroupcpuload/
loadcompare.elf_CFLAGS								+= $(SHARED_CFLAGS)
loadcompare.elf_LDFLAGS								+= $(call option_lib_raw,$(loadcompare.elf_LIBS))
loadcompare.elf_PREREQUISITES					+= $(call lib_buildtarget_raw,$(loadcompare.elf_LIBS))

checkcpu.elf_SOURCES									+= $(call glob_r,$(addprefix $(SRC_DIR)/checkcpu/**/*.,$(SOURCE_FILE_EXTENSIONS)))
checkcpu.elf_LIBS											+= cgroupcpuload
checkcpu.elf_CFLAGS										+= -I $(SRC_DIR)/cgroupcpuload/
checkcpu.elf_CFLAGS										+= $(SHARED_CFLAGS)
checkcpu.elf_LDFLAGS									+= $(call option_lib_raw,$(checkcpu.elf_LIBS))
checkcpu.elf_PREREQUISITES						+= $(call lib_buildtarget_raw,$(checkcpu.elf_LIBS))

bellysmile.elf_SOURCES								+= $(call glob_r,$(addprefix $(SRC_DIR)/bellysmile/**/*.,$(SOURCE_FILE_EXTENSIONS)))
bellysmile.elf_LIBS										+= cgroupcpuload
bellysmile.elf_CFLAGS									+= -I $(SRC_DIR)/cgroupcpuload/
bellysmile.elf_CFLAGS									+= $(SHARED_CFLAGS)
bellysmile.elf_LDFLAGS								+= $(call option_lib_raw,$(bellysmile.elf_LIBS))
bellysmile.elf_PREREQUISITES					+= $(call lib_buildtarget_raw,$(bellysmile.elf_LIBS))

testapp.elf_SOURCES										+= $(call glob_r,$(addprefix $(SRC_DIR)/testapp/**/*.,$(SOURCE_FILE_EXTENSIONS)))
testapp.elf_LIBS											+= cgroupcpuload
testapp.elf_CFLAGS										+= -I $(SRC_DIR)/cgroupcpuload/
testapp.elf_CFLAGS										+= $(SHARED_CFLAGS)
testapp.elf_LDFLAGS										+= $(call option_lib_raw,$(testapp.elf_LIBS))
testapp.elf_PREREQUISITES							+= $(call lib_buildtarget_raw,$(testapp.elf_LIBS))


cpuloadmonitor.elf_SOURCES						+= $(call glob_r,$(addprefix $(SRC_DIR)/cpuloadmonitor/**/*.,$(SOURCE_FILE_EXTENSIONS)))
cpuloadmonitor.elf_LIBS								+= cgroupcpuload
cpuloadmonitor.elf_CFLAGS							+= -I $(SRC_DIR)/cgroupcpuload/
cpuloadmonitor.elf_CFLAGS							+= $(SHARED_CFLAGS)
cpuloadmonitor.elf_LDFLAGS						+= $(call option_lib_raw,$(cpuloadmonitor.elf_LIBS))
cpuloadmonitor.elf_PREREQUISITES			+= $(call lib_buildtarget_raw,$(cpuloadmonitor.elf_LIBS))

#######################################################################################################################
# Build infrastructure
include $(SCRIPT_DIR)/buildclang_1.mk

#######################################################################################################################
# Custom rules

$(DESTDIR)/usr/include/%.h: $(PROJECT_ROOT)/src/cgroupcpuload/%.h | $(DESTDIR)/usr/include
	cp $< $@

#######################################################################################################################
# Bootstrapping

$(SCRIPT_DIR)/%.mk :
	$(error build scripts unavailable ($@) - set script dir (SCRIPT_DIR=$(SCRIPT_DIR)))

