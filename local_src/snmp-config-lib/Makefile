#######################################################################################################################
# Makefile for PTXdist package snmp-config-lib
# This makefile uses the infrastructure provided by ct-build.

.SUFFIXES:  # Delete make's default suffix rules

.PHONY: default
default: all

#######################################################################################################################
# Overridable project configuration

PROJECT_ROOT ?= .

PTXDIST_PACKAGE ?= SNMP_CONFIG_LIB
PTXDIST_PACKAGE_MK_FILE ?= $(PTXDIST_WORKSPACE)/rules/snmp-config-lib.make

SRC_DIR = $(PROJECT_ROOT)

#######################################################################################################################
# Optional package/ platform feature configuration

-include $(SCRIPT_DIR)/platform_1.mk

#######################################################################################################################
# Custom toolchain configuration

LINT_RULE_FILES = \
	$(LINT_CONFIG_DIR)/lint-rules/pfc.lnt

SNMPCONFIGLIB_HEADER_INSTALL_DIR = $(DESTDIR)/usr/include/snmp-config-lib

SNMPCPNFIGLIB_INSTALL_HEADERS = $(call fglob_r,$(PROJECT_ROOT)/extern, h hpp)
SNMPCPNFIGLIB_HEADER_INSTALL_TARGETS = $(addprefix $(SNMPCONFIGLIB_HEADER_INSTALL_DIR)/, $(notdir $(SNMPCPNFIGLIB_INSTALL_HEADERS)))


#######################################################################################################################
# Build target configuration

MAIN_BUILDTARGETS += \
	libsnmpconfig.a \
	libsnmpconfig.so

TEST_BUILDTARGETS += \
	alltests.elf

BUILDTARGETS += \
	$(MAIN_BUILDTARGETS) \
	$(TEST_BUILDTARGETS)

INSTALL_TARGETS += \
	$(SNMPCPNFIGLIB_HEADER_INSTALL_TARGETS) \
	$(DESTDIR)/usr/lib/pkgconfig/snmp-config-lib.pc \
	$(DESTDIR)/usr/lib/libsnmpconfig.so.$(SNMP_CONFIG_LIB_VERSION)

#######################################################################################################################
# Settings shared between build targets

SHARED_CCXXFLAGS += -fPIC
#SHARED_CCXXFLAGS += -fvisibility=hidden
SHARED_CFLAGS    += $(call option_std,gnu99)
SHARED_CXXFLAGS  += $(call option_std,gnu++17)
#SHARED_CXXFLAGS  += -fvisibility-inlines-hidden

SHARED_INCLUDES  += -I$(SRC_DIR)/extern
SHARED_INCLUDES  += -I$(SRC_DIR)/src
SHARED_INCLUDES  += -I$(SRC_DIR)/src/util

# $(call local_prerequisites,name)
local_prerequisites = $(call lib_buildtarget_raw,$($(1)_LIBS) $($(1)_PKG_CONFIG_LIBS),$($(1)_STATICALLYLINKED))

#######################################################################################################################
# Settings for build target libsnmpconfig.so

