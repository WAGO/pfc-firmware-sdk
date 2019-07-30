# -*-makefile-*-
#
# Copyright (C) 2004 by Benedikt Spranger
#               2009 by Marc Kleine-Budde <mkl@pengutronix.de>
#               2011 by Michael Olbrich <m.olbrich@pengutronix.de>
#
# See CREDITS for details about who has contributed to this project.
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
PACKAGES-$(PTXCONF_HEXEDIT) += hexedit

#
# Paths and names
#
HEXEDIT_VERSION	:= 1.4.2
HEXEDIT_MD5	:= 6fa1ce9c8c0306f3392f1ea002c80370
HEXEDIT		:= hexedit-$(HEXEDIT_VERSION)
HEXEDIT_SUFFIX	:= tar.gz
HEXEDIT_URL	:= https://github.com/pixel/hexedit/archive/$(HEXEDIT_VERSION).$(HEXEDIT_SUFFIX)
HEXEDIT_SOURCE	:= $(SRCDIR)/$(HEXEDIT).$(HEXEDIT_SUFFIX)
HEXEDIT_DIR	:= $(BUILDDIR)/$(HEXEDIT)
HEXEDIT_LICENSE	:= GPL-2.0-or-later

#
# autoconf
#
HEXEDIT_CONF_TOOL	:= autoconf
HEXEDIT_CONF_OPT	:= \
	$(CROSS_AUTOCONF_USR) \
	$(GLOBAL_LARGE_FILE_OPTION)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/hexedit.targetinstall:
	@$(call targetinfo)

	@$(call install_init, hexedit)
	@$(call install_fixup, hexedit,PRIORITY,optional)
	@$(call install_fixup, hexedit,SECTION,base)
	@$(call install_fixup, hexedit,AUTHOR,"Robert Schwebel <r.schwebel@pengutronix.de>")
	@$(call install_fixup, hexedit,DESCRIPTION,missing)

	@$(call install_copy, hexedit, 0, 0, 0755, -, /usr/bin/hexedit)

	@$(call install_finish, hexedit)

	@$(call touch)

# vim: syntax=make
