# -*-makefile-*-
#
# Copyright (C) 2008 by mol@pengutronix.de
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
HOST_PACKAGES-$(PTXCONF_HOST_NCURSES) += host-ncurses

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

#
# autoconf
#
HOST_NCURSES_CONF_TOOL	:= autoconf
HOST_NCURSES_CONF_OPT	= \
	$(HOST_AUTOCONF) \
	$(call NCURSES_AUTOCONF_SHARED,NCURSES_SHARED_HOST)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

$(STATEDIR)/host-ncurses.install:
	@$(call targetinfo)
	@$(call world/install, HOST_NCURSES)
#	# don't install headers or libs, so packages like the kernel don't use it
	@rm -r $(HOST_NCURSES_PKGDIR)/include
#	@rm -r $(HOST_NCURSES_PKGDIR)/lib/*.a
	@$(call touch)

# vim: syntax=make
