# -*-makefile-*-
#
# Copyright (C) 2019 by Maxim Laschinsky (maxim.laschinsky@wago.com), WAGO GmbH & Co. KG
#
# See CREDITS for details about who has contributed to this project.
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
PACKAGES-$(PTXCONF_LICENSE_INFO) += license-info

#
# Paths and names
#
LICENSE_INFO            := license-info
LICENSE_INFO_SRC_DIR    :=
LICENSE_INFO_TARGET_DIR :=
LICENSE_INFO_VERSION    := 1.0
LICENSE_INFO_LICENSE    :=
LICENSE_INFO_MAKE_ENV   :=

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

$(STATEDIR)/license-info.prepare:
	@$(call targetinfo)
	@$(call touch)

# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------

$(STATEDIR)/license-info.compile:
	@$(call targetinfo)
	@$(call touch)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

$(STATEDIR)/license-info.install:
	@$(call targetinfo)
	@$(call touch)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/license-info.targetinstall:
	@$(call targetinfo)

	@$(call install_init,  license-info)
	@$(call install_fixup, license-info, PRIORITY, optional)
	@$(call install_fixup, license-info, SECTION, base)
	@$(call install_fixup, license-info, AUTHOR,"Maxim Laschinsky, WAGO GmbH \& Co. KG")
	@$(call install_fixup, license-info, DESCRIPTION, missing)

  # copy license infos
	@$(call install_alternative, license-info, 0, 0, 0644, /etc/LICENSE);

	@$(call install_finish, license-info)
	@$(call touch)

# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------

$(STATEDIR)/license-info.clean:
	@$(call targetinfo)

# vim: syntax=make
