# -*-makefile-*-
#
# Copyright (C) 2009 by Erwin Rol
# Copyright (C) 2009 by Robert Schwebel
#
# See CREDITS for details about who has contributed to this project.
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
PACKAGES-$(PTXCONF_DRI2PROTO) += dri2proto

#
# Paths and names
#
DRI2PROTO_VERSION	:= 2.8
DRI2PROTO_MD5		:= 2eb74959684f47c862081099059a11ab
DRI2PROTO		:= dri2proto-$(DRI2PROTO_VERSION)
DRI2PROTO_SUFFIX	:= tar.bz2
DRI2PROTO_URL		:= $(call ptx/mirror, XORG, individual/proto/$(DRI2PROTO).$(DRI2PROTO_SUFFIX))
DRI2PROTO_SOURCE	:= $(SRCDIR)/$(DRI2PROTO).$(DRI2PROTO_SUFFIX)
DRI2PROTO_DIR		:= $(BUILDDIR)/$(DRI2PROTO)

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

#
# autoconf
#
DRI2PROTO_CONF_TOOL := autoconf

# vim: syntax=make
