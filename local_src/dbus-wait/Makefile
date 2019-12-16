#######################################################################################################################
# Makefile for PTXdist package netconfd
# This makefile uses the infrastructure provided by ct-build.


.SUFFIXES:

.PHONY: default
default: all

#######################################################################################################################
# Overridable project configuration

PROJECT_ROOT ?= .

PTXDIST_PACKAGE ?= DBUS_WAIT
PTXDIST_PACKAGE_MK_FILE ?= $(PTXDIST_WORKSPACE)/rules/dbus-wait.make

CFLAGS += $(call option_std,c99)
CXXFLAGS += $(call option_std,c++14)

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
dbus-wait.elf

INSTALL_TARGETS += \
$(DESTDIR)/usr/bin/dbus-wait.elf

BUILDTARGETS += $(MAIN_BUILDTARGETS) $(TEST_BUILDTARGETS)

OPTION_HIDE_SYMBOLS ?= -fvisibility=hidden

dbus-wait.elf_INCLUDES =


#######################################################################################################################
# Settings for build target netconfd

dbus-wait.elf_VERSION = $(DBUS_WAIT_VERSION)

dbus-wait.elf_DISABLEDWARNINGS += unused-parameter unused-variable sign-conversion discarded-qualifiers
dbus-wait.elf_CDISABLEDWARNINGS += $(dbus-wait.elf_DISABLEDWARNINGS)  
dbus-wait.elf_CXXDISABLEDWARNINGS += $(dbus-wait.elf_DISABLEDWARNINGS) 
dbus-wait.elf_STATICALLYLINKED += 
dbus-wait.elf_LIBS +=
dbus-wait.elf_PKG_CONFIGS += dbus-1
dbus-wait.elf_PKG_CONFIG_LIBS += 
dbus-wait.elf_PREREQUISITES += 
dbus-wait.elf_INCLUDES += 
dbus-wait.elf_DEFINES += 
dbus-wait.elf_CPPFLAGS += $(dbus-wait.elf_INCLUDES)
dbus-wait.elf_CPPFLAGS += $(dbus-wait.elf_DEFINES)
dbus-wait.elf_CPPFLAGS += $(call pkg_config_cppflags,$(dbus-wait.elf_PKG_CONFIGS))
dbus-wait.elf_CCXXFLAGS += $(OPTION_HIDE_SYMBOLS)
dbus-wait.elf_CCXXFLAGS += $(OPTION_PIC)
dbus-wait.elf_CCXXFLAGS += $(dbus-wait.elf_DEFINES)
dbus-wait.elf_CXXFLAGS += $(call option_disable_warning,$(dbus-wait.elf_CXXDISABLEDWARNINGS))
dbus-wait.elf_CXXFLAGS += $(call pkg_config_cxxflags,$(dbus-wait.elf_PKG_CONFIGS))
dbus-wait.elf_CXXFLAGS += $(dbus-wait.elf_CCXXFLAGS)
dbus-wait.elf_CFLAGS += $(dbus-wait.elf_CCXXFLAGS)
dbus-wait.elf_CFLAGS += $(call option_disable_warning,$(dbus-wait.elf_CDISABLEDWARNINGS))
dbus-wait.elf_LDFLAGS += $(OPTION_PIC)
dbus-wait.elf_LDFLAGS += -Wl,--no-undefined
dbus-wait.elf_LDFLAGS += -Wl,--exclude-libs=ALL
dbus-wait.elf_LDFLAGS += $(call option_lib,$(dbus-wait.elf_LIBS),dbus-wait.elf)
dbus-wait.elf_LDFLAGS += $(call pkg_config_ldflags,$(dbus-wait.elf_PKG_CONFIGS))
dbus-wait.elf_SOURCES += $(call fglob_r,$(PROJECT_ROOT)/src,$(SOURCE_FILE_EXTENSIONS))

dbus-wait.elf_CLANG_TIDY_RULESET = $(CLANG_TIDY_CHECKS)
dbus-wait.elf_CLANG_TIDY_CHECKS += -clang-diagnostic-c++98-c++11-compat
dbus-wait.elf_CLANG_TIDY_CHECKS += -google-runtime-references




#######################################################################################################################



#######################################################################################################################
# Settings for module build targets

include $(MODULES)

#######################################################################################################################
# Build infrastructure

-include $(SCRIPT_DIR)/buildclang_1.mk

#######################################################################################################################
# Custom rules


#######################################################################################################################
# Bootstrapping

$(SCRIPT_DIR)/%.mk :
	$(error build scripts unavailable - set script dir (SCRIPT_DIR=$(SCRIPT_DIR)) or checkout ct-build)

	