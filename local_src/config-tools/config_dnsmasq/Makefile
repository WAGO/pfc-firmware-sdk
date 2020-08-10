#######################################################################################################################
# Makefile for PTXdist package config_dnsmasq
# This makefile uses the infrastructure provided by ct-build.
.SUFFIXES:  # Delete make's default suffix rules
.PHONY: default
default: all

#######################################################################################################################
# Overridable project configuration
PROJECT_ROOT ?= .
PTXDIST_PACKAGE ?= CONFIG_DNSMASQ
PTXDIST_PACKAGE_MK_FILE ?= $(PTXDIST_WORKSPACE)/rules/config_dnsmasq.make
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
MAIN_BUILDTARGETS += \
	config_dnsmasq_c.elf \
	libconfig_dnsmasq.a

TEST_BUILDTARGETS += \
	alltests.elf

BUILDTARGETS += \
	$(MAIN_BUILDTARGETS) \
	$(TEST_BUILDTARGETS)

INSTALL_TARGETS += \
	$(DESTDIR)/etc/config-tools/config_dnsmasq_c

#######################################################################################################################
# Settings shared between build targets
SHARED_CCXXFLAGS += -fPIC
SHARED_CCXXFLAGS += -fvisibility=hidden
SHARED_CFLAGS    += $(call option_std,gnu99)
SHARED_CXXFLAGS  += $(call option_std,gnu++14)
SHARED_CXXFLAGS  += -fvisibility-inlines-hidden
SHARED_INCLUDES  += 
SHARED_DISABLEDWARNINGS  += suggest-attribute=noreturn useless-cast

# $(call local_prerequisites,name)
local_prerequisites = $(call lib_buildtarget_raw,$($(1)_LIBS) $($(1)_PKG_CONFIG_LIBS),$($(1)_STATICALLYLINKED))


#######################################################################################################################
# Settings for build target libconfig_dnsmasq.a
libconfig_dnsmasq.a_LIBS             += 
libconfig_dnsmasq.a_STATICALLYLINKED +=
libconfig_dnsmasq.a_PKG_CONFIGS      += libnetconf glib-2.0
libconfig_dnsmasq.a_DISABLEDWARNINGS += $(SHARED_DISABLEDWARNINGS)
libconfig_dnsmasq.a_PREREQUISITES    += $(call local_prerequisites,libconfig_dnsmasq.a)
libconfig_dnsmasq.a_SOURCES          += $(call glob_r,$(addprefix $(SRC_DIR)/config_dnsmasq/**/*.,$(SOURCE_FILE_EXTENSIONS)))
libconfig_dnsmasq.a_CPPFLAGS         += $(call pkg_config_cppflags,$(libconfig_dnsmasq.a_PKG_CONFIGS))
libconfig_dnsmasq.a_CPPFLAGS         += -I$(PROJECT_ROOT)/../libnet/ -I$(PROJECT_ROOT)/../liblog/
libconfig_dnsmasq.a_CCXXFLAGS        += $(SHARED_CCXXFLAGS)
libconfig_dnsmasq.a_CCXXFLAGS        += $(call option_disable_warning,$(libconfig_dnsmasq.a_DISABLEDWARNINGS))
libconfig_dnsmasq.a_CFLAGS           += $(SHARED_CFLAGS)
libconfig_dnsmasq.a_CFLAGS           += $(libconfig_dnsmasq.a_CCXXFLAGS)
libconfig_dnsmasq.a_CFLAGS           += $(call pkg_config_cflags,$(libconfig_dnsmasq.a_PKG_CONFIGS))
libconfig_dnsmasq.a_CXXFLAGS         += $(SHARED_CXXFLAGS)
libconfig_dnsmasq.a_CXXFLAGS         += $(libconfig_dnsmasq.a_CCXXFLAGS)
libconfig_dnsmasq.a_CXXFLAGS         += $(call pkg_config_cxxflags,$(libconfig_dnsmasq.a_PKG_CONFIGS))
libnetwork_config.a_CLANG_TIDY_CHECKS += $(GTEST_CLANG_TIDY_CHECKS)
libnetwork_config.a_CLANG_TIDY_CHECKS += -google-runtime-references
#libconfig_dnsmasq.a_LDFLAGS          += $(call option_lib,$(libconfig_dnsmasq.a_LIBS),libconfig_dnsmasq)
#libconfig_dnsmasq.a_LDFLAGS          += $(call pkg_config_ldflags,$(libconfig_dnsmasq.a_PKG_CONFIGS))


#######################################################################################################################
# Settings for build target alltests.elf
alltests.elf_LIBS             += config_dnsmasq gmock_main gmock boost_system ctnetwork ctlog
alltests.elf_STATICALLYLINKED += config_dnsmasq gmock_main gmock
alltests.elf_PKG_CONFIGS      += $(libconfig_dnsmasq.a_PKG_CONFIGS)
alltests.elf_PKG_CONFIGS      += $(libconfig_dnsmasq.so_PKG_CONFIGS)
alltests.elf_DISABLEDWARNINGS += $(SHARED_DISABLEDWARNINGS) 
alltests.elf_PREREQUISITES    += $(call local_prerequisites,alltests.elf)
alltests.elf_SOURCES          += $(call glob_r,$(addprefix $(PROJECT_ROOT)/test-src/**/*.,$(SOURCE_FILE_EXTENSIONS)))
alltests.elf_CPPFLAGS         += -I$(SRC_DIR)
alltests.elf_CPPFLAGS         += -I$(SRC_DIR)/config_dnsmasq
alltests.elf_CPPFLAGS         += $(call pkg_config_cppflags,$(alltests.elf_PKG_CONFIGS))
alltests.elf_CCXXFLAGS        += $(SHARED_CCXXFLAGS)
alltests.elf_CCXXFLAGS        += $(call option_disable_warning,$(alltests.elf_DISABLEDWARNINGS))
alltests.elf_CFLAGS           += $(SHARED_CFLAGS)
alltests.elf_CFLAGS           += $(alltests.elf_CCXXFLAGS)
alltests.elf_CFLAGS           += $(call pkg_config_cflags,$(alltests.elf_PKG_CONFIGS))
alltests.elf_CXXFLAGS         += $(SHARED_CXXFLAGS)
alltests.elf_CXXFLAGS         += $(alltests.elf_CCXXFLAGS)
alltests.elf_CXXFLAGS         += $(call pkg_config_cxxflags,$(alltests.elf_PKG_CONFIGS))
alltests.elf_LDFLAGS          += $(call option_lib,$(alltests.elf_LIBS),alltests.elf)
alltests.elf_LDFLAGS          += $(call pkg_config_ldflags,$(alltests.elf_PKG_CONFIGS))
alltests.elf_CLANG_TIDY_CHECKS += $(GTEST_CLANG_TIDY_CHECKS)


