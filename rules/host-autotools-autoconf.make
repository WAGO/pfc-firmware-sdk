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
LAZY_PACKAGES-$(PTXCONF_HOST_AUTOTOOLS_AUTOCONF) += host-autotools-autoconf

#
# Paths and names
#
HOST_AUTOTOOLS_AUTOCONF_VERSION	:= 2.69
HOST_AUTOTOOLS_AUTOCONF_MD5	:= 50f97f4159805e374639a73e2636f22e
HOST_AUTOTOOLS_AUTOCONF		:= autoconf-$(HOST_AUTOTOOLS_AUTOCONF_VERSION)
HOST_AUTOTOOLS_AUTOCONF_SUFFIX	:= tar.xz
HOST_AUTOTOOLS_AUTOCONF_URL	:= $(call ptx/mirror, GNU, autoconf/$(HOST_AUTOTOOLS_AUTOCONF).$(HOST_AUTOTOOLS_AUTOCONF_SUFFIX))
HOST_AUTOTOOLS_AUTOCONF_SOURCE	:= $(SRCDIR)/$(HOST_AUTOTOOLS_AUTOCONF).$(HOST_AUTOTOOLS_AUTOCONF_SUFFIX)
HOST_AUTOTOOLS_AUTOCONF_DIR	:= $(HOST_BUILDDIR)/$(HOST_AUTOTOOLS_AUTOCONF)
HOST_AUTOTOOLS_AUTOCONF_LICENSE	:= GPL-2.0-only AND GPL-3.0-only AND Autoconf-exception-3.0
HOST_AUTOTOOLS_AUTOCONF_LICENSE_FILES := \
	file://COPYING;md5=751419260aa954499f7abaabaa882bbe \
	file://COPYINGv3;md5=d32239bcb673463ab874e80d47fae504 \
	file://COPYING.EXCEPTION;md5=fdef168ebff3bc2f13664c365a5fb515

$(STATEDIR)/autogen-tools: $(STATEDIR)/host-autotools-autoconf.install.post

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

#
# autoconf
#
HOST_AUTOTOOLS_AUTOCONF_CONF_TOOL	:= autoconf

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

$(STATEDIR)/host-autotools-autoconf.install.post:
	@$(call targetinfo)
	@sed -i \
		-e "s;'\(/share/autoconf\)';'$(PTXDIST_SYSROOT_HOST)\1';g" \
		-e "s;'\(/bin/auto[^']*\)';'$(PTXDIST_SYSROOT_HOST)\1';g" \
		-e "s;'/[^']*/sysroot-host\(/bin/m4\)';'$(PTXDIST_SYSROOT_HOST)\1';g" \
		$(HOST_AUTOTOOLS_AUTOCONF_PKGDIR)/bin/* \
		$(HOST_AUTOTOOLS_AUTOCONF_PKGDIR)/share/autoconf/autom4te.cfg
	@$(call world/install.post, HOST_AUTOTOOLS_AUTOCONF)
	@$(call touch)

# vim: syntax=make
