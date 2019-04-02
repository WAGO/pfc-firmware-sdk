# -*-makefile-*-
#
# Copyright (C) 2017 by <JB>
#
# See CREDITS for details about who has contributed to this project.
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
PACKAGES-$(PTXCONF_TOOLCHAIN_TESTS) += toolchain-tests

#
# Paths and names
#
TOOLCHAIN_TESTS_VERSION         := 0.0.1
TOOLCHAIN_TESTS_MD5             :=
TOOLCHAIN_TESTS                 := toolchain-tests
TOOLCHAIN_TESTS_URL             := file://wago_intern/$(TOOLCHAIN_TESTS)
TOOLCHAIN_TESTS_BUILDCONFIG     := Release
TOOLCHAIN_TESTS_SRC_DIR         := $(PTXDIST_WORKSPACE)/wago_intern/$(TOOLCHAIN_TESTS)
TOOLCHAIN_TESTS_BUILDROOT_DIR   := $(BUILDDIR)/$(TOOLCHAIN_TESTS)
TOOLCHAIN_TESTS_DIR             := $(TOOLCHAIN_TESTS_BUILDROOT_DIR)
TOOLCHAIN_TESTS_BUILD_DIR       := $(TOOLCHAIN_TESTS_BUILDROOT_DIR)/bin/$(TOOLCHAIN_TESTS_BUILDCONFIG)
TOOLCHAIN_TESTS_LICENSE         := unknown
TOOLCHAIN_TESTS_BIN             := $(TOOLCHAIN_TESTS).so.$(TOOLCHAIN_TESTS_VERSION)
TOOLCHAIN_TESTS_CONF_TOOL       := NO
TOOLCHAIN_TESTS_MAKE_ENV        := $(CROSS_ENV) \
BUILDCONFIG=$(TOOLCHAIN_TESTS_BUILDCONFIG) \
BIN_DIR=$(TOOLCHAIN_TESTS_BUILD_DIR) \
SCRIPT_DIR=$(PTXDIST_SYSROOT_HOST)/lib/ct-build


ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES

# ----------------------------------------------------------------------------
# Extract
# ----------------------------------------------------------------------------

$(STATEDIR)/toolchain-tests.extract:
	@$(call targetinfo)
	@$(call touch)

$(STATEDIR)/toolchain-tests.extract.post:
	@$(call targetinfo)
	@$(call touch)

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

$(STATEDIR)/toolchain-tests.prepare:
	@$(call targetinfo)
	@$(call touch)

# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------

$(STATEDIR)/toolchain-tests.compile:
	@$(call targetinfo)
	@$(call touch)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

$(STATEDIR)/toolchain-tests.install:
	@$(call targetinfo)
	@$(call touch)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/toolchain-tests.targetinstall:
	@$(call targetinfo)
	@$(call touch)


# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------

$(STATEDIR)/toolchain-tests.clean:
endif

# vim: syntax=make
