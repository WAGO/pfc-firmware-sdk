# -*-makefile-*-
#
# Copyright (C) 2011 by George McCollister <george.mccollister@gmail.com>
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
HOST_PACKAGES-$(PTXCONF_HOST_OPKG) += host-opkg

#
# Paths and names
#

HOST_OPKG	= $(OPKG)
HOST_OPKG_DIR	= $(HOST_BUILDDIR)/$(HOST_OPKG)

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

HOST_OPKG_ENV	:= $(HOST_ENV)

#
# autoconf
#
HOST_OPKG_CONF_TOOL	:= autoconf
HOST_OPKG_CONF_OPT	:= \
	$(HOST_AUTOCONF) \
	--disable-libopkg-api \
	--disable-static \
	--disable-pathfinder \
	--disable-xz \
	--disable-bzip2 \
	--disable-curl \
	--disable-sha256 \
	--disable-openssl \
	--disable-ssl-curl \
	--disable-gpg \
	--without-libsolv \
	--without-static-libopkg

# vim: syntax=make