libsnmpconfig.so_INCLUDES					+= $(SHARED_INCLUDES)
libsnmpconfig.so_VERSION          += $(SNMP_CONFIG_LIB_VERSION)
libsnmpconfig.so_LIBS             += 
libsnmpconfig.so_STATICALLYLINKED += 
libsnmpconfig.so_PKG_CONFIGS      += 
libsnmpconfig.so_DISABLEDWARNINGS += 
libsnmpconfig.so_PREREQUISITES    += $(call local_prerequisites,libsnmpconfig.so)
libsnmpconfig.so_SOURCES          += $(call glob_r,$(addprefix $(SRC_DIR)/src/**/*.,$(SOURCE_FILE_EXTENSIONS)))
libsnmpconfig.so_CPPFLAGS 				+= $(call uniq, $(libsnmpconfig.so_INCLUDES))
libsnmpconfig.so_CPPFLAGS         += $(call pkg_config_cppflags,$(libsnmpconfig.so_PKG_CONFIGS))
libsnmpconfig.so_CPPFLAGS         += -D'SNMP_CONFIG_LIB_API=WC_API_EXPORT'
libsnmpconfig.so_CPPFLAGS         += -isystem ${SYSROOT}/usr/include/glib-2.0
libsnmpconfig.so_CCXXFLAGS        += $(SHARED_CCXXFLAGS)
libsnmpconfig.so_CCXXFLAGS        += $(call option_disable_warning,$(libsnmpconfig.so_DISABLEDWARNINGS))
libsnmpconfig.so_CCXXFLAGS 				+= $(OPTION_PIC)
libsnmpconfig.so_CFLAGS           += $(SHARED_CFLAGS)
libsnmpconfig.so_CFLAGS           += $(libsnmpconfig.so_CCXXFLAGS)
libsnmpconfig.so_CFLAGS           += $(call pkg_config_cflags,$(libsnmpconfig.so_PKG_CONFIGS))
libsnmpconfig.so_CXXFLAGS         += $(SHARED_CXXFLAGS)
libsnmpconfig.so_CXXFLAGS         += $(libsnmpconfig.so_CCXXFLAGS)
libsnmpconfig.so_CXXFLAGS         += $(call pkg_config_cxxflags,$(libsnmpconfig.so_PKG_CONFIGS))
libsnmpconfig.so_LDFLAGS          += $(call option_lib,$(libsnmpconfig.so_LIBS) $(libsnmpconfig.so_PKG_CONFIG_LIBS),libsnmpconfig.so)
libsnmpconfig.so_LDFLAGS          += $(call pkg_config_ldflags,$(libsnmpconfig.so_PKG_CONFIGS))
libsnmpconfig.so_CLANG_TIDY_RULESET = $(CLANG_TIDY_CHECKS)
libsnmpconfig.so_CLANG_TIDY_CHECKS += -clang-diagnostic-c++98-c++11-c++14-compat
libsnmpconfig.so_CLANG_TIDY_CHECKS += -clang-diagnostic-c++98-c++11-compat
libsnmpconfig.so_CLANG_TIDY_CHECKS += -google-runtime-references

#######################################################################################################################
# Settings for build target libsnmpconfig.a

libsnmpconfig.a_INCLUDES				 += $(SHARED_INCLUDES)
libsnmpconfig.a_STATICALLYLINKED +=
libsnmpconfig.a_PKG_CONFIGS      += glib-2.0
libsnmpconfig.a_DISABLEDWARNINGS += useless-cast
libsnmpconfig.a_PREREQUISITES    += $(call local_prerequisites,libsnmpconfig.a)
libsnmpconfig.a_SOURCES          += $(call glob_r,$(addprefix $(SRC_DIR)/src/**/*.,$(SOURCE_FILE_EXTENSIONS)))
libsnmpconfig.a_CPPFLAGS         += $(libsnmpconfig.a_INCLUDES)
libsnmpconfig.a_CPPFLAGS         += $(call pkg_config_cppflags,$(libsnmpconfig.a_PKG_CONFIGS))
libsnmpconfig.a_CPPFLAGS         += -isystem ${SYSROOT}/usr/include/glib-2.0
libsnmpconfig.a_CCXXFLAGS        += $(SHARED_CCXXFLAGS)
libsnmpconfig.a_CCXXFLAGS        += $(call option_disable_warning,$(libsnmpconfig.a_DISABLEDWARNINGS))
libsnmpconfig.a_CCXXFLAGS 			 += $(OPTION_PIC)
libsnmpconfig.a_CFLAGS           += $(SHARED_CFLAGS)
libsnmpconfig.a_CFLAGS           += $(libsnmpconfig.a_CCXXFLAGS)
libsnmpconfig.a_CFLAGS           += $(call pkg_config_cflags,$(libsnmpconfig.a_PKG_CONFIGS))
libsnmpconfig.a_CXXFLAGS         += $(SHARED_CXXFLAGS)
libsnmpconfig.a_CXXFLAGS         += $(libsnmpconfig.a_CCXXFLAGS)
libsnmpconfig.a_CXXFLAGS         += $(call pkg_config_cxxflags,$(libsnmpconfig.a_PKG_CONFIGS))
libsnmpconfig.a_LDFLAGS          += $(call option_lib,$(libsnmpconfig.a_LIBS) $(libsnmpconfig.a_PKG_CONFIG_LIBS),libsnmpconfig.a)
libsnmpconfig.a_LDFLAGS          += $(call pkg_config_ldflags,$(libsnmpconfig.a_PKG_CONFIGS))
libsnmpconfig.a_CLANG_TIDY_RULESET = $(CLANG_TIDY_CHECKS)
libsnmpconfig.a_CLANG_TIDY_CHECKS += -clang-diagnostic-c++98-c++11-c++14-compat
libsnmpconfig.a_CLANG_TIDY_CHECKS += -clang-diagnostic-c++98-c++11-compat
libsnmpconfig.a_CLANG_TIDY_CHECKS += -google-runtime-references

