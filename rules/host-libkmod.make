# -*-makefile-*-
#
# Copyright (C) 2012 by Michael Olbrich <m.olbrich@pengutronix.de>
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
HOST_PACKAGES-$(PTXCONF_HOST_LIBKMOD) += host-libkmod

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

#
# autoconf
#
HOST_LIBKMOD_CONF_TOOL	:= autoconf
HOST_LIBKMOD_CONF_OPT	:= \
	$(HOST_AUTOCONF) \
	--disable-static \
	--enable-shared \
	--enable-tools \
	--disable-manpages \
	--disable-test-modules \
	--disable-logging \
	--disable-debug \
	--disable-python \
	--disable-coverage \
	--without-xz \
	--without-zlib \
	--without-openssl

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

$(STATEDIR)/host-libkmod.install:
	@$(call targetinfo)
	@$(call world/install, HOST_LIBKMOD)
	@ln -s ../bin/kmod $(HOST_LIBKMOD_PKGDIR)/sbin/depmod
	@$(call touch)

# vim: syntax=make
