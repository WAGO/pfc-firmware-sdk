# -*-makefile-*-
#
# Copyright (C) 2009 by Juergen Beisert
#
# See CREDITS for details about who has contributed to this project.
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
PACKAGES-$(PTXCONF_LIBUSB) += libusb

#
# Paths and names
#
LIBUSB_VERSION	:= 1.0.22
LIBUSB_MD5	:= 466267889daead47674df933cea9cacb
LIBUSB		:= libusb-$(LIBUSB_VERSION)
LIBUSB_SUFFIX	:= tar.bz2
LIBUSB_URL	:= $(call ptx/mirror, SF, libusb/$(LIBUSB).$(LIBUSB_SUFFIX))
LIBUSB_SOURCE	:= $(SRCDIR)/$(LIBUSB).$(LIBUSB_SUFFIX)
LIBUSB_DIR	:= $(BUILDDIR)/$(LIBUSB)
LIBUSB_LICENSE	:= LGPL-2.1-only

#
# autoconf
#
LIBUSB_AUTOCONF := \
	$(CROSS_AUTOCONF_USR) \
	--enable-shared \
	--disable-static \
	--disable-udev \
	--enable-timerfd \
	--$(call ptx/endis, PTXCONF_LIBUSB_LOG)-log \
	--$(call ptx/endis, PTXCONF_LIBUSB_DEBUG_LOG)-debug-log \
	--$(call ptx/endis, PTXCONF_LIBUSB_SYSTEM_LOG)-system-log \
	--disable-examples-build \
	--disable-tests-build

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/libusb.targetinstall:
	@$(call targetinfo)

	@$(call install_init, libusb)
	@$(call install_fixup, libusb,PRIORITY,optional)
	@$(call install_fixup, libusb,SECTION,base)
	@$(call install_fixup, libusb,AUTHOR,"Juergen Beisert <j.beisert@pengutronix.de>")
	@$(call install_fixup, libusb,DESCRIPTION,missing)

	@$(call install_lib, libusb, 0, 0, 0644, libusb-1.0)

	@$(call install_finish, libusb)

	@$(call touch)

# vim: syntax=make
