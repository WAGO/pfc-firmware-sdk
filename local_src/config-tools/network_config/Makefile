#######################################################################################################################
# Makefile for PTXdist package network_config
# This makefile uses the infrastructure provided by ct-build.
.SUFFIXES:  # Delete make's default suffix rules
.PHONY: default
default: all

#######################################################################################################################
# Overridable project configuration
PROJECT_ROOT ?= .
PTXDIST_PACKAGE ?= NETWORK_CONFIG
PTXDIST_PACKAGE_MK_FILE ?= $(PTXDIST_WORKSPACE)/rules/network_config.make
SRC_DIR = $(PROJECT_ROOT)/src

#CLANG_TIDY_HEADER_FILTER :=

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
	network_config.elf \
	libnetwork_config.a
	
TEST_BUILDTARGETS += \
	alltests.elf
	
BUILDTARGETS += \
	$(MAIN_BUILDTARGETS) \
	$(TEST_BUILDTARGETS)
	
INSTALL_TARGETS += \
	$(DESTDIR)/usr/bin/network_config
	
	
#######################################################################################################################
# Settings shared between build targets
SHARED_CCXXFLAGS += -fPIC
SHARED_CCXXFLAGS += -fvisibility=hidden
SHARED_CFLAGS    += $(call option_std,gnu99)
SHARED_CXXFLAGS  += $(call option_std,gnu++14)
SHARED_CXXFLAGS  += -fvisibility-inlines-hidden
SHARED_INCLUDES  += -I$(PROJECT_ROOT)/src/network_config/ \
										-I$(PROJECT_ROOT)/src/network_config/handler/
# $(call local_prerequisites,name)
local_prerequisites = $(call lib_buildtarget_raw,$($(1)_LIBS) $($(1)_PKG_CONFIG_LIBS),$($(1)_STATICALLYLINKED))

#######################################################################################################################
# Settings for build target libnetwork_config.a
libnetwork_config.a_LIBS             += 
libnetwork_config.a_STATICALLYLINKED +=
libnetwork_config.a_PKG_CONFIGS      += libnetconf gio-unix-2.0 glib-2.0
libnetwork_config.a_DISABLEDWARNINGS += 
libnetwork_config.a_PREREQUISITES    += $(call local_prerequisites,libnetwork_config.a)
libnetwork_config.a_SOURCES          += $(call glob_r,$(addprefix $(SRC_DIR)/network_config/**/*.,$(SOURCE_FILE_EXTENSIONS)))
libnetwork_config.a_CPPFLAGS         += $(call pkg_config_cppflags,$(libnetwork_config.a_PKG_CONFIGS))
libnetwork_config.a_CPPFLAGS         += $(SHARED_INCLUDES) 
libnetwork_config.a_CCXXFLAGS        += $(SHARED_CCXXFLAGS)
libnetwork_config.a_CFLAGS           += $(SHARED_CFLAGS)
libnetwork_config.a_CFLAGS           += $(libnetwork_config.a_CCXXFLAGS)
libnetwork_config.a_CFLAGS           += $(call pkg_config_cflags,$(libnetwork_config.a_PKG_CONFIGS))
libnetwork_config.a_CXXFLAGS         += $(SHARED_CXXFLAGS)
libnetwork_config.a_CXXFLAGS         += $(libnetwork_config.a_CCXXFLAGS)
libnetwork_config.a_CXXFLAGS         += $(call pkg_config_cxxflags,$(libnetwork_config.a_PKG_CONFIGS))
libnetwork_config.a_CLANG_TIDY_CHECKS += $(GTEST_CLANG_TIDY_CHECKS)
libnetwork_config.a_CLANG_TIDY_CHECKS += -google-runtime-references

