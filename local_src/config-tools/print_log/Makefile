#######################################################################################################################
# Makefile for PTXdist package print_log
# This makefile uses the infrastructure provided by ct-build.
.SUFFIXES:  # Delete make's default suffix rules
.PHONY: default
default: all

#######################################################################################################################
# Overridable project configuration
PROJECT_ROOT ?= .
PTXDIST_PACKAGE ?= PRINT_LOG
PTXDIST_PACKAGE_MK_FILE ?= $(PTXDIST_WORKSPACE)/rules/print_log.make
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
	print_log.elf \
	libutil_log.a

TEST_BUILDTARGETS += \
	alltests.elf

BUILDTARGETS += \
	$(MAIN_BUILDTARGETS) \
	$(TEST_BUILDTARGETS)

INSTALL_TARGETS += \
	$(DESTDIR)/etc/config-tools/print_log

#######################################################################################################################
# Settings shared between build targets
SHARED_CCXXFLAGS += -fPIC
SHARED_CCXXFLAGS += -fvisibility=hidden
SHARED_CFLAGS    += $(call option_std,gnu99)
SHARED_CXXFLAGS  += $(call option_std,gnu++14)
SHARED_CXXFLAGS  += -fvisibility-inlines-hidden

# $(call local_prerequisites,name)
local_prerequisites = $(call lib_buildtarget_raw,$($(1)_LIBS) $($(1)_PKG_CONFIG_LIBS),$($(1)_STATICALLYLINKED))

#######################################################################################################################
# Settings for build target libutil_log.a
libutil_log.a_LIBS             += boost_system boost_filesystem
libutil_log.a_STATICALLYLINKED +=
libutil_log.a_PKG_CONFIGS      += glib-2.0
libutil_log.a_DISABLEDWARNINGS += $(SHARED_DISABLEDWARNINGS)
libutil_log.a_PREREQUISITES    += $(call local_prerequisites,libutil_log.a)
libutil_log.a_SOURCES          += $(call glob_r,$(addprefix $(SRC_DIR)/libutil_log/**/*.,$(SOURCE_FILE_EXTENSIONS)))
libutil_log.a_CPPFLAGS         += $(call pkg_config_cppflags,$(libutil_log.a_PKG_CONFIGS))
libutil_log.a_CPPFLAGS         += -I$(PROJECT_ROOT)/../liblog/
libutil_log.a_CCXXFLAGS        += $(SHARED_CCXXFLAGS)
libutil_log.a_CCXXFLAGS        += $(call option_disable_warning,$(libutil_log.a_DISABLEDWARNINGS))
libutil_log.a_CFLAGS           += $(SHARED_CFLAGS)
libutil_log.a_CFLAGS           += $(libutil_log.a_CCXXFLAGS)
libutil_log.a_CFLAGS           += $(call pkg_config_cflags,$(libutil_log.a_PKG_CONFIGS))
libutil_log.a_CXXFLAGS         += $(SHARED_CXXFLAGS)
libutil_log.a_CXXFLAGS         += $(libutil_log.a_CCXXFLAGS)
libutil_log.a_CXXFLAGS         += $(call pkg_config_cxxflags,$(libutil_log.a_PKG_CONFIGS))
libutil_log.a_CLANG_TIDY_CHECKS += $(GTEST_CLANG_TIDY_CHECKS)
libutil_log.a_CLANG_TIDY_CHECKS += -google-runtime-references

