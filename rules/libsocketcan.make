# -*-makefile-*-
#
# Copyright (C) 2009 by Luotao Fu <l.fu@pengutronix.de>
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
PACKAGES-$(PTXCONF_LIBSOCKETCAN) += libsocketcan

#
# Paths and names
#
LIBSOCKETCAN_VERSION	:= 0.0.11
LIBSOCKETCAN_MD5	:= 2722cc3fcb4db9d1cfc57906391c8169
LIBSOCKETCAN		:= libsocketcan-$(LIBSOCKETCAN_VERSION)
LIBSOCKETCAN_SUFFIX	:= tar.bz2
LIBSOCKETCAN_URL	:= https://public.pengutronix.de/software/libsocketcan/$(LIBSOCKETCAN).$(LIBSOCKETCAN_SUFFIX)
LIBSOCKETCAN_SOURCE	:= $(SRCDIR)/$(LIBSOCKETCAN).$(LIBSOCKETCAN_SUFFIX)
LIBSOCKETCAN_DIR	:= $(BUILDDIR)/$(LIBSOCKETCAN)
LIBSOCKETCAN_LICENSE	:= LGPL-2.1-or-later
LIBSOCKETCAN_LICENSE_FILES := file://LICENSE;md5=4fbd65380cdd255951079008b364516c

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

#
# autoconf
#
LIBSOCKETCAN_AUTOCONF := $(CROSS_AUTOCONF_USR)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/libsocketcan.targetinstall:
	@$(call targetinfo)

	@$(call install_init, libsocketcan)
	@$(call install_fixup, libsocketcan,PRIORITY,optional)
	@$(call install_fixup, libsocketcan,SECTION,base)
	@$(call install_fixup, libsocketcan,AUTHOR,"Luotao Fu <l.fu@pengutronix.de>")
	@$(call install_fixup, libsocketcan,DESCRIPTION,missing)

	@$(call install_lib, libsocketcan, 0, 0, 0644, libsocketcan)

	@$(call install_finish, libsocketcan)

	@$(call touch)

# vim: syntax=make
