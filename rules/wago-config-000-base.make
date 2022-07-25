#######################################################################################################################
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.
#
# This file is part of project parameter-service (PTXdist package wago-parameter-service).
#
# Copyright (c) 2020 WAGO GmbH & Co. KG
#
# Contributors:
#   PEn: WAGO GmbH & Co. KG
#######################################################################################################################
# Makefile for PTXdist package config-tool-base.
#
# For further information about the PTXdist project and license conditions
# see the PTXdist README file.
#

#
# We provide this package
#
PACKAGES-$(PTXCONF_CONFIG_TOOL_BASE) += config-tool-base

#
# Paths and names
#
CONFIG_TOOL_BASE_VERSION      := 2.0.0
CONFIG_TOOL_BASE              := config-tool-base
CONFIG_TOOL_BASE_URL          := file://$(PTXDIST_WORKSPACE)/local_src/$(CONFIG_TOOL_BASE)
CONFIG_TOOL_BASE_DIR          := $(BUILDDIR)/config-tools

# ----------------------------------------------------------------------------
# Extract
# ----------------------------------------------------------------------------

$(STATEDIR)/config-tool-base.extract:
	@$(call targetinfo)
	@$(call clean, $(CONFIG_TOOL_BASE_DIR))
	rsync -a --exclude=.libs/ --exclude=objs/ --exclude="*.o" --exclude="*.a" --exclude="*.so" --exclude="*.so" \
    $(PTXDIST_WORKSPACE)/local_src/config-tools $(BUILDDIR)
	@$(call touch)

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

$(STATEDIR)/config-tool-base.prepare:
	@$(call targetinfo)
	@$(call touch)

# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------

# config-tools use barebox' headers
$(STATEDIR)/config-tool-base.compile:
	@$(call targetinfo)
	@$(call touch)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

$(STATEDIR)/config-tool-base.install:
	@$(call targetinfo)
	@cp $(CONFIG_TOOL_BASE_DIR)/ct_error_handling.h      $(PTXCONF_SYSROOT_TARGET)/usr/include/ct_error_handling.h
	@$(call touch)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/config-tool-base.targetinstall:
	@$(call targetinfo)

	@$(call install_init, config-tool-base)
	@$(call install_fixup,config-tool-base,PRIORITY,optional)
	@$(call install_fixup,config-tool-base,SECTION,base)
	@$(call install_fixup,config-tool-base,AUTHOR,"WAGO")
	@$(call install_fixup,config-tool-base,DESCRIPTION,missing)

	@$(call install_finish,config-tool-base)
	@$(call touch)

# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------

$(STATEDIR)/config-tool-base.clean:
	@$(call targetinfo)

	rm -rf $(PTXCONF_SYSROOT_TARGET)/usr/include/ct_error_handling.h
	@$(call clean_pkg, CONFIG_TOOL_BASE)
	@rm -rf $(WAGO_PARAMETER_SERVICE_BUILDROOT_DIR)

# vim: syntax=make
