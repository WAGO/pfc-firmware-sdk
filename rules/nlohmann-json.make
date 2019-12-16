#######################################################################################################################
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.
#
# This file is part of PTXdist package nlohmann-json.
#
# Copyright (c) 2019 WAGO Kontakttechnik GmbH & Co. KG
#
# Contributors:
#   PEn: WAGO Kontakttechnik GmbH & Co. KG
#######################################################################################################################
# Makefile for PTXdist package nlohmann-json.
#
# For further information about the PTXdist project and license conditions
# see the PTXdist README file.
#

#
# We provide this package
#
PACKAGES-$(PTXCONF_NLOHMANN_JSON) += nlohmann-json

#
# Paths and names
#
NLOHMANN_JSON_VERSION      := 3.7.0
NLOHMANN_JSON_MD5          := 4755f892ee7770ecee31296b29d43fef
NLOHMANN_JSON              := nlohmann-json-v$(NLOHMANN_JSON_VERSION)
NLOHMANN_JSON_SUFFIX       := tar.gz
NLOHMANN_JSON_URL          := https://github.com/nlohmann/json/archive/v$(NLOHMANN_JSON_VERSION).$(NLOHMANN_JSON_SUFFIX)
NLOHMANN_JSON_SOURCE       := $(SRCDIR)/$(NLOHMANN_JSON).$(NLOHMANN_JSON_SUFFIX)
NLOHMANN_JSON_DIR          := $(BUILDDIR)/$(NLOHMANN_JSON)
NLOHMANN_JSON_LICENSE      := MIT
NLOHMANN_JSON_LICENSE_FILE := LICENSE.MIT

# ----------------------------------------------------------------------------
# Get
# ----------------------------------------------------------------------------

#$(STATEDIR)/nlohmann-json.get:
#	@$(call targetinfo)
#	@$(call world/get, NLOHMANN_JSON)
#	@$(call touch)

# ----------------------------------------------------------------------------
# Extract
# ----------------------------------------------------------------------------

#$(STATEDIR)/nlohmann-json.extract:
#	@$(call targetinfo)
#	@$(call world/extract, NLOHMANN_JSON)
#	@$(call touch)

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

#NLOHMANN_JSON_PATH      := PATH=$(CROSS_PATH)
NLOHMANN_JSON_CONF_TOOL  := cmake
NLOHMANN_JSON_CONF_OPT   := $(CROSS_CMAKE_USR) -D JSON_BuildTests:BOOL=OFF -D JSON_MultipleHeaders:BOOL=ON
NLOHMANN_JSON_MAKE_ENV   := $(CROSS_ENV)

#$(STATEDIR)/nlohmann-json.prepare:
#	@$(call targetinfo)
#	@$(call world/prepare, nlohmann-json)
#	@$(call touch)

# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------

#$(STATEDIR)/nlohmann-json.compile:
#	@$(call targetinfo)
#	@$(call world/compile, nlohmann-json)
#	@$(call touch)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

#$(STATEDIR)/nlohmann-json.install:
#	@$(call targetinfo)
#	@$(call world/install, nlohmann-json)
#	@$(call touch)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/nlohmann-json.targetinstall:
	@$(call targetinfo)
	@$(call world/targetinstall, nlohmann-json)
	@$(call install_init, nlohmann-json)
	@$(call install_fixup, nlohmann-json,PRIORITY,optional)
	@$(call install_fixup, nlohmann-json,SECTION,base)
	@$(call install_fixup, nlohmann-json,AUTHOR,"Niels Lohmann <mail@nlohmann.me>.")
	@$(call install_fixup, nlohmann-json,DESCRIPTION,missing)

	@$(call install_copy, nlohmann-json, 0, 0, 0644, $(NLOHMANN_JSON_DIR)/$(NLOHMANN_JSON_LICENSE_FILE), /usr/share/licenses/oss/license.nlohmann-json_$(NLOHMANN_JSON_VERSION).txt)
	@$(call install_finish, nlohmann-json)
	@$(call touch)

# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------

$(STATEDIR)/nlohmann-json.clean:
	@$(call targetinfo)
	@$(call clean_pkg, nlohmann-json)

# vim: syntax=make
