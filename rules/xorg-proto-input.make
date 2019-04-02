# -*-makefile-*-
#
# Copyright (C) 2006 by Erwin Rol
#           (C) 2009 by Robert Schwebel
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
PACKAGES-$(PTXCONF_XORG_PROTO_INPUT) += xorg-proto-input

#
# Paths and names
#
XORG_PROTO_INPUT_VERSION:= 2.3.2
XORG_PROTO_INPUT_MD5	:= b290a463af7def483e6e190de460f31a
XORG_PROTO_INPUT	:= inputproto-$(XORG_PROTO_INPUT_VERSION)
XORG_PROTO_INPUT_SUFFIX	:= tar.bz2
XORG_PROTO_INPUT_URL	:= $(call ptx/mirror, XORG, individual/proto/$(XORG_PROTO_INPUT).$(XORG_PROTO_INPUT_SUFFIX))
XORG_PROTO_INPUT_SOURCE	:= $(SRCDIR)/$(XORG_PROTO_INPUT).$(XORG_PROTO_INPUT_SUFFIX)
XORG_PROTO_INPUT_DIR	:= $(BUILDDIR)/$(XORG_PROTO_INPUT)
XORG_PROTO_INPUT_LICENSE	:= MIT

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

#
# autoconf
#
XORG_PROTO_INPUT_CONF_TOOL	:= autoconf
XORG_PROTO_INPUT_CONF_OPT	:= \
	$(CROSS_AUTOCONF_USR) \
	--disable-specs \
	--without-asciidoc

# vim: syntax=make

