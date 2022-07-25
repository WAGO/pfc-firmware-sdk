# -*-makefile-*-
#
# Copyright (C) 2017 by Henrik Lampe (henrik.lampe@wago.com), WAGO GmbH & Co. KG
#
# See CREDITS for details about who has contributed to this project.
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
PACKAGES-$(PTXCONF_LICENSES) += licenses

#
# Paths and names
#
LICENSES		:= licenses
LICENSES_TARGET_DIR	:= /usr/share/licenses
LICENSES_WBM_DIR	:= /var/www/wbm/licenses
LICENSES_PATH		:= $(PTXDIST_WORKSPACE)/projectroot$(LICENSES_TARGET_DIR)/oss
LICENSES_DIR		:= $(BUILDDIR)/$(LICENSES)
LICENSES_VERSION        := 1.0
LICENSES_PACKAGE_NAME 	:= $(LICENSES)_$(LICENSES_VERSION)_$(PTXDIST_IPKG_ARCH_STRING)
LICENSES_PLATFORMCONFIGPACKAGEDIR	:= $(PTXDIST_PLATFORMCONFIGDIR)/packages

LICENSES_LICENSE        := unknown
LICENSES_MAKE_ENV       :=

# ----------------------------------------------------------------------------
# Get
# ----------------------------------------------------------------------------

# use ptxdist default

# ----------------------------------------------------------------------------
# Extract
# ----------------------------------------------------------------------------

# use ptxdist default

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

$(STATEDIR)/licenses.prepare:
	@$(call targetinfo)
	@$(call touch)

# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------

$(STATEDIR)/licenses.compile:
	@$(call targetinfo)
	@$(call touch)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

$(STATEDIR)/licenses.install:
	@$(call targetinfo)
	
ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
#	# BSP mode: install by extracting tgz file
	@mkdir -p $(LICENSES_DIR) && \
	  tar xvzf $(LICENSES_PLATFORMCONFIGPACKAGEDIR)/$(LICENSES_PACKAGE_NAME).tgz \
	    -C $(LICENSES_DIR)
else
	@$(shell mkdir -p $(LICENSES_DIR))
#	# copy only needed licences, i.e. for selected packages, to working directory
#	# and then work with the copies. do NOT modify them directly in */projectroot*/...
	@$(foreach pkg, $(IMAGE_ROOT_TGZ_PKGS), \
	  $(shell test -f $(LICENSES_PATH)/license.$(subst _,-,$(pkg))_*.txt && \
	  cp $(LICENSES_PATH)/license.$(subst _,-,$(pkg))_*.txt $(LICENSES_DIR)/) \
	  $(shell [[ "$(pkg)" == "udev-legacy" ]] && \
	  cp $(LICENSES_PATH)/license.udev_*.txt $(LICENSES_DIR)/) \
	)

#	# WBM can only show UTF-8 format, so test and convert licenses
	@$(PTXDIST_WORKSPACE)/wago_intern/tools/create-license-infos/convert_dynamically.sh \
	  $(LICENSES_DIR)
#	# license files need a lot of space, so package them and copy to build-target
	@$(PTXDIST_WORKSPACE)/wago_intern/tools/create-license-infos/pack_licenses.sh \
	  $(LICENSES_DIR)

ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_RELEASE
#	# save install directory to tgz for BSP mode
	@mkdir -p $(LICENSES_PLATFORMCONFIGPACKAGEDIR)
	@cd $(LICENSES_DIR) && \
	  tar cvzf $(LICENSES_PLATFORMCONFIGPACKAGEDIR)/$(LICENSES_PACKAGE_NAME).tgz *.xz
endif # PTXCONF_WAGO_TOOLS_BUILD_VERSION_RELEASE
endif # PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES

	@$(call touch)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/licenses.targetinstall:
	@$(call targetinfo)

	@$(call install_init, licenses)
	@$(call install_fixup, licenses, PRIORITY, optional)
	@$(call install_fixup, licenses, SECTION, base)
	@$(call install_fixup, licenses, AUTHOR,"Henrik Lampe, WAGO GmbH \& Co. KG")
	@$(call install_fixup, licenses, DESCRIPTION, missing)

#	# install general licenses
	@$(call install_alternative, licenses, 0, 0, 0644, $(LICENSES_TARGET_DIR)/oss.txt)
	@$(call install_alternative, licenses, 0, 0, 0644, $(LICENSES_TARGET_DIR)/wago.txt)
	@$(call install_alternative, licenses, 0, 0, 0644, $(LICENSES_TARGET_DIR)/trademarks.txt)

#	# _generic_ licenses, i.e. do not depend on selected packages - GPL etc.
	@$(call install_alternative_tree, licenses, 0, 0, $(LICENSES_TARGET_DIR)/oss/generic/)

#	# OSS licenses depending on selected(!) and INSTALLED packages
	@$(call install_glob, licenses, 0, 0, $(LICENSES_DIR), $(LICENSES_TARGET_DIR)/oss/, *.xz, *.txt)

#	# install link to target dir inside WBM dir
	@$(call install_link, licenses, $(LICENSES_TARGET_DIR), $(LICENSES_WBM_DIR))

	@$(call install_finish, licenses)
	@$(call touch)

# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------

# use ptxdist default

# vim: syntax=make
