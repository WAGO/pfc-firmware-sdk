# -*-makefile-*-
#
# Copyright (C) 2017 by Henrik Lampe (henrik.lampe@wago.com), WAGO Kontakttechnik GmbH & Co. KG
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
LICENSES                := licenses
LICENSES_SRC_DIR				:= /usr/share/licenses
LICENSES_TARGET_DIR			:= /var/www/wbm/licenses
LICENSES_VERSION        := 1.0

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
	@$(call touch)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/licenses.targetinstall:
	@$(call targetinfo)

	@$(call install_init, licenses)
	@$(call install_fixup, licenses, PRIORITY, optional)
	@$(call install_fixup, licenses, SECTION, base)
	@$(call install_fixup, licenses, AUTHOR,"Henrik Lampe, WAGO Kontakttechnik GmbH \& Co. KG")
	@$(call install_fixup, licenses, DESCRIPTION, missing)

# do not run in BSP mode
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	@lic_path=$(PTXDIST_WORKSPACE)/projectroot.$(PTXCONF_PLATFORM)$(LICENSES_SRC_DIR)/oss
	@$(PTXDIST_WORKSPACE)/wago_intern/tools/create-license-infos/convert_dynamically.sh $(lic_path) 
endif
	
  # copy license files and create a link
	@$(call install_alternative_tree, licenses, 0, 0, $(LICENSES_SRC_DIR));
	@$(call install_link, licenses, $(LICENSES_SRC_DIR), $(LICENSES_TARGET_DIR));

	@$(call install_finish, licenses)
	@$(call touch)

# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------

# use ptxdist default

# vim: syntax=make
