# -*-makefile-*-
#
# Copyright (C) 2014 by Michael Olbrich <m.olbrich@pengutronix.de>
# Copyright (C) 2019 by Oleg Karfich <oleg.karfich@wago.de>
#
# See CREDITS for details about who has contributed to this project.
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
IMAGE_PACKAGES-$(PTXCONF_IMAGE_BOOT_VFAT) += image-boot-vfat

#
# Paths and names
#
IMAGE_BOOT_VFAT		:= image-boot-vfat
IMAGE_BOOT_VFAT_DIR	:= $(BUILDDIR)/$(IMAGE_BOOT_VFAT)
IMAGE_BOOT_VFAT_IMAGE	:= $(IMAGEDIR)/boot.vfat

ifdef PTXCONF_IMAGE_BOOT_VFAT_MLO_BAREBOX_PFC
IMAGE_BOOT_VFAT_CONFIG	:= boot-vfat-mlo-barebox-pfc.config
endif

ifdef PTXCONF_IMAGE_BOOT_VFAT_MLO_BAREBOX_PAC
IMAGE_BOOT_VFAT_CONFIG	:= boot-vfat-mlo-barebox-pac.config
endif

# ----------------------------------------------------------------------------
# Image
# ----------------------------------------------------------------------------

$(IMAGE_BOOT_VFAT_IMAGE):
	@$(call targetinfo)
	@$(call image/genimage, IMAGE_BOOT_VFAT)

	# This is only a temporaly workaround until the Development-Tools-Team
	# introduce a proper solution for configuring jenkins artifacts.
ifdef PTXCONF_IMAGE_BOOT_VFAT_MLO_BAREBOX_PAC
	@> $(IMAGEDIR)/emmc-commission-pfc200v3.img
	@> $(IMAGEDIR)/emmc-wago-production-pfc200v3.img
	@> $(IMAGEDIR)/firmware.tar
	@> $(IMAGEDIR)/nand-wago-production-pfc100.ubi
	@> $(IMAGEDIR)/nand-wago-production-pfc200.img
	@> $(IMAGEDIR)/nand-wago-production-pfc200v2.ubi
	@> $(IMAGEDIR)/sd-downgrade-firmware-02-pfc200.img
	@> $(IMAGEDIR)/sd-downgrade-firmware-03-pfc200.img
	@> $(IMAGEDIR)/sd-downgrade-firmware-04-pfc200.img
endif

	@$(call finish)

# vim: syntax=make
