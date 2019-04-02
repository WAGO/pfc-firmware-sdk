# -*-makefile-*-
#
# Copyright (C) 2006 by Erwin Rol
#           (C) 2010 by Michael Olbrich <m.olbrich@pengutronix.de>
#
# See CREDITS for details about who has contributed to this project.
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
PACKAGES-$(PTXCONF_XORG_PROTO_X) += xorg-proto-x

#
# Paths and names
#
XORG_PROTO_X_VERSION 	:= 7.0.31
XORG_PROTO_X_MD5	:= 16791f7ca8c51a20608af11702e51083
XORG_PROTO_X		:= xproto-$(XORG_PROTO_X_VERSION)
XORG_PROTO_X_SUFFIX	:= tar.bz2
XORG_PROTO_X_URL	:= $(call ptx/mirror, XORG, individual/proto/$(XORG_PROTO_X).$(XORG_PROTO_X_SUFFIX))
XORG_PROTO_X_SOURCE	:= $(SRCDIR)/$(XORG_PROTO_X).$(XORG_PROTO_X_SUFFIX)
XORG_PROTO_X_DIR	:= $(BUILDDIR)/$(XORG_PROTO_X)
XORG_PROTO_X_LICENSE	:= MIT

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

#
# autoconf
#
XORG_PROTO_X_CONF_TOOL	:= autoconf
XORG_PROTO_X_CONF_OPT	:= \
	$(CROSS_AUTOCONF_USR) \
	--disable-specs \
	$(XORG_OPTIONS_DOCS)

#
# this was valid for an x86 target. Check on other
# architectures and do not trust the autodetection
#
XORG_PROTO_X_CONF_OPT += \
	--enable-const-prototypes \
	--enable-function-prototypes \
	--enable-varargs-prototypes \
	--enable-nested-prototypes \
	--enable-wide-prototypes=no

# vim: syntax=make

