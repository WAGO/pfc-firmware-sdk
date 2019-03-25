# -*-makefile-*-
#
# Copyright (C) 2016 by Google
#
# See CREDITS for details about who has contributed to this project.
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
PACKAGES-$(PTXCONF_GOOGLETEST) += googletest

#
# Paths and names
#
GOOGLETEST_VERSION	:= 1.8.0
GOOGLETEST_MD5		:= 16877098823401d1bf2ed7891d7dce36
GOOGLETEST		:= googletest-$(GOOGLETEST_VERSION)
GOOGLETEST_SUFFIX	:= tar.gz
# This is the actual package URL. We renamed the archive file. To get file server mirror to work, a dummy URL with
# the changed file name is required.
#GOOGLETEST_URL		:= https://github.com/google/googletest/archive/release-$(GOOGLETEST_VERSION).$(GOOGLETEST_SUFFIX)
GOOGLETEST_URL		:= https://github.com/google/googletest/archive/googletest-$(GOOGLETEST_VERSION).$(GOOGLETEST_SUFFIX)
GOOGLETEST_SOURCE	:= $(SRCDIR)/$(GOOGLETEST).$(GOOGLETEST_SUFFIX)
GOOGLETEST_DIR		:= $(BUILDDIR)/$(GOOGLETEST)
GOOGLETEST_LICENSE	:= unknown

GOOGLETEST_BUILD_TYPE:=Debug

# ----------------------------------------------------------------------------
# Get
# ----------------------------------------------------------------------------

#$(GOOGLETEST_SOURCE):
#	@$(call targetinfo)
#	@$(call get, GOOGLETEST)

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

#GOOGLETEST_CONF_ENV	:= $(CROSS_ENV)

#
# cmake
#
GOOGLETEST_CONF_TOOL	:= cmake
GOOGLETEST_GCOV_FLAGS="-fprofile-arcs -ftest-coverage"
GOOGLETEST_CONF_OPT	= $(CROSS_CMAKE_USR)
GOOGLETEST_CONF_OPT += -DCMAKE_BUILD_TYPE="$(GOOGLETEST_BUILD_TYPE)"
GOOGLETEST_CONF_OPT += -DCMAKE_C_FLAGS=$(GOOGLETEST_GCOV_FLAGS)
GOOGLETEST_CONF_OPT += -DCMAKE_CXX_FLAGS=$(GOOGLETEST_GCOV_FLAGS)

#$(STATEDIR)/googletest.prepare:
#	@$(call targetinfo)
#	@$(call clean, $(GOOGLETEST_DIR)/config.cache)
#	cd $(GOOGLETEST_DIR) && \
#		$(GOOGLETEST_PATH) $(GOOGLETEST_ENV) \
#		./configure $(GOOGLETEST_CONF_OPT)
#	@$(call touch)

# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------

#$(STATEDIR)/googletest.compile:
#	@$(call targetinfo)
#	@$(call world/compile, GOOGLETEST)
#	@$(call touch)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

#$(STATEDIR)/googletest.install:
#	@$(call targetinfo)
#	@$(call world/install, GOOGLETEST)
#	@$(call touch)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/googletest.targetinstall:
	@$(call targetinfo)

# Packet contains no target artifacts.

	@$(call touch)

# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------

#$(STATEDIR)/googletest.clean:
#	@$(call targetinfo)
#	@$(call clean_pkg, GOOGLETEST)

# vim: syntax=make
