# -*-makefile-*-
#
# Copyright (C) 2012 by Jan Luebbe <jlu@pengutronix.de>
#
# See CREDITS for details about who has contributed to this project.
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
IMAGE_PACKAGES-$(PTXCONF_IMAGE_BOOT_MLO_SRC) += image-boot-mlo-src

#
# Paths and names
#
IMAGE_BOOT_MLO_SRC		:= image-boot-mlo-src
IMAGE_BOOT_MLO_SRC_DIR		:= $(BUILDDIR)/$(IMAGE_BOOT_MLO_SRC)
IMAGE_BOOT_MLO_SRC_IMAGE	:= $(IMAGEDIR)/boot-mlo-src.vfat
IMAGE_BOOT_MLO_SRC_CONFIG	:= boot-mlo-vfat-src.config

# ----------------------------------------------------------------------------
# Image
# ----------------------------------------------------------------------------


$(IMAGE_BOOT_MLO_SRC_IMAGE): $(STATEDIR)/dtc.targetinstall $(IMAGEDIR)/data-src.ubi
	@$(call targetinfo)
	echo -n $(PTXCONF_IMAGE_BOOT_VFAT_KERNEL_CMDLINE) > $(IMAGEDIR)/cmdline
	@$(call image/genimage, IMAGE_BOOT_MLO_SRC)
	@$(call finish)

# vim: syntax=make
