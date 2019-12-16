#######################################################################################################################
# Makefile for PTXdist package @name@
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
#/# shared object file (.so; lib@name@.so) together with an API description.
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
PTXDIST_PACKAGE ?= @NAME@
#/# The path of the PTXdist make file.
#/# PTXDIST_PACKAGE_MK_FILE is not required by ct-build, it is included and some of the resolved content is printed 
#/# by build target 'info-txt'.
PTXDIST_PACKAGE_MK_FILE ?= $(PTXDIST_WORKSPACE)/rules/@name@.make

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

#/# MAIN_BUILDTARGETS defines which artefacts are subject to be built upon a request for target 'all'.
#/# Required by ct-build.
#/# Shared libraries' names always have to start with 'lib' as required by the gcc linker.
#/# This requirement also applies to static libraries (.a).
MAIN_BUILDTARGETS += \
	@name@.elf \
	lib@name@.so \
	libfirst.a \
	libsecond.a

#/# TEST_BUILDTARGETS defines which artefacts are subject to be built and executed upon a request for target 'check'.
#/# If the environment variable 'ENABLE_TESTS' is defined ('T'), these targets are also built upon 'all'.
#/# Ihe the environment variable 'ENABLE_CHECK_DURING_BUILD' is set, the tests are even run upon 'all'.
#/# Required by ct-build.
TEST_BUILDTARGETS += \
	alltests.elf \
	whitebox.elf

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
	$(DESTDIR)/usr/lib/pkgconfig/@name@.pc \
	$(DESTDIR)/usr/lib/lib@name@.so.$(@NAME@_VERSION) \
	$(DESTDIR)/usr/bin/@name@ \
	$(DESTDIR)/usr/include/@name@/@name@_API.h

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


#/# Find the names of those artefacts in a dependency list which are generated by this makefile.
# $(call local_prerequisites,name)
local_prerequisites = $(call lib_buildtarget_raw,$($(1)_LIBS) $($(1)_PKG_CONFIG_LIBS),$($(1)_STATICALLYLINKED))


#######################################################################################################################
# Settings for build target lib@name@.so

