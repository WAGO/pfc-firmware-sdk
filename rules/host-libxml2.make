# -*-makefile-*-
#
# Copyright (C) 2007 by Robert Schwebel
#               2008 by Marc Kleine-Budde <mkl@pengutronix.de>
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
HOST_PACKAGES-$(PTXCONF_HOST_LIBXML2) += host-libxml2

#
# Paths and names
#
HOST_LIBXML2_DIR	= $(HOST_BUILDDIR)/$(LIBXML2)

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

HOST_LIBXML2_PATH	:= PATH=$(HOST_PATH)
HOST_LIBXML2_ENV 	:= $(HOST_ENV)

#
# autoconf
#
HOST_LIBXML2_AUTOCONF	:= \
	$(HOST_AUTOCONF) \
	--disable-static \
	--with-iconv=yes \
	--without-python

# vim: syntax=make