#######################################################################################################################
# Settings for build target alltests.elf
alltests.elf_LIBS             += network_config gmock_main gmock boost_system boost_program_options
alltests.elf_STATICALLYLINKED += network_config gmock_main gmock
alltests.elf_PKG_CONFIGS      += $(libnetwork_config.a_PKG_CONFIGS)
alltests.elf_DISABLEDWARNINGS += suggest-override
alltests.elf_PREREQUISITES    += $(call local_prerequisites,alltests.elf)
alltests.elf_SOURCES          += $(call glob_r,$(addprefix $(PROJECT_ROOT)/test-src/**/*.,$(SOURCE_FILE_EXTENSIONS)))
alltests.elf_CPPFLAGS         += -I$(SRC_DIR)
alltests.elf_CPPFLAGS         += $(call pkg_config_cppflags,$(alltests.elf_PKG_CONFIGS))
alltests.elf_CPPFLAGS         += $(SHARED_INCLUDES)
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
alltests.elf_CLANG_TIDY_CHECKS += -google-runtime-references

#######################################################################################################################
# Settings for build target network_config.elf
network_config.elf_LIBS             += network_config netconf boost_system boost_program_options
network_config.elf_STATICALLYLINKED += netconf network_config 
network_config.elf_PKG_CONFIGS      += $(libnetwork_config.a_PKG_CONFIGS)
network_config.elf_DISABLEDWARNINGS += 
network_config.elf_PREREQUISITES    += $(call local_prerequisites,network_config.elf)
network_config.elf_SOURCES          += $(call glob_r,$(addprefix $(PROJECT_ROOT)/src/*.,$(SOURCE_FILE_EXTENSIONS)))
network_config.elf_CPPFLAGS         += -I$(SRC_DIR)
network_config.elf_CPPFLAGS         += $(SHARED_INCLUDES)
network_config.elf_CPPFLAGS         += $(call pkg_config_cppflags,$(network_config.elf_PKG_CONFIGS))
network_config.elf_CCXXFLAGS        += $(SHARED_CCXXFLAGS)
network_config.elf_CCXXFLAGS        += $(call option_disable_warning,$(network_config.elf_DISABLEDWARNINGS))
network_config.elf_CFLAGS           += $(SHARED_CFLAGS)
network_config.elf_CFLAGS           += $(network_config.elf_CCXXFLAGS)
network_config.elf_CFLAGS           += $(call pkg_config_cflags,$(network_config.elf_PKG_CONFIGS))
network_config.elf_CXXFLAGS         += $(SHARED_CXXFLAGS)
network_config.elf_CXXFLAGS         += $(network_config.elf_CCXXFLAGS)
network_config.elf_CXXFLAGS         += $(call pkg_config_cxxflags,$(network_config.elf_PKG_CONFIGS))
network_config.elf_LDFLAGS          += $(call option_lib,$(network_config.elf_LIBS),network_config.elf)
network_config.elf_LDFLAGS          += $(call pkg_config_ldflags,$(network_config.elf_PKG_CONFIGS))
network_config.elf_CLANG_TIDY_RULESET = $(CLANG_TIDY_CHECKS)
network_config.elf_CLANG_TIDY_CHECKS += -google-runtime-references

#######################################################################################################################
# Build infrastructure
include $(SCRIPT_DIR)/buildclang_1.mk
-include $(PTXDIST_WORKSPACE)/../wago_intern/debugging/scripts/valgrind-target.mk

#######################################################################################################################
# Custom rules
HDR_DIR=$(DESTDIR)/usr/include/network_config
$(DESTDIR)/usr/lib/pkgconfig/%.pc: $(PROJECT_ROOT)/%.pc | $(DESTDIR)/usr/lib/pkgconfig
	cp $< $@
$(DESTDIR)/usr/bin/%: $(BIN_DIR)/%.elf | $(DESTDIR)/usr/bin
	cp $< $@
$(HDR_DIR)/%.h: $(PROJECT_ROOT)/src/%.h | $(HDR_DIR)
	cp $< $@
$(HDR_DIR):
	mkdir -p $@
	
#######################################################################################################################
# Bootstrapping
$(SCRIPT_DIR)/%.mk:
	$(error build scripts unavailable ($@) - set script dir (SCRIPT_DIR=$(SCRIPT_DIR)))
