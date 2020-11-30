#######################################################################################################################
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.
#
# This file is part of project fwupdate (PTXdist package rauc_bundle_conf).
#
# Copyright (c) 2018
#
# Contributors:
#   PEn: WAGO Kontakttechnik GmbH & Co. KG
#######################################################################################################################
# Makefile for PTXdist package rauc_bundle_conf.
#
# For further information about the PTXdist project and license conditions
# see the PTXdist README file.
#

#
# We provide this package
#
PACKAGES-$(PTXCONF_RAUC_BUNDLE_CONF) += rauc_bundle_conf

#
# Paths and names
#
RAUC_BUNDLE_CONF_VERSION     := 1.2
RAUC_BUNDLE_CONF_MD5         :=
RAUC_BUNDLE_CONF             := rauc_bundle_conf-$(RAUC_BUNDLE_CONF_VERSION)
RAUC_BUNDLE_CONF_URL         := file://shared_public/fwupdate/rauc_bundle_conf
RAUC_BUNDLE_CONF_SRC_DIR     := $(WAGO_FWUPDATE_SRC_DIR)/rauc_bundle_conf
RAUC_BUNDLE_CONF_DIR         := $(BUILDDIR)/rauc_bundle_conf
RAUC_BUNDLE_CONF_TARGET      := $(RAUC_BUNDLE_CONF_DIR)/rauc_bundle_conf.tar.gz
RAUC_BUNDLE_CONF_OBSOLETE_TARGET := $(RAUC_BUNDLE_CONF_SRC_DIR)/rauc_bundle_conf.tar.gz
RAUC_BUNDLE_CONF_FILES       := update
WAGO_FWUPDATE_LICENSE        := MPL-2.0

# ----------------------------------------------------------------------------
# Extract
# ----------------------------------------------------------------------------

$(STATEDIR)/rauc_bundle_conf.extract:
	@$(call targetinfo)
	@$(call clean, $(RAUC_BUNDLE_CONF_DIR))
	@mkdir -p $(RAUC_BUNDLE_CONF_DIR)
	@rsync -a --exclude=".project" --exclude=".cproject"  $(RAUC_BUNDLE_CONF_SRC_DIR) $(BUILDDIR)
	@$(call patchin, RAUC_BUNDLE_CONF)
	@$(call touch)

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

#RAUC_BUNDLE_CONF_PATH	:= PATH=$(CROSS_PATH)
RAUC_BUNDLE_CONF_CONF_TOOL	:= NO
RAUC_BUNDLE_CONF_MAKE_ENV	:= $(CROSS_ENV)

#$(STATEDIR)/rauc_bundle_conf.prepare:
#	@$(call targetinfo)
#	@$(call world/prepare, RAUC_BUNDLE_CONF)
#	@$(call touch)

# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------

$(STATEDIR)/rauc_bundle_conf.compile: 
	@$(call targetinfo)
	@cd $(RAUC_BUNDLE_CONF_DIR) && \
		tar cfz $(RAUC_BUNDLE_CONF_TARGET) -C $(RAUC_BUNDLE_CONF_FILES) .
	@$(call touch)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

$(STATEDIR)/rauc_bundle_conf.install:
	@$(call targetinfo)
	@$(call touch)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/rauc_bundle_conf.targetinstall:
	@$(call targetinfo)

	@$(call install_init, rauc_bundle_conf)
	@$(call install_fixup, rauc_bundle_conf,PRIORITY,optional)
	@$(call install_fixup, rauc_bundle_conf,SECTION,base)
	@$(call install_fixup, rauc_bundle_conf,AUTHOR,"<PEn>")
	@$(call install_fixup, rauc_bundle_conf,DESCRIPTION,missing)

	@$(call install_finish, rauc_bundle_conf)

	@$(call touch)

# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------

$(STATEDIR)/rauc_bundle_conf.clean:
	@$(call targetinfo)
	@rm -rf $(RAUC_BUNDLE_CONF_OBSOLETE_TARGET)
	@$(call clean_pkg, RAUC_BUNDLE_CONF)

# vim: syntax=make
