# -*-makefile-*-
#
# Copyright (C) 2010 by Erwin Rol <erwin@erwinrol.com>
#
# See CREDITS for details about who has contributed to this project.
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
PACKAGES-$(PTXCONF_XCB_UTIL) += xcb-util

#
# Paths and names
#
XCB_UTIL_VERSION	:= 0.3.9
XCB_UTIL_MD5		:= 01dcc7a16d5020530552712710646ea2
XCB_UTIL		:= xcb-util-$(XCB_UTIL_VERSION)
XCB_UTIL_SUFFIX		:= tar.bz2
XCB_UTIL_URL		:= http://xcb.freedesktop.org/dist/$(XCB_UTIL).$(XCB_UTIL_SUFFIX)
XCB_UTIL_SOURCE		:= $(SRCDIR)/$(XCB_UTIL).$(XCB_UTIL_SUFFIX)
XCB_UTIL_DIR		:= $(BUILDDIR)/$(XCB_UTIL)
XCB_UTIL_LICENSE	:= unknown

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

XCB_UTIL_PATH		:= PATH=$(CROSS_PATH)
XCB_UTIL_CONF_ENV	:= $(CROSS_ENV)

#
# autoconf
#
XCB_UTIL_CONF_TOOL	:= autoconf

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/xcb-util.targetinstall:
	@$(call targetinfo)

	@$(call install_init, xcb-util)
	@$(call install_fixup, xcb-util,PRIORITY,optional)
	@$(call install_fixup, xcb-util,SECTION,base)
	@$(call install_fixup, xcb-util,AUTHOR,"Erwin Rol <erwin@erwinrol.com>")
	@$(call install_fixup, xcb-util,DESCRIPTION,missing)

	@$(call install_lib, xcb-util, 0, 0, 0644, libxcb-util)
#	@$(call install_lib, xcb-util, 0, 0, 0644, libxcb-atom)
#	@$(call install_lib, xcb-util, 0, 0, 0644, libxcb-keysyms)
#	@$(call install_lib, xcb-util, 0, 0, 0644, libxcb-aux)
#	@$(call install_lib, xcb-util, 0, 0, 0644, libxcb-property)
#	@$(call install_lib, xcb-util, 0, 0, 0644, libxcb-event)
#	@$(call install_lib, xcb-util, 0, 0, 0644, libxcb-render-util)
#	@$(call install_lib, xcb-util, 0, 0, 0644, libxcb-icccm)
#	@$(call install_lib, xcb-util, 0, 0, 0644, libxcb-reply)
#	@$(call install_lib, xcb-util, 0, 0, 0644, libxcb-image)

	@$(call install_finish, xcb-util)

	@$(call touch)


# vim: syntax=make
