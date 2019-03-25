# -*-makefile-*-
#
# Copyright (C) 2013 by WAGO
#
# See CREDITS for details about who has contributed to this project.
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
PACKAGES-$(PTXCONF_LIBUCI) += libuci

#
# Paths and names
#
LIBUCI_VERSION	:= gitmaster
LIBUCI		:= libuci-$(LIBUCI_VERSION)
LIBUCI_SUFFIX	:= tar.gz
LIBUCI_URL	:= http://svsv01003.wago.local/wago-ptxdist-src/$(LIBUCI).$(LIBUCI_SUFFIX)
LIBUCI_SOURCE	:= $(SRCDIR)/$(LIBUCI).$(LIBUCI_SUFFIX)
LIBUCI_MD5      := e31ef3176688297d19cb7fe1e7645bb9
LIBUCI_DIR	:= $(BUILDDIR)/$(LIBUCI)

# ----------------------------------------------------------------------------
# Get
# ----------------------------------------------------------------------------

$(LIBUCI_SOURCE):
	@$(call targetinfo)
	@$(call get, LIBUCI)

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

LIBUCI_CONF_TOOL := cmake

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/libuci.targetinstall:
	@$(call targetinfo)

	@$(call install_init, libuci)
	@$(call install_fixup, libuci,PRIORITY,optional)
	@$(call install_fixup, libuci,SECTION,base)
	@$(call install_fixup, libuci,AUTHOR,"Marc Kleine-Budde <mkl@pengutronix.de>")
	@$(call install_fixup, libuci,DESCRIPTION,missing)

	@$(call install_lib, libuci, 0, 0, 0644, libuci)

	@$(call install_finish, libuci)

	@$(call touch)

# vim: syntax=make
