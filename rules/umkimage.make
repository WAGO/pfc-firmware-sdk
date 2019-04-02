# -*-makefile-*-
#
# Copyright (C) 2010 by Remy Bohmer <linux@bohmer.net>
#
# See CREDITS for details about who has contributed to this project.
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
PACKAGES-$(PTXCONF_UMKIMAGE) += umkimage

#
# Paths and names
#
UMKIMAGE_VERSION	:= 2010.03-1
UMKIMAGE		:= u-boot-mkimage-$(UMKIMAGE_VERSION)
UMKIMAGE_SUFFIX		:= tar.gz
UMKIMAGE_URL		:= http://www.pengutronix.de/software/ptxdist/temporary-src/$(UMKIMAGE).$(UMKIMAGE_SUFFIX)
UMKIMAGE_SOURCE		:= $(SRCDIR)/$(UMKIMAGE).$(UMKIMAGE_SUFFIX)
UMKIMAGE_DIR		:= $(BUILDDIR)/$(UMKIMAGE)
UMKIMAGE_LICENSE	:= GPLv2
UMKIMAGE_MD5        := 5e7b4ed03e855a4824317fc4e3917dba 

# ----------------------------------------------------------------------------
# Get
# ----------------------------------------------------------------------------

$(UMKIMAGE_SOURCE):
	@$(call targetinfo)
	@$(call get, UMKIMAGE)

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

UMKIMAGE_CONF_TOOL	:= NO
UMKIMAGE_MAKE_OPT := \
	$(CROSS_ENV_CFLAGS) \
	$(CROSS_ENV_CPPFLAGS) \
	$(CROSS_ENV_LDFLAGS) \
	$(CROSS_ENV_CC)

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

$(STATEDIR)/umkimage.install:
	@$(call targetinfo, $@)
	mkdir -p $(PTXDIST_SYSROOT_TARGET)/include/u-boot-mkimage
	install $(UMKIMAGE_DIR)/include/export/image.h $(PTXDIST_SYSROOT_TARGET)/include/u-boot-mkimage/image.h
	install $(UMKIMAGE_DIR)/include/compiler.h $(PTXDIST_SYSROOT_TARGET)/include/u-boot-mkimage/compiler.h
	@$(call touch, $@)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/umkimage.targetinstall:
	@$(call targetinfo)

	@$(call install_init,  umkimage)
	@$(call install_fixup, umkimage,PRIORITY,optional)
	@$(call install_fixup, umkimage,SECTION,base)
	@$(call install_fixup, umkimage,AUTHOR,"Remy Bohmer <linux@bohmer.net>")
	@$(call install_fixup, umkimage,DESCRIPTION,missing)

ifdef PTXCONF_UMKIMAGE_INSTALL_MKIMAGE
	@$(call install_copy, umkimage, 0, 0, 0755, $(UMKIMAGE_DIR)/mkimage, /usr/bin/mkimage)
endif

	@$(call install_finish, umkimage)

	@$(call touch)

# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------

umkimage_clean:
	rm -rf $(STATEDIR)/umkimage.*
	rm -rf $(PTXCONF_SYSROOT_TARGET)/include/u-boot-mkimage

# vim: syntax=make
