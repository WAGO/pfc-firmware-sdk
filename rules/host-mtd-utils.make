# -*-makefile-*-
#
# Copyright (C) 2003-2008 by Pengutronix e.K., Hildesheim, Germany
#               2009 by Marc Kleine-Budde <mkl@pengutronix.de>
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
HOST_PACKAGES-$(PTXCONF_HOST_MTD_UTILS) += host-mtd-utils

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

#
# autoconf
#
HOST_MTD_UTILS_CONF_TOOL	:= autoconf
HOST_MTD_UTILS_CONF_OPT		:= \
	$(HOST_AUTOCONF) \
	--disable-unit-tests \
	--disable-tests \
	--disable-install-tests \
	--disable-lsmtd \
	--without-jffs \
	--with-ubifs \
	--with-xattr \
	--with-lzo \
	--without-zstd \
	--without-selinux \
	--without-crypto

# vim: syntax=make
