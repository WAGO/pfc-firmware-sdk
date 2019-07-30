# -*-makefile-*-
#
# Copyright (C) 2009 by Marc Kleine-Budde <mkl@pengutronix.de>
#
# See CREDITS for details about who has contributed to this project.
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
LAZY_PACKAGES-$(PTXCONF_HOST_AUTOTOOLS_AUTOMAKE) += host-autotools-automake

#
# Paths and names
#
HOST_AUTOTOOLS_AUTOMAKE_VERSION	:= 1.16.1
HOST_AUTOTOOLS_AUTOMAKE_MD5	:= 53f38e7591fa57c3d2cee682be668e5b
HOST_AUTOTOOLS_AUTOMAKE		:= automake-$(HOST_AUTOTOOLS_AUTOMAKE_VERSION)
HOST_AUTOTOOLS_AUTOMAKE_SUFFIX	:= tar.xz
HOST_AUTOTOOLS_AUTOMAKE_URL	:= $(call ptx/mirror, GNU, automake/$(HOST_AUTOTOOLS_AUTOMAKE).$(HOST_AUTOTOOLS_AUTOMAKE_SUFFIX))
HOST_AUTOTOOLS_AUTOMAKE_SOURCE	:= $(SRCDIR)/$(HOST_AUTOTOOLS_AUTOMAKE).$(HOST_AUTOTOOLS_AUTOMAKE_SUFFIX)
HOST_AUTOTOOLS_AUTOMAKE_DIR	:= $(HOST_BUILDDIR)/$(HOST_AUTOTOOLS_AUTOMAKE)
HOST_AUTOTOOLS_AUTOMAKE_LICENSE	:= GPL-2.0-only

$(STATEDIR)/autogen-tools: $(STATEDIR)/host-autotools-automake.install.post

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

#
# autoconf
#
HOST_AUTOTOOLS_AUTOMAKE_CONF_TOOL	:= autoconf

# create man pages during install to avoid parallel building race
HOST_AUTOTOOLS_AUTOMAKE_MAKE_OPT	:= MANS=

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

$(STATEDIR)/host-autotools-automake.install.post:
	@$(call targetinfo)
	@sed -i \
		-e "s;'\(/share/automake-[^']*\)';'$(PTXDIST_SYSROOT_HOST)\1';g" \
		-e "s;'\(/share/aclocal[^']*\)';'$(PTXDIST_SYSROOT_HOST)\1';g" \
		-e "s;'\(/bin/m4\)';'$(PTXDIST_SYSROOT_HOST)\1';g" \
		$(HOST_AUTOTOOLS_AUTOMAKE_PKGDIR)/bin/* \
		$(HOST_AUTOTOOLS_AUTOMAKE_PKGDIR)/share/automake-*/Automake/Config.pm
	@$(call world/install.post, HOST_AUTOTOOLS_AUTOMAKE)
	@$(call touch)

# vim: syntax=make
