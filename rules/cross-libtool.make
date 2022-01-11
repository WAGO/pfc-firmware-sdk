# -*-makefile-*-
#
# Copyright (C) 2008 by Michael Olbrich <m.olbrich@pengutronix.de>
#
# See CREDITS for details about who has contributed to this project.
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
LAZY_PACKAGES-$(PTXCONF_CROSS_LIBTOOL) += cross-libtool

#
# Paths and names
#
CROSS_LIBTOOL		= $(LIBLTDL)
CROSS_LIBTOOL_MD5	= $(LIBLTDL_MD5)
CROSS_LIBTOOL_DIR	= $(CROSS_BUILDDIR)/$(LIBLTDL)
CROSS_LIBTOOL_URL	= $(LIBLTDL_URL)
CROSS_LIBTOOL_SOURCE	= $(LIBLTDL_SOURCE)
CROSS_LIBTOOL_DEVPKG	= NO

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

# We really only need the libtool script and it should default to $(CROSS_CC)
# as compiler. So we configure libtool for the target and only create and
# install the scripts.

CROSS_LIBTOOL_CONF_ENV := \
	$(CROSS_ENV)

#
# autoconf
#
CROSS_LIBTOOL_AUTOCONF	:= \
	--prefix=$(PTXCONF_SYSROOT_CROSS) \
	--host=$(PTXCONF_GNU_TARGET) \
	--build=$(GNU_HOST)

CROSS_LIBTOOL_MAKE_OPT		:= libtool libtoolize
CROSS_LIBTOOL_INSTALL_OPT	:= install-binSCRIPTS

# vim: syntax=make
