#######################################################################################################################
# Makefile for PTXdist package netconfd
# This makefile uses the infrastructure provided by ct-build.


.SUFFIXES:

.PHONY: default
default: all

#######################################################################################################################
# Overridable project configuration

PROJECT_ROOT ?= .

PTXDIST_PACKAGE ?= NETCONFD
PTXDIST_PACKAGE_MK_FILE ?= $(PTXDIST_WORKSPACE)/rules/netconfd.make

INCLUDES += \
-I$(PROJECT_ROOT)/src/

CFLAGS += $(call option_std,c99)
CXXFLAGS += $(call option_std,c++14)

CLANG_TIDY_HEADER_FILTER :=

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

MODULES += \
bal/module.mk \
libbridge/module.mk \
netconfd/module.mk \
dbus/module.mk \
utility/module.mk \
clientapi/module.mk

MK_FILES += $(MODULES)

MAIN_BUILDTARGETS += \
netconfd.elf

INSTALL_TARGETS += \
$(DESTDIR)/usr/bin/netconfd.elf

BUILDTARGETS += $(MAIN_BUILDTARGETS) $(TEST_BUILDTARGETS)

OPTION_HIDE_SYMBOLS ?= -fvisibility=hidden

netconfd.elf_INCLUDES = \
-I$(PROJECT_ROOT)/src/ \
-I$(PROJECT_ROOT)/extern/ \
-I$(PROJECT_ROOT)/netconfd/extern/


#######################################################################################################################
# Settings for build target netconfd

netconfd.elf_VERSION = $(NETCONFD_VERSION)

netconfd.elf_DISABLEDWARNINGS += packed
netconfd.elf_CDISABLEDWARNINGS += $(netconfd.so_DISABLEDWARNINGS)  
netconfd.elf_CXXDISABLEDWARNINGS += $(netconfd.so_DISABLEDWARNINGS) abi-tag useless-cast
netconfd.elf_STATICALLYLINKED += netconfd bal utility bridge netconfddbus
netconfd.elf_LIBS += netconfd bal boost_log utility bridge boost_thread boost_system boost_filesystem boost_serialization netconfddbus
netconfd.elf_PKG_CONFIGS += $(call uniq,$(libbal.a_PKG_CONFIGS) $(libbridge.a_PKG_CONFIGS) $(libnetconfddbus.a_PKG_CONFIGS) $(libutility.a_PKG_CONFIGS))
netconfd.elf_PKG_CONFIG_LIBS += $(call uniq,$(libbal.a_PKG_CONFIG_LIBS) $(libbridge.a_PKG_CONFIG_LIBS))
netconfd.elf_PREREQUISITES += $(call lib_buildtarget_raw, $(netconfd.elf_LIBS) $(netconfd.elf_PKG_CONFIG_LIBS),$(netconfd.elf_STATICALLYLINKED))
netconfd.elf_INCLUDES += $(call uniq,$(libnetconfd.a_INCLUDES) $(libbal.a_INCLUDES) $(libbridge.a_INCLUDES) $(libnetconfddbus.a_INCLUDES))
netconfd.elf_INCLUDES += -I$(PROJECT_ROOT)/extern/
netconfd.elf_DEFINES += $(call uniq,$(libbal.a_DEFINES)) -DBOOST_LOG_DYN_LINK -DNETCONFD_VERSION=$(netconfd.elf_VERSION)
netconfd.elf_CPPFLAGS += $(netconfd.elf_INCLUDES)
netconfd.elf_CPPFLAGS += $(netconfd.elf_DEFINES)
netconfd.elf_CPPFLAGS += $(call pkg_config_cppflags,$(netconfd.elf_PKG_CONFIGS))
netconfd.elf_CCXXFLAGS += $(OPTION_HIDE_SYMBOLS)
netconfd.elf_CCXXFLAGS += $(OPTION_PIC)
netconfd.elf_CCXXFLAGS += $(netconfd.elf_DEFINES)
netconfd.elf_CXXFLAGS += $(call option_disable_warning,$(netconfd.elf_CXXDISABLEDWARNINGS))
netconfd.elf_CXXFLAGS += $(call pkg_config_cxxflags,$(netconfd.elf_PKG_CONFIGS))
netconfd.elf_CXXFLAGS += $(netconfd.elf_CCXXFLAGS)
netconfd.elf_CFLAGS += $(netconfd.elf_CCXXFLAGS)
netconfd.elf_CFLAGS += $(call option_disable_warning,$(netconfd.elf_CDISABLEDWARNINGS))
netconfd.elf_LDFLAGS += $(OPTION_PIC)
netconfd.elf_LDFLAGS += -Wl,--no-undefined
netconfd.elf_LDFLAGS += -Wl,--exclude-libs=ALL
netconfd.elf_LDFLAGS += $(call option_lib,$(netconfd.elf_LIBS),netconfd.elf)
netconfd.elf_LDFLAGS += $(call pkg_config_ldflags,$(netconfd.elf_PKG_CONFIGS))
netconfd.elf_SOURCES += $(call fglob_r,$(PROJECT_ROOT)/src,$(SOURCE_FILE_EXTENSIONS))
netconfd.elf_CLANG_TIDY_RULESET = $(CLANG_TIDY_CHECKS)
netconfd.elf_CLANG_TIDY_CHECKS += -clang-diagnostic-c++98-c++11-compat
netconfd.elf_CLANG_TIDY_CHECKS += -google-runtime-references

#######################################################################################################################
# Settings for module build targets

include $(MODULES)

#######################################################################################################################
# Build infrastructure

-include $(SCRIPT_DIR)/buildclang_1.mk
-include $(PTXDIST_WORKSPACE)/../wago_intern/debugging/scripts/valgrind-target.mk

#######################################################################################################################
# Custom rules

$(DESTDIR)/usr/include/% : $(PROJECT_ROOT)/extern/%
	mkdir -p $(dir $@) && cp $(realpath $<) $@

# install files into 'etc' folder
$(DESTDIR)/etc/% : $(PROJECT_ROOT)/etc/%
	mkdir -p $(dir $@) && cp $(realpath $<) $@

#######################################################################################################################
# Bootstrapping

$(SCRIPT_DIR)/%.mk :
	$(error build scripts unavailable - set script dir (SCRIPT_DIR=$(SCRIPT_DIR)) or checkout ct-build)

	