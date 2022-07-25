#######################################################################################################################
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.
#
# This file is part of PTXdist package nlohmann-json.
#
# Copyright (c) 2019 WAGO GmbH & Co. KG
#######################################################################################################################
# Makefile for PTXdist package jsoncpp.
#
# For further information about the PTXdist project and licende conditions
# see the PTXdist README file.
#

#
# We provide this package
#
PACKAGES-$(PTXCONF_JSONCPP) += jsoncpp

#
# Paths and names
#
JSONCPP_VERSION      := 1.9.3
JSONCPP_MD5          := 288bedd396758709e517fcc4acad6ac2
JSONCPP              := jsoncpp-$(JSONCPP_VERSION)
JSONCPP_SUFFIX       := tar.gz
#JSONCPP_URL          := https://github.com/open-source-parsers/jsoncpp/archive/$(JSONCPP_VERSION).$(JSONCPP_SUFFIX)
JSONCPP_URL          := https://github.com/open-source-parsers/jsoncpp/archive/jsoncpp-$(JSONCPP_VERSION).$(JSONCPP_SUFFIX)
JSONCPP_SOURCE       := $(SRCDIR)/$(JSONCPP).$(JSONCPP_SUFFIX)
JSONCPP_DIR          := $(BUILDDIR)/$(JSONCPP)
JSONCPP_LICENSE      := MIT
JSONCPP_LICENSE_FILE := LICENSE

# ----------------------------------------------------------------------------
# Get
# ----------------------------------------------------------------------------

#$(STATEDIR)/jsoncpp.get:
#	@$(call targetinfo)
#	@$(call world/get, jsoncpp)
#	@$(call touch)

# ----------------------------------------------------------------------------
# Extract
# ----------------------------------------------------------------------------

#$(STATEDIR)/jsoncpp.extract:
#	@$(call targetinfo)
#	@$(call world/extract, jsoncpp)
#	@$(call touch)

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

JSONCPP_CONF_TOOL  := cmake
JSONCPP_CONF_OPT   := $(CROSS_CMAKE_USR) -D JSONCPP_WITH_TESTS:BOOL=OFF
JSONCPP_MAKE_ENV   := $(CROSS_ENV)

#$(STATEDIR)/jsoncpp.prepare:
#	@$(call targetinfo)
#	@$(call world/prepare, jsoncpp)
#	@$(call touch)

# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------

#$(STATEDIR)/jsoncpp.compile:
#	@$(call targetinfo)
#	@$(call world/compile, jsoncpp)
#	@$(call touch)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

#$(STATEDIR)/jsoncpp.install:
#	@$(call targetinfo)
#	@$(call world/install, jsoncpp)
#	@$(call touch)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/jsoncpp.targetinstall:
	@$(call targetinfo)
	@$(call world/targetinstall, jsoncpp)
	@$(call install_init, jsoncpp)
	@$(call install_fixup, jsoncpp,PRIORITY,optional)
	@$(call install_fixup, jsoncpp,SECTION,base)
	@$(call install_fixup, jsoncpp,AUTHOR,"Baptiste Lepilleur <blep@users.sourceforge.net>")
	@$(call install_fixup, jsoncpp,DESCRIPTION,"A C++ library for interacting with JSON")

	@$(call install_copy, jsoncpp, 0, 0, 0644, $(JSONCPP_DIR)/$(JSONCPP_LICENSE_FILE), /usr/share/licenses/oss/license.jsoncpp_$(JSONCPP_VERSION).txt)
	@$(call install_finish, jsoncpp)
	@$(call touch)

# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------

$(STATEDIR)/jsoncpp.clean:
	@$(call targetinfo)
	@$(call clean_pkg, jsoncpp)

# vim: syntax=make