#/# For readability's sake, each line should add only one item to a variable.
#/#
#/# Define the version of the created library. Used to build the complete name of the .so.
#/# VERSION is required by ct-build.
lib@name@.so_VERSION          = $(@NAME@_VERSION)
#/# LIBS defines the list of libraries used by an artefact.
#/# The order in which librarbies are provided is important; the linker searches and processes libraries and 
#/# object files in the order they are specified. Thus, ‘foo.o -lz bar.o’ searches library ‘z’ after file foo.o 
#/# but before bar.o. If bar.o refers to functions in ‘z’, those functions may not be loaded. 
#/# For more details, cf. https://gcc.gnu.org/onlinedocs/gcc/Link-Options.html
#/# LIBS is not used by ct-build, however the resolved content is printed by build target 'info-txt'.
lib@name@.so_LIBS             += first second
#/# STATICALLYLINKED defines those libraries in LIBS that will be linked statically.
#/# STATICALLYLINKED is not used by ct-build, however the resolved content is printed by build target 'info-txt'.
lib@name@.so_STATICALLYLINKED += first second
#/# The shared object does not introduce its own dependencies but reuses the dependencies of its contributing libraries.
#/# PKG_CONFIGS is not used by ct-build, however the resolved content is printed by build target 'info-txt'.
lib@name@.so_PKG_CONFIGS      += $(libfirst.a_PKG_CONFIGS)
lib@name@.so_PKG_CONFIGS      += $(libsecond.a_PKG_CONFIGS)
#/# ct-build provides a mechanism to explicitly disable compiler warnings without having to know the specific compiler syntax.
#/# The list of compiler warnings enabled by default can be found in ct-build/internal/gcc_<version>_warnings.switch.
#/# DISABLEDWARNINGS is not used by ct-build, however the resolved content is printed by build target 'info-txt'.
lib@name@.so_DISABLEDWARNINGS +=
#/# PREREQUISITES describes which artefacts should be present before the artefact at hand itself is built.
#/# It usually contains libraries or generated source code generated by the same project.
#/# PREREQUISITES is required by ct-build.
lib@name@.so_PREREQUISITES    += $(call local_prerequisites,lib@name@.so)
#/# Add all files with an extension of [c,cc,cpp] in $(SRC_DIR)/ to source file list.
#/# The source code of the shared object does only contain interface functions which provide the API.
#/# The compiler option '-fvisibility=hidden', which is contained in SHARED_CCXXFLAGS, tells the linker to hide all of
#/# the module's symbols from access by default.
#/# SOURCES is required by ct-build. It determines which files are to be compiled and further which objects will be linked together.
lib@name@.so_SOURCES          += $(call glob_r,$(addprefix $(SRC_DIR)/*.,$(SOURCE_FILE_EXTENSIONS)))
#/# The function pkg_config_cppflags extracts all flags needed for the preprocessor from a given list of pkg-config files.
#/# CPPFLAGS is required by ct-build. It contains the flags which are passed to the compiler's preprocessor.
lib@name@.so_CPPFLAGS         += $(call pkg_config_cppflags,$(lib@name@.so_PKG_CONFIGS))
#/# Tell compiler and linker that the functions declared with this macro should not be removed from the .so API.
#/# See also the file @name@_API.h
lib@name@.so_CPPFLAGS         += -D'@NAME@_API=WC_API_EXPORT'
#/# CCXXFLAGS is not used by ct-build, only within this makefile 
#/# It contains the flags which are passed to the compiler for C and C++ sources.
lib@name@.so_CCXXFLAGS        += $(SHARED_CCXXFLAGS)
lib@name@.so_CCXXFLAGS        += $(call option_disable_warning,$(lib@name@.so_DISABLEDWARNINGS))
#/# CFLAGS is required by ct-build. It contains the flags which are passed to the compiler for C sources.
#/# Compiler and linker flags enabled by default can be found in ct-build/internal/gcc_<version>.mk.
lib@name@.so_CFLAGS           += $(SHARED_CFLAGS)
lib@name@.so_CFLAGS           += $(lib@name@.so_CCXXFLAGS)
#/# The function pkg_config_cflags extracts all flags needed for the C compiler from a given list of pkg-config files.
lib@name@.so_CFLAGS           += $(call pkg_config_cflags,$(lib@name@.so_PKG_CONFIGS))
#/# CXXFLAGS is required by ct-build. It contains the flags which are passed to the compiler for C++ sources.
lib@name@.so_CXXFLAGS         += $(SHARED_CXXFLAGS)
lib@name@.so_CXXFLAGS         += $(lib@name@.so_CCXXFLAGS)
#/# The function pkg_config_cxxflags extracts all flags needed for the C++ compiler from a given list of pkg-config files.
lib@name@.so_CXXFLAGS         += $(call pkg_config_cxxflags,$(lib@name@.so_PKG_CONFIGS))
#/# LDFLAGS is required by ct-build. It contains the flags which are passed to the linker for C++ sources.
lib@name@.so_LDFLAGS          += $(call option_lib,$(lib@name@.so_LIBS) $(lib@name@.so_PKG_CONFIG_LIBS),lib@name@.so)
#/# The function pkg_config_ldflags extracts all flags needed for the linker from a given list of pkg-config files.
lib@name@.so_LDFLAGS          += $(call pkg_config_ldflags,$(lib@name@.so_PKG_CONFIGS))

#/# The resolution of all these variables can be hard to predict. Therefore there is a target "info-txt", which
#/# prints the contents of the project's variables to screen. Give it a try.
#/# If you want to inspect variables not listed, the "print-%" make target may came handy.
#/# for example ./make-wrapper print-SHARED_CXXFLAGS


#######################################################################################################################
# Settings for build target libfirst.a

libfirst.a_LIBS             += 
libfirst.a_STATICALLYLINKED +=
libfirst.a_PKG_CONFIGS      += 
libfirst.a_DISABLEDWARNINGS += 
libfirst.a_PREREQUISITES    += $(call local_prerequisites,libfirst.a)
#/# Add all files with an extension of [c,cc,cpp] in the folder 'first/' or any subfolder (**) to source file list.
#/# The function glob_r resolves to the names of all files matching the passed globbing expression.
#/# See also https://www.codeprocess.io/recursively-walk-a-directory-hierarchy-with-bash-globstar/
libfirst.a_SOURCES          += $(call glob_r,$(addprefix $(SRC_DIR)/first/**/*.,$(SOURCE_FILE_EXTENSIONS)))
libfirst.a_CPPFLAGS         += $(call pkg_config_cppflags,$(libfirst.a_PKG_CONFIGS))
libfirst.a_CCXXFLAGS        += $(SHARED_CCXXFLAGS)
libfirst.a_CFLAGS           += $(SHARED_CFLAGS)
libfirst.a_CFLAGS           += $(libfirst.a_CCXXFLAGS)
libfirst.a_CFLAGS           += $(call pkg_config_cflags,$(libfirst.a_PKG_CONFIGS))
libfirst.a_CXXFLAGS         += $(SHARED_CXXFLAGS)
libfirst.a_CXXFLAGS         += $(libfirst.a_CCXXFLAGS)
libfirst.a_CXXFLAGS         += $(call pkg_config_cxxflags,$(libfirst.a_PKG_CONFIGS))
libfirst.a_LDFLAGS          += $(call option_lib,$(libfirst.a_LIBS),libfirst)
libfirst.a_LDFLAGS          += $(call pkg_config_ldflags,$(libfirst.a_PKG_CONFIGS))


