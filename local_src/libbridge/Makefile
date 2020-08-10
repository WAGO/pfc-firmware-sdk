#######################################################################################################################
# Makefile for PTXdist package libbridge
# This makefile uses the infrastructure provided by ct-build.
#/#
#/# Lines starting with '#/#' are explanations. If you want to use this example as a template for your own project,
#/# remove these lines, for example with sed:
#/# sed -i '/^#\/#.*/d' Makefile
#/# or using eclipse's Find/Replace
#/# Find:^#\/#.*\n  Replace With:[nix]  check Regular expressions
#/#
#/# gnu make's manual can be found here: https://www.gnu.org/software/make/manual/html_node/
#/#
#/# This package compiles into two archive files (.a; first.a, second.a), which are linked into a 
#/# shared object file (.so; liblibbridge.so) together with an API description.
#/# There are also two test executables. alltests.elf runs tests against the API of the shared object, whereas
#/# whitebox.elf calls the no-exported functions in the archives.

.SUFFIXES:  # Delete make's default suffix rules

#/# When make is called without parameters, it reads a file with name 'Makefile' and evaluates the first rule it finds.
#/# We call the target of this first rule 'default' and have it evaluate 'all', which is provided by ct-build.
#/# 'all' builds all MAIN_BUILDTARGETS; if the environment variable 'ENABLE_TESTS' ist set to 'T', 'all' also 
#/# builds TEST_BUILDTARGETS.
.PHONY: default
default: all

#######################################################################################################################
# Overridable project configuration

#/# Describes where relative to this make file the project structure is located.
#/# Required by ct-build.
PROJECT_ROOT ?= .

#/# The PTXdist name of this package in upper case.
#/# Required by ct-build.
PTXDIST_PACKAGE ?= LIBBRIDGE
#/# The path of the PTXdist make file.
#/# PTXDIST_PACKAGE_MK_FILE is not required by ct-build, it is included and some of the resolved content is printed 
#/# by build target 'info-txt'.
PTXDIST_PACKAGE_MK_FILE ?= $(PTXDIST_WORKSPACE)/rules/libbridge.make

#/# SRC_DIR is not used by ct-build, however the resolved content is printed by build target 'info-txt'.
SRC_DIR = $(PROJECT_ROOT)/src

#######################################################################################################################
# Optional package/ platform feature configuration

#/# Include common defines and macros for use in this make file.
-include $(SCRIPT_DIR)/platform_1.mk

#######################################################################################################################
# Custom toolchain configuration

#/# LINT_RULE_FILES is used to configure which rule set lint uses for checking the source code.
#/# Required by ct-build.
LINT_RULE_FILES = \
	$(LINT_CONFIG_DIR)/lint-rules/pfc.lnt

#######################################################################################################################
# Build target configuration

MAIN_BUILDTARGETS += \
	libbridge.a 

TEST_BUILDTARGETS += 

#/# Required by ct-build.
BUILDTARGETS += \
	$(MAIN_BUILDTARGETS) \
	$(TEST_BUILDTARGETS)

#/# Define a list of those artifacts that will be installed when the build target 'install' is requested (which will 
#/# also be done by PTXdist).
#/# Although it is possible to define the rule to install these targets in the PTXdist makefile, it is recommended 
#/# to define them in this Makefile, as according to GNU standard a makefile should have an install target. 
#/# Required by ct-build.
#/# DESTDIR: https://www.gnu.org/prep/standards/html_node/DESTDIR.html
#/# PTXdist will call 'install' with DESTDIR set to 'ptxproj.<XXX>/platform-<XXX>/packages'
INSTALL_TARGETS += \
	$(DESTDIR)/usr/lib/pkgconfig/libbridge.pc \
	$(DESTDIR)/usr/lib/libbridge.a \
	$(DESTDIR)/usr/include/libbridge/libbridge.h

#######################################################################################################################
# Settings shared between build targets

