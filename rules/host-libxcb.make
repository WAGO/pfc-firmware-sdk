# -*-makefile-*-
#
# Copyright (C) 2011 by Michael Olbrich <m.olbrich@pengutronix.de>
#
# See CREDITS for details about who has contributed to this project.
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
HOST_PACKAGES-$(PTXCONF_HOST_LIBXCB) += host-libxcb

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

HOST_LIBXCB_ENV	:= \
	$(HOST_ENV) \
	ac_cv_prog_BUILD_DOCS=no \
	ac_cv_lib_Xdmcp_XdmcpWrap=no

#
# autoconf
#
HOST_LIBXCB_CONF_TOOL	:= autoconf
HOST_LIBXCB_CONF_OPT	:= \
	$(HOST_AUTOCONF)

# vim: syntax=make
