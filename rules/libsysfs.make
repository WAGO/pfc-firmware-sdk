# -*-makefile-*-
#
# Copyright (C) 2009 by Marc Kleine-Budde <mkl@pengutronix.de>
#
# See CREDITS for details about who has contributed to this project.
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
PACKAGES-$(PTXCONF_LIBSYSFS) += libsysfs

#
# Paths and names
#
LIBSYSFS_VERSION	:= 2.1.0
LIBSYSFS_MD5		:= 14e7dcd0436d2f49aa403f67e1ef7ddc
LIBSYSFS		:= sysfsutils-$(LIBSYSFS_VERSION)
LIBSYSFS_SUFFIX		:= tar.gz
LIBSYSFS_URL		:= $(call ptx/mirror, SF, linux-diag/$(LIBSYSFS).$(LIBSYSFS_SUFFIX))
LIBSYSFS_SOURCE		:= $(SRCDIR)/$(LIBSYSFS).$(LIBSYSFS_SUFFIX)
LIBSYSFS_DIR		:= $(BUILDDIR)/$(LIBSYSFS)
LIBSYSFS_LICENSE	:= GPLv2, LGPLv2
LIBSYSFS_LICENSE_FILES	:= \
	file://cmd/GPL;md5=d41d4e2e1e108554e0388ea4aecd8d27 \
	file://lib/LGPL;md5=b75d069791103ffe1c0d6435deeff72e

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

LIBSYSFS_PATH	:= PATH=$(CROSS_PATH)
LIBSYSFS_ENV 	:= $(CROSS_ENV)

#
# autoconf
#
LIBSYSFS_AUTOCONF := $(CROSS_AUTOCONF_USR)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/libsysfs.targetinstall:
	@$(call targetinfo)

	@$(call install_init, libsysfs)
	@$(call install_fixup, libsysfs,PRIORITY,optional)
	@$(call install_fixup, libsysfs,SECTION,base)
	@$(call install_fixup, libsysfs,AUTHOR,"Marc Kleine-Budde <mkl@pengutronix.de>")
	@$(call install_fixup, libsysfs,DESCRIPTION,missing)

	@$(call install_lib, libsysfs, 0,0, 644, libsysfs)

ifdef PTXCONF_LIBSYSFS_SYSTOOL
	@$(call install_copy, libsysfs, 0, 0, 0775, -, /usr/bin/systool)
endif
	@$(call install_finish, libsysfs)

	@$(call touch)

# vim: syntax=make