#######################################################################################################################
# Settings for build target alltests.elf

alltests.elf_LIBS             += gmock_main gmock snmpconfig $(libsnmpconfig.a_LIBS)
alltests.elf_STATICALLYLINKED += gmock_main gmock snmpconfig
alltests.elf_PKG_CONFIGS      += 
alltests.elf_PKG_CONFIGS      += $(libsnmpconfig.a_PKG_CONFIGS)
alltests.elf_DISABLEDWARNINGS += suggest-override
alltests.elf_PREREQUISITES    += $(call local_prerequisites,alltests.elf)
alltests.elf_SOURCES          += $(call glob_r,$(addprefix $(PROJECT_ROOT)/test-src/**/*.,$(SOURCE_FILE_EXTENSIONS)))
alltests.elf_CPPFLAGS         += $(libsnmpconfig.a_INCLUDES)
alltests.elf_CPPFLAGS         += $(call pkg_config_cppflags,$(alltests.elf_PKG_CONFIGS))
alltests.elf_CPPFLAGS         += -isystem ${SYSROOT}/usr/include/glib-2.0
alltests.elf_CCXXFLAGS        += $(SHARED_CCXXFLAGS)
alltests.elf_CCXXFLAGS        += $(call option_disable_warning,$(alltests.elf_DISABLEDWARNINGS))
alltests.elf_CFLAGS           += $(SHARED_CFLAGS)
alltests.elf_CFLAGS           += $(alltests.elf_CCXXFLAGS)
alltests.elf_CFLAGS           += $(call pkg_config_cflags,$(alltests.elf_PKG_CONFIGS))
alltests.elf_CXXFLAGS         += $(SHARED_CXXFLAGS)
alltests.elf_CXXFLAGS         += $(alltests.elf_CCXXFLAGS)
alltests.elf_CXXFLAGS         += $(call pkg_config_cxxflags,$(alltests.elf_PKG_CONFIGS))
alltests.elf_LDFLAGS          += $(call option_lib,$(alltests.elf_LIBS) $(alltests.elf_PKG_CONFIG_LIBS),alltests.elf)
alltests.elf_LDFLAGS          += $(call pkg_config_ldflags,$(alltests.elf_PKG_CONFIGS))
alltests.elf_CLANG_TIDY_RULESET = $(CLANG_TIDY_CHECKS)
alltests.elf_CLANG_TIDY_CHECKS += -clang-diagnostic-c++98-c++11-c++14-compat
alltests.elf_CLANG_TIDY_CHECKS += -clang-diagnostic-c++98-c++11-compat
alltests.elf_CLANG_TIDY_CHECKS += -google-runtime-references
alltests.elf_CLANG_TIDY_CHECKS += $(GTEST_CLANG_TIDY_CHECKS)

#######################################################################################################################
# Build infrastructure

include $(SCRIPT_DIR)/buildclang_1.mk
-include $(PTXDIST_WORKSPACE)/../wago_intern/debugging/scripts/valgrind-target.mk


#######################################################################################################################
# Custom rules

$(SNMPCONFIGLIB_HEADER_INSTALL_DIR)/% : $(PROJECT_ROOT)/extern/% 
	mkdir -p $(dir $@) && cp $(realpath $<) $@
		
$(DESTDIR)/usr/lib/pkgconfig/%.pc: $(PROJECT_ROOT)/%.pc | $(DESTDIR)/usr/lib/pkgconfig
	cp $< $@

$(DESTDIR)/usr/bin/%: $(BIN_DIR)/%.elf | $(DESTDIR)/usr/bin
	cp $< $@

#######################################################################################################################
# Bootstrapping

$(SCRIPT_DIR)/%.mk:
	$(error build scripts unavailable ($@) - set script dir (SCRIPT_DIR=$(SCRIPT_DIR)))

