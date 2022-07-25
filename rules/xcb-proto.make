# -*-makefile-*-
#
# Copyright (C) 2007 by Michael Olbrich <m.olbrich@pengutronix.de>
# Copyright (C) 2008 by Robert Schwebel
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
PACKAGES-$(PTXCONF_XCB_PROTO) += xcb-proto

#
# Paths and names
#
XCB_PROTO_VERSION	:= 1.14.1
XCB_PROTO_MD5		:= ecd6955dab1a7b9ba9756a11b8bdb48f
XCB_PROTO		:= xcb-proto-$(XCB_PROTO_VERSION)
XCB_PROTO_SUFFIX	:= tar.xz
XCB_PROTO_URL		:= http://xorg.freedesktop.org/archive/individual/proto/$(XCB_PROTO).$(XCB_PROTO_SUFFIX)
XCB_PROTO_SOURCE	:= $(SRCDIR)/$(XCB_PROTO).$(XCB_PROTO_SUFFIX)
XCB_PROTO_DIR		:= $(BUILDDIR)/$(XCB_PROTO)
XCB_PROTO_LICENSE	:= MIT

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

XCB_PROTO_CONF_ENV := \
	$(CROSS_ENV) \
	ac_cv_path_PYTHON=python3

#
# autoconf
#
XCB_PROTO_CONF_TOOL := autoconf

# vim: syntax=make