#/# The variables in this section are used only within this make file.
SHARED_CCXXFLAGS += -fPIC
#/# Hide all symbols of the referenced object files from the shared object's API, except for those marked for export.
#/# https://gcc.gnu.org/wiki/Visibility and https://www.akkadia.org/drepper/dsohowto.pdf
SHARED_CCXXFLAGS += -fvisibility=hidden
SHARED_CFLAGS    += $(call option_std,gnu99)
SHARED_CXXFLAGS  += $(call option_std,gnu++11)
SHARED_CXXFLAGS  += -fvisibility-inlines-hidden


#######################################################################################################################
# Settings for build target liblibbridge.a
libbridge.a_INCLUDES += \
-I$(PROJECT_ROOT)/extern \
-I$(PROJECT_ROOT)/src

libbridge.a_DISABLEDWARNINGS += packed sign-conversion conversion format vla stack-protector missing-prototypes missing-declarations suggest-attribute=pure suggest-attribute=const
libbridge.a_CXXDISABLEDWARNINGS += $(libbridge.a_DISABLEDWARNINGS) useless-cast abi-tag
libbridge.a_CDISABLEDWARNINGS += $(libbridge.a_DISABLEDWARNINGS)
libbridge.a_DEFINES +=
libbridge.a_LIBS +=
libbridge.a_CPPFLAGS += $(call pkg_config_cppflags,$(libbridge.a_PKG_CONFIGS))
libbridge.a_CPPFLAGS += $(libbridge.a_INCLUDES)
libbridge.a_CCXXFLAGS += $(SHARED_CCXXFLAGS)
libbridge.a_CFLAGS += $(call pkg_config_cflags,$(libbridge.a_PKG_CONFIGS))
libbridge.a_CFLAGS += $(call option_std,c99)
libbridge.a_CFLAGS += $(call option_disable_warning,$(libbridge.a_CDISABLEDWARNINGS))
libbridge.a_CFLAGS += $(libbridge.a_CCXXFLAGS)
libbridge.a_CFLAGS += $(SHARED_CFLAGS)
libbridge.a_CXXFLAGS += $(call pkg_config_cxxflags,$(libbridge.a_PKG_CONFIGS))
libbridge.a_CXXFLAGS += $(SHARED_CXXFLAGS)
libbridge.a_CXXFLAGS += $(call option_disable_warning,$(libbridge.a_CXXDISABLEDWARNINGS))
libbridge.a_CXXFLAGS += $(libbridge.a_CCXXFLAGS)
libbridge.a_SOURCES += $(call fglob_r,$(PROJECT_ROOT)/src,$(SOURCE_FILE_EXTENSIONS))
libbridge.a_DISABLE_CLANG_TIDY = T



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
#/# same folder. This location is also published by libbridge.pc.
#/# DESTDIR: https://www.gnu.org/prep/standards/html_node/DESTDIR.html
HDR_DIR=$(DESTDIR)/usr/include/libbridge

#/# This package provides a pkg-config file for use by other packages.
$(DESTDIR)/usr/lib/pkgconfig/%.pc: $(PROJECT_ROOT)/%.pc | $(DESTDIR)/usr/lib/pkgconfig
	cp $< $@

#/# Install binaries.
$(DESTDIR)/usr/bin/%: $(BIN_DIR)/%.elf | $(DESTDIR)/usr/bin
	cp $< $@

#/# Install API headers.
$(HDR_DIR)/%.h: $(PROJECT_ROOT)/extern/%.h | $(HDR_DIR)
	cp $< $@

#/# Create folder if it is missing.
$(HDR_DIR):
	mkdir -p $@

#######################################################################################################################
# Bootstrapping

#/# This rule complains if the folder pointed to by SCRIPT_DIR does not contain any .mk files.
$(SCRIPT_DIR)/%.mk:
	$(error build scripts unavailable ($@) - set script dir (SCRIPT_DIR=$(SCRIPT_DIR)))