#######################################################################################################################
# Settings for build target config_dnsmasq_c.elf
config_dnsmasq_c.elf_LIBS             += config_dnsmasq ctnetwork ctlog glib-2.0
config_dnsmasq_c.elf_STATICALLYLINKED += config_dnsmasq
config_dnsmasq_c.elf_PKG_CONFIGS      += libnetconf glib-2.0
config_dnsmasq_c.elf_DISABLEDWARNINGS += $(SHARED_DISABLEDWARNINGS)
config_dnsmasq_c.elf_PREREQUISITES    += $(call local_prerequisites,config_dnsmasq_c.elf)
config_dnsmasq_c.elf_SOURCES          += $(call glob_r,$(addprefix $(PROJECT_ROOT)/src/app/**/*.,$(SOURCE_FILE_EXTENSIONS)))
config_dnsmasq_c.elf_CPPFLAGS         += -I$(SRC_DIR)
config_dnsmasq_c.elf_CPPFLAGS         += -I$(SRC_DIR)/config_dnsmasq
config_dnsmasq_c.elf_CPPFLAGS         += $(call pkg_config_cppflags,$(config_dnsmasq_c.elf_PKG_CONFIGS))
config_dnsmasq_c.elf_CCXXFLAGS        += $(SHARED_CCXXFLAGS)
config_dnsmasq_c.elf_CCXXFLAGS        += $(call option_disable_warning,$(config_dnsmasq_c.elf_DISABLEDWARNINGS))
config_dnsmasq_c.elf_CFLAGS           += $(SHARED_CFLAGS)
config_dnsmasq_c.elf_CFLAGS           += $(config_dnsmasq_c.elf_CCXXFLAGS)
config_dnsmasq_c.elf_CFLAGS           += $(call pkg_config_cflags,$(config_dnsmasq_c.elf_PKG_CONFIGS))
config_dnsmasq_c.elf_CXXFLAGS         += $(SHARED_CXXFLAGS)
config_dnsmasq_c.elf_CXXFLAGS         += $(config_dnsmasq_c.elf_CCXXFLAGS)
config_dnsmasq_c.elf_CXXFLAGS         += $(call pkg_config_cxxflags,$(config_dnsmasq_c.elf_PKG_CONFIGS))
config_dnsmasq_c.elf_LDFLAGS          += $(call option_lib,$(config_dnsmasq_c.elf_LIBS),config_dnsmasq_c.elf)
config_dnsmasq_c.elf_LDFLAGS          += $(call pkg_config_ldflags,$(config_dnsmasq_c.elf_PKG_CONFIGS))


#######################################################################################################################
# Build infrastructure

#/# Include variables and rules on how to build a project for the C language.
include $(SCRIPT_DIR)/buildclang_1.mk
#/# Include support for checking all executables with valgrind, on-target.
-include $(PTXDIST_WORKSPACE)/../wago_intern/debugging/scripts/valgrind-target.mk


#######################################################################################################################
# Custom rules

#/# This section can be used to define specific make rules, as opposed to the rest of this file, 
#/# where only configuration takes place.
#/# One example for specific rules is how to install header or packageconfig files.
#/#
#/# Define a non-standard location for installed header files.
#/# As this package installs "dozens of headers" (i.e. one), it decides to install them into a separate folder 
#/# instead of into just /usr/include.
#/# Use a separate folder if your package contains many headers that may be included more easily if they can be all
#/# found in one place, or if you suspect that their names may clash with other packages' headers' if placed in the
#/# same folder. This location is also published by config_dnsmasq.pc.
#/# DESTDIR: https://www.gnu.org/prep/standards/html_node/DESTDIR.html
HDR_DIR=$(DESTDIR)/usr/include/config_dnsmasq

#/# This package provides a pkg-config file for use by other packages.
#$(DESTDIR)/usr/lib/pkgconfig/%.pc: $(PROJECT_ROOT)/%.pc | $(DESTDIR)/usr/lib/pkgconfig
#	cp $< $@

#/# Install binaries.
$(DESTDIR)/etc/config-tools/%: $(BIN_DIR)/%.elf | $(DESTDIR)/etc/config-tools
	cp $< $@

$(DESTDIR)/etc/config-tools: $(DESTDIR)
	mkdir -p $(DESTDIR)/etc/config-tools

#######################################################################################################################
# Bootstrapping

#/# This rule complains if the folder pointed to by SCRIPT_DIR does not contain any .mk files.
$(SCRIPT_DIR)/%.mk:
	$(error build scripts unavailable ($@) - set script dir (SCRIPT_DIR=$(SCRIPT_DIR)))

