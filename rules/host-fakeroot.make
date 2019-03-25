# -*-makefile-*-
#
# Copyright (C) 2003 by Benedikt Spranger
#               2010 by Marc Kleine-Budde <mkl@pengutronix.de>
#
# See CREDITS for details about who has contributed to this project.
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
HOST_PACKAGES-$(PTXCONF_HOST_FAKEROOT) += host-fakeroot

#
# Paths and names
#
HOST_FAKEROOT_VERSION	:= 1.20.2
HOST_FAKEROOT_MD5	:= a4b4564a75024aa96c86e4d1017ac786
HOST_FAKEROOT_SUFFIX	:= tar.bz2
HOST_FAKEROOT		:= fakeroot-$(HOST_FAKEROOT_VERSION)
HOST_FAKEROOT_TARBALL	:= fakeroot_$(HOST_FAKEROOT_VERSION).orig.$(HOST_FAKEROOT_SUFFIX)
HOST_FAKEROOT_URL	:= $(call ptx/mirror, DEB, pool/main/f/fakeroot/$(HOST_FAKEROOT_TARBALL))
HOST_FAKEROOT_SOURCE	:= $(SRCDIR)/$(HOST_FAKEROOT_TARBALL)
HOST_FAKEROOT_DIR	:= $(HOST_BUILDDIR)/$(HOST_FAKEROOT)

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

HOST_FAKEROOT_CONF_TOOL := autoconf
HOST_FAKEROOT_CONF_OPT := \
	$(HOST_AUTOCONF) \
	--libdir=/lib/fakeroot

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

$(STATEDIR)/host-fakeroot.install.post:
	@$(call targetinfo)
	@$(call world/install.post, HOST_FAKEROOT)
	@sed -ie 's,FAKEROOT_SYSROOT,$(PTXCONF_SYSROOT_HOST),' \
		$(PTXCONF_SYSROOT_HOST)/bin/fakeroot
	@$(call touch)

# vim: syntax=make
