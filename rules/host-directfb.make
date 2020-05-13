# -*-makefile-*-
#
# Copyright (C) 2007 by Marc Kleine-Budde <mkl@pengutronix.de>
#
# See CREDITS for details about who has contributed to this project.
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
HOST_PACKAGES-$(PTXCONF_HOST_DIRECTFB) += host-directfb

#
# Paths and names
#
HOST_DIRECTFB_DIR	= $(HOST_BUILDDIR)/$(DIRECTFB)

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

#
# autoconf
#
HOST_DIRECTFB_CONF_TOOL	:= autoconf
HOST_DIRECTFB_CONF_OPT	:= \
	$(HOST_AUTOCONF) \
	--disable-osx \
	--disable-x11 \
	--disable-network \
	--disable-multi \
	--disable-voodoo \
	--disable-unique \
	--disable-fbdev \
	--disable-sdl \
	--disable-vnc \
	--disable-jpeg \
	--disable-zlib \
	--disable-gif \
	--disable-freetype \
	--disable-video4linux \
	--disable-video4linux2 \
	\
	--with-gfxdrivers=none \
	--with-inputdrivers=none \
	\
	--enable-png

HOST_DIRECTFB_MAKE_OPT	:= \
	-C tools directfb-csource

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

$(STATEDIR)/host-directfb.install:
	@$(call targetinfo)
	install -D -m 755 $(HOST_DIRECTFB_DIR)/tools/directfb-csource $(HOST_DIRECTFB_PKGDIR)/bin/directfb-csource
	@$(call touch)

# vim: syntax=make
