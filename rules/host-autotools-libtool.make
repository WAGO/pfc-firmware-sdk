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
LAZY_PACKAGES-$(PTXCONF_HOST_AUTOTOOLS_LIBTOOL) += host-autotools-libtool

#
# Paths and names
#
HOST_AUTOTOOLS_LIBTOOL_VERSION	:= 2.4.6
HOST_AUTOTOOLS_LIBTOOL_MD5	:= addf44b646ddb4e3919805aa88fa7c5e
HOST_AUTOTOOLS_LIBTOOL		:= libtool-$(HOST_AUTOTOOLS_LIBTOOL_VERSION)
HOST_AUTOTOOLS_LIBTOOL_SUFFIX	:= tar.gz
HOST_AUTOTOOLS_LIBTOOL_URL	:= $(call ptx/mirror, GNU, libtool/$(HOST_AUTOTOOLS_LIBTOOL).$(HOST_AUTOTOOLS_LIBTOOL_SUFFIX))
HOST_AUTOTOOLS_LIBTOOL_SOURCE	:= $(SRCDIR)/$(HOST_AUTOTOOLS_LIBTOOL).$(HOST_AUTOTOOLS_LIBTOOL_SUFFIX)
HOST_AUTOTOOLS_LIBTOOL_DIR	:= $(HOST_BUILDDIR)/$(HOST_AUTOTOOLS_LIBTOOL)
HOST_AUTOTOOLS_LIBTOOL_DEVPKG	:= NO
HOST_AUTOTOOLS_LIBTOOL_LICENSE	:= GPL-2.0-or-later

$(STATEDIR)/autogen-tools: $(STATEDIR)/host-autotools-libtool.install.post

# ----------------------------------------------------------------------------
# Extract
# ----------------------------------------------------------------------------

$(STATEDIR)/host-autotools-libtool.extract.post:
	@$(call targetinfo)
	@cd $(HOST_AUTOTOOLS_LIBTOOL_DIR) && touch -r m4/ltdl.m4 m4/libtool.m4
	@$(call world/patchin/post, HOST_AUTOTOOLS_LIBTOOL)
	@$(call touch)

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

HOST_AUTOTOOLS_LIBTOOL_CONF_ENV		:= \
	$(HOST_ENV) \
	MAKEINFO=:

#
# autoconf
#
HOST_AUTOTOOLS_LIBTOOL_CONF_TOOL	:= autoconf
HOST_AUTOTOOLS_LIBTOOL_CONF_OPT		:= \
	$(HOST_AUTOCONF_SYSROOT) \
	--disable-static

# vim: syntax=make
