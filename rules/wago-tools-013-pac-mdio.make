# -*-makefile-*-
#
# Copyright (C) 2013 by Heinrich Toews <heinrich.toews@wago.com>
#
# See CREDITS for details about who has contributed to this project.
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
PACKAGES-$(PTXCONF_PAC_MDIO) += pac-mdio

#
# Paths and names
#
PAC_MDIO_VERSION	:= 1.1
PAC_MDIO		:= pac-mdio-$(PAC_MDIO_VERSION)
PAC_MDIO_SUFFIX	:= tar.gz
PAC_MDIO_URL		:= http://svsv01003.wago.local/wago-ptxdist-src/$(PAC_MDIO).$(PAC_MDIO_SUFFIX)
PAC_MDIO_SOURCE	:= $(SRCDIR)/$(PAC_MDIO).$(PAC_MDIO_SUFFIX)
PAC_MDIO_DIR		:= $(BUILDDIR)/$(PAC_MDIO)
PAC_MDIO_LICENSE	:= unknown
PAC_MDIO_MD5        := 997400eab176b8725de2b4877bc1d3fc

# ----------------------------------------------------------------------------
# Get
# ----------------------------------------------------------------------------

$(PAC_MDIO_SOURCE):
	@$(call targetinfo)
	@$(call get, PAC_MDIO)

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

#PAC_MDIO_CONF_ENV	:= $(CROSS_ENV)

#
# autoconf
#
PAC_MDIO_CONF_TOOL	:= autoconf
#PAC_MDIO_CONF_OPT	:= $(CROSS_AUTOCONF_USR)

#$(STATEDIR)/pac-mdio.prepare:
#	@$(call targetinfo)
#	@$(call clean, $(PAC_MDIO_DIR)/config.cache)
#	cd $(PAC_MDIO_DIR) && \
#		$(PAC_MDIO_PATH) $(PAC_MDIO_ENV) \
#		./configure $(PAC_MDIO_CONF_OPT)
#	@$(call touch)

# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------

#$(STATEDIR)/pac-mdio.compile:
#	@$(call targetinfo)
#	@$(call world/compile, PAC_MDIO)
#	@$(call touch)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

#$(STATEDIR)/pac-mdio.install:
#	@$(call targetinfo)
#	@$(call world/install, PAC_MDIO)
#	@$(call touch)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/pac-mdio.targetinstall:
	@$(call targetinfo)

	@$(call install_init, pac-mdio)
	@$(call install_fixup, pac-mdio,PRIORITY,optional)
	@$(call install_fixup, pac-mdio,SECTION,base)
	@$(call install_fixup, pac-mdio,AUTHOR,"Heinrich Toews <heinrich.toews@wago.com>")
	@$(call install_fixup, pac-mdio,DESCRIPTION,missing)

	@$(call install_copy, pac-mdio, 0, 0, 0755, $(PAC_MDIO_DIR)/src/pac-mdio, /usr/bin/pac-mdio)
	@$(call install_lib, pac-mdio, 0, 0, 0755, libpac-mdio)

	@$(call install_finish, pac-mdio)

	@$(call touch)

# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------

#$(STATEDIR)/pac-mdio.clean:
#	@$(call targetinfo)
#	@$(call clean_pkg, PAC_MDIO)

# vim: syntax=make
