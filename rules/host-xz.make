# -*-makefile-*-
#
# Copyright (C) 2010 by Marc Kleine-Budde <mkl@pengutronix.de>
#
# See CREDITS for details about who has contributed to this project.
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
HOST_PACKAGES-$(PTXCONF_HOST_XZ) += host-xz

#
# Paths and names
#
HOST_XZ_VERSION	:= 5.2.1
HOST_XZ_MD5	:= d484910b26fec5aff99ee66350589e29
HOST_XZ		:= xz-$(HOST_XZ_VERSION)
HOST_XZ_SUFFIX	:= tar.bz2
HOST_XZ_URL	:= http://tukaani.org/xz/$(HOST_XZ).$(HOST_XZ_SUFFIX)
HOST_XZ_SOURCE	:= $(SRCDIR)/$(HOST_XZ).$(HOST_XZ_SUFFIX)
HOST_XZ_DIR	:= $(HOST_BUILDDIR)/$(HOST_XZ)

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

#
# autoconf
#
HOST_XZ_CONF_TOOL	:= autoconf
HOST_XZ_CONF_OPT	:= \
	$(HOST_AUTOCONF) \
	--libdir=/lib/xz \
	--disable-shared

# vim: syntax=make
