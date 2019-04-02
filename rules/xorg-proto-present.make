# -*-makefile-*-
#
# Copyright (C) 2014 by Michael Olbrich <m.olbrich@pengutronix.de>
#
# See CREDITS for details about who has contributed to this project.
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
PACKAGES-$(PTXCONF_XORG_PROTO_PRESENT) += xorg-proto-present

#
# Paths and names
#
XORG_PROTO_PRESENT_VERSION	:= 1.1
XORG_PROTO_PRESENT_MD5		:= 92f9dda9c870d78a1d93f366bcb0e6cd
XORG_PROTO_PRESENT		:= presentproto-$(XORG_PROTO_PRESENT_VERSION)
XORG_PROTO_PRESENT_SUFFIX	:= tar.bz2
XORG_PROTO_PRESENT_URL		:= $(call ptx/mirror, XORG, individual/proto/$(XORG_PROTO_PRESENT).$(XORG_PROTO_PRESENT_SUFFIX))
XORG_PROTO_PRESENT_SOURCE	:= $(SRCDIR)/$(XORG_PROTO_PRESENT).$(XORG_PROTO_PRESENT_SUFFIX)
XORG_PROTO_PRESENT_DIR		:= $(BUILDDIR)/$(XORG_PROTO_PRESENT)
XORG_PROTO_PRESENT_LICENSE	:= MIT
XORG_PROTO_PRESENT_LICENSE_FILES := \
	file://presentproto.h;startline=2;endline=20;md5=2dd66dffa047e40483dd101640f3043b

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

#
# autoconf
#
XORG_PROTO_PRESENT_CONF_TOOL	:= autoconf

# vim: syntax=make