#######################################################################################################################
# Settings for build target libsecond.a

libsecond.a_LIBS             += 
libsecond.a_STATICALLYLINKED += 
libsecond.a_PKG_CONFIGS      += 
libsecond.a_DISABLEDWARNINGS += 
libsecond.a_PREREQUISITES    += $(call local_prerequisites,libsecond.a_LIBS)
#/# Add all files with an extension of [c,cc,cpp] in the folder 'second/' or any subfolder (**) to source file list.
libsecond.a_SOURCES          += $(call glob_r,$(addprefix $(SRC_DIR)/second/**/*.,$(SOURCE_FILE_EXTENSIONS)))
libsecond.a_CPPFLAGS         += $(call pkg_config_cppflags,$(libsecond.a_PKG_CONFIGS))
libsecond.a_CCXXFLAGS        += $(SHARED_CCXXFLAGS)
libsecond.a_CFLAGS           += $(SHARED_CFLAGS)
libsecond.a_CFLAGS           += $(libsecond.a_CCXXFLAGS)
libsecond.a_CFLAGS           += $(call pkg_config_cflags,$(libsecond.a_PKG_CONFIGS))
libsecond.a_CXXFLAGS         += $(SHARED_CXXFLAGS)
libsecond.a_CXXFLAGS         += $(libsecond.a_CCXXFLAGS)
libsecond.a_CXXFLAGS         += $(call pkg_config_cxxflags,$(libsecond.a_PKG_CONFIGS))
libsecond.a_LDFLAGS          += $(call option_lib,$(libsecond.a_LIBS),libsecond)
libsecond.a_LDFLAGS          += $(call pkg_config_ldflags,$(libsecond.a_PKG_CONFIGS))


#######################################################################################################################
# Settings for build target alltests.elf

#/# alltests.elf contains only blackbox tests, which use solely the .so's public interface.
alltests.elf_LIBS             += @name@ gmock_main gmock
alltests.elf_STATICALLYLINKED += gmock_main gmock
alltests.elf_PKG_CONFIGS      += 
alltests.elf_DISABLEDWARNINGS += 
alltests.elf_PREREQUISITES    += $(call local_prerequisites,alltests.elf)
alltests.elf_SOURCES          += $(call glob_r,$(addprefix $(PROJECT_ROOT)/test-src/blackbox/**/*.,$(SOURCE_FILE_EXTENSIONS)))
#/# Add name of the folder containing the API headers to the include search path.
alltests.elf_CPPFLAGS         += -I$(SRC_DIR)
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
# Settings for build target whitebox.elf