#######################################################################################################################
# Settings for build target alltests.elf
alltests.elf_LIBS              += util_log gmock_main gmock boost_system boost_filesystem
alltests.elf_STATICALLYLINKED  +=
alltests.elf_PKG_CONFIGS       += $(libutil_log.a_PKG_CONFIGS)
alltests.elf_DISABLEDWARNINGS  += $(SHARED_DISABLEDWARNINGS) 
alltests.elf_PREREQUISITES     += $(call local_prerequisites,alltests.elf)
alltests.elf_SOURCES           += $(call glob_r,$(addprefix $(PROJECT_ROOT)/test-src/**/*.,$(SOURCE_FILE_EXTENSIONS)))
alltests.elf_CPPFLAGS          += -I$(SRC_DIR)
alltests.elf_CPPFLAGS          += -I$(SRC_DIR)/libutil_log
alltests.elf_CPPFLAGS          += -isystem ${SYSROOT}/usr/include/glib-2.0
alltests.elf_CPPFLAGS          += $(call pkg_config_cppflags,$(alltests.elf_PKG_CONFIGS))
alltests.elf_CCXXFLAGS         += $(SHARED_CCXXFLAGS)
alltests.elf_CCXXFLAGS         += $(call option_disable_warning,$(alltests.elf_DISABLEDWARNINGS))
alltests.elf_CFLAGS            += $(SHARED_CFLAGS)
alltests.elf_CFLAGS            += $(alltests.elf_CCXXFLAGS)
alltests.elf_CFLAGS            += $(call pkg_config_cflags,$(alltests.elf_PKG_CONFIGS))
alltests.elf_CXXFLAGS          += $(SHARED_CXXFLAGS)
alltests.elf_CXXFLAGS          += $(alltests.elf_CCXXFLAGS)
alltests.elf_CXXFLAGS          += $(call pkg_config_cxxflags,$(alltests.elf_PKG_CONFIGS))
alltests.elf_LDFLAGS           += $(call option_lib,$(alltests.elf_LIBS),alltests.elf)
alltests.elf_LDFLAGS           += $(call pkg_config_ldflags,$(alltests.elf_PKG_CONFIGS))
alltests.elf_CLANG_TIDY_CHECKS += $(GTEST_CLANG_TIDY_CHECKS)
alltests.elf_CLANG_TIDY_CHECKS += -google-runtime-references

#######################################################################################################################
# Settings for build target print_file.elf
print_log.elf_LIBS             += util_log boost_system boost_filesystem
print_log.elf_STATICALLYLINKED += util_log
print_log.elf_PKG_CONFIGS      += glib-2.0
print_log.elf_DISABLEDWARNINGS += $(SHARED_DISABLEDWARNINGS)
print_log.elf_PREREQUISITES    += $(call local_prerequisites,print_log.elf)
print_log.elf_SOURCES          += $(call glob_r,$(addprefix $(PROJECT_ROOT)/src/app/**/*.,$(SOURCE_FILE_EXTENSIONS)))
print_log.elf_CPPFLAGS         += -I$(SRC_DIR)
print_log.elf_CPPFLAGS         += -I$(SRC_DIR)/libutil_log
print_log.elf_CPPFLAGS         += $(call pkg_config_cppflags,$(print_log.elf_PKG_CONFIGS))
print_log.elf_CCXXFLAGS        += $(SHARED_CCXXFLAGS)
print_log.elf_CCXXFLAGS        += $(call option_disable_warning,$(print_log.elf_DISABLEDWARNINGS))
print_log.elf_CFLAGS           += $(SHARED_CFLAGS)
print_log.elf_CFLAGS           += $(print_log.elf_CCXXFLAGS)
print_log.elf_CFLAGS           += $(call pkg_config_cflags,$(print_log.elf_PKG_CONFIGS))
print_log.elf_CXXFLAGS         += $(SHARED_CXXFLAGS)
print_log.elf_CXXFLAGS         += $(print_log.elf_CCXXFLAGS)
print_log.elf_CXXFLAGS         += $(call pkg_config_cxxflags,$(print_log.elf_PKG_CONFIGS))
print_log.elf_LDFLAGS          += $(call option_lib,$(print_log.elf_LIBS),print_log.elf)
print_log.elf_LDFLAGS          += $(call pkg_config_ldflags,$(print_log.elf_PKG_CONFIGS))

#######################################################################################################################
# Build infrastructure

#/# Include variables and rules on how to build a project for the C language.
include $(SCRIPT_DIR)/buildclang_1.mk
#/# Include support for checking all executables with valgrind, on-target.
-include $(PTXDIST_WORKSPACE)/../wago_intern/debugging/scripts/valgrind-target.mk

#######################################################################################################################
# Custom rules

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

