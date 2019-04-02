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
PACKAGES-$(PTXCONF_XORG_DRIVER_INPUT_EVDEV) += xorg-driver-input-evdev

#
# Paths and names
#
XORG_DRIVER_INPUT_EVDEV_VERSION	:= 2.10.6
XORG_DRIVER_INPUT_EVDEV_MD5	:= e8bd1edc6751f92e425cae7eba3c61eb
XORG_DRIVER_INPUT_EVDEV		:= xf86-input-evdev-$(XORG_DRIVER_INPUT_EVDEV_VERSION)
XORG_DRIVER_INPUT_EVDEV_SUFFIX	:= tar.bz2
XORG_DRIVER_INPUT_EVDEV_URL	:= $(call ptx/mirror, XORG, individual/driver/$(XORG_DRIVER_INPUT_EVDEV).$(XORG_DRIVER_INPUT_EVDEV_SUFFIX))
XORG_DRIVER_INPUT_EVDEV_SOURCE	:= $(SRCDIR)/$(XORG_DRIVER_INPUT_EVDEV).$(XORG_DRIVER_INPUT_EVDEV_SUFFIX)
XORG_DRIVER_INPUT_EVDEV_DIR	:= $(BUILDDIR)/$(XORG_DRIVER_INPUT_EVDEV)

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

XORG_DRIVER_INPUT_EVDEV_CONF_ENV := \
	$(CROSS_ENV) \
	PTXDIST_PKG_CONFIG_VAR_NO_SYSROOT=sdkdir

#
# autoconf
#
XORG_DRIVER_INPUT_EVDEV_CONF_TOOL := autoconf

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/xorg-driver-input-evdev.targetinstall:
	@$(call targetinfo)

	@$(call install_init, xorg-driver-input-evdev)
	@$(call install_fixup, xorg-driver-input-evdev,PRIORITY,optional)
	@$(call install_fixup, xorg-driver-input-evdev,SECTION,base)
	@$(call install_fixup, xorg-driver-input-evdev,AUTHOR,"Erwin Rol <ero@pengutronix.de>")
	@$(call install_fixup, xorg-driver-input-evdev,DESCRIPTION,missing)

	@$(call install_copy, xorg-driver-input-evdev, 0, 0, 0755, -, \
		/usr/lib/xorg/modules/input/evdev_drv.so)

	@$(call install_finish, xorg-driver-input-evdev)

	@$(call touch)

# vim: syntax=make