#/# whitebox.elf contains only whitebox tests
whitebox.elf_LIBS             += gmock_main gmock first second
whitebox.elf_STATICALLYLINKED += gmock_main gmock first second
whitebox.elf_PKG_CONFIGS      += 
#/# Reuse the pkg-config dependencies used for lib@name@.so.
#/# alltests.elf does not have to declare these dependencies, as the are already provided by @name@.so.
whitebox.elf_PKG_CONFIGS      += $(lib@name@.so_PKG_CONFIGS)
whitebox.elf_DISABLEDWARNINGS += 
whitebox.elf_PREREQUISITES    += $(call local_prerequisites,whitebox.elf)
whitebox.elf_SOURCES          += $(call glob_r,$(addprefix $(PROJECT_ROOT)/test-src/whitebox/**/*.,$(SOURCE_FILE_EXTENSIONS)))
#/# Add name of the folder containing the API headers to the include search path.
whitebox.elf_CPPFLAGS         += -I$(SRC_DIR)
whitebox.elf_CPPFLAGS         += $(call pkg_config_cppflags,$(whitebox.elf_PKG_CONFIGS))
whitebox.elf_CCXXFLAGS        += $(SHARED_CCXXFLAGS)
whitebox.elf_CCXXFLAGS        += $(call option_disable_warning,$(whitebox.elf_DISABLEDWARNINGS))
whitebox.elf_CFLAGS           += $(SHARED_CFLAGS)
whitebox.elf_CFLAGS           += $(whitebox.elf_CCXXFLAGS)
whitebox.elf_CFLAGS           += $(call pkg_config_cflags,$(whitebox.elf_PKG_CONFIGS))
whitebox.elf_CXXFLAGS         += $(SHARED_CXXFLAGS)
whitebox.elf_CXXFLAGS         += $(whitebox.elf_CCXXFLAGS)
whitebox.elf_CXXFLAGS         += $(call pkg_config_cxxflags,$(whitebox.elf_PKG_CONFIGS))
whitebox.elf_LDFLAGS          += $(call option_lib,$(whitebox.elf_LIBS),whitebox.elf)
whitebox.elf_LDFLAGS          += $(call pkg_config_ldflags,$(whitebox.elf_PKG_CONFIGS))
whitebox.elf_CLANG_TIDY_CHECKS += $(GTEST_CLANG_TIDY_CHECKS)


#######################################################################################################################
# Settings for build target @name@.elf

@name@.elf_LIBS             += @name@ 
@name@.elf_STATICALLYLINKED += 
@name@.elf_PKG_CONFIGS      += 
@name@.elf_DISABLEDWARNINGS += 
@name@.elf_PREREQUISITES    += $(call local_prerequisites,@name@.elf)
@name@.elf_SOURCES          += $(call glob_r,$(addprefix $(PROJECT_ROOT)/src/@name@/**/*.,$(SOURCE_FILE_EXTENSIONS)))
#/# Add name of the folder containing the API headers to the include search path.
@name@.elf_CPPFLAGS         += -I$(SRC_DIR)
@name@.elf_CPPFLAGS         += $(call pkg_config_cppflags,$(@name@.elf_PKG_CONFIGS))
@name@.elf_CCXXFLAGS        += $(SHARED_CCXXFLAGS)
@name@.elf_CCXXFLAGS        += $(call option_disable_warning,$(@name@.elf_DISABLEDWARNINGS))
@name@.elf_CFLAGS           += $(SHARED_CFLAGS)
@name@.elf_CFLAGS           += $(@name@.elf_CCXXFLAGS)
@name@.elf_CFLAGS           += $(call pkg_config_cflags,$(@name@.elf_PKG_CONFIGS))
@name@.elf_CXXFLAGS         += $(SHARED_CXXFLAGS)
@name@.elf_CXXFLAGS         += $(@name@.elf_CCXXFLAGS)
@name@.elf_CXXFLAGS         += $(call pkg_config_cxxflags,$(@name@.elf_PKG_CONFIGS))
@name@.elf_LDFLAGS          += $(call option_lib,$(@name@.elf_LIBS),@name@.elf)
@name@.elf_LDFLAGS          += $(call pkg_config_ldflags,$(@name@.elf_PKG_CONFIGS))


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
#/# same folder. This location is also published by @name@.pc.
#/# DESTDIR: https://www.gnu.org/prep/standards/html_node/DESTDIR.html
HDR_DIR=$(DESTDIR)/usr/include/@name@

#/# This package provides a pkg-config file for use by other packages.
$(DESTDIR)/usr/lib/pkgconfig/%.pc: $(PROJECT_ROOT)/%.pc | $(DESTDIR)/usr/lib/pkgconfig
	cp $< $@

#/# Install binaries.
$(DESTDIR)/usr/bin/%: $(BIN_DIR)/%.elf | $(DESTDIR)/usr/bin
	cp $< $@

#/# Install API headers.
$(HDR_DIR)/%.h: $(PROJECT_ROOT)/src/%.h | $(HDR_DIR)
	cp $< $@

#/# Create folder if it is missing.
$(HDR_DIR):
	mkdir -p $@

#######################################################################################################################
# Bootstrapping

#/# This rule complains if the folder pointed to by SCRIPT_DIR does not contain any .mk files.
$(SCRIPT_DIR)/%.mk:
	$(error build scripts unavailable ($@) - set script dir (SCRIPT_DIR=$(SCRIPT_DIR)))

