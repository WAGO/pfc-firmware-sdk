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
IMAGE_PACKAGES-$(PTXCONF_IMAGE_EMMC) += image-emmc

#
# Paths and names
#
IMAGE_EMMC		:= image-emmc
IMAGE_EMMC_DIR		:= $(BUILDDIR)/$(IMAGE_EMMC)
IMAGE_EMMC_IMAGE	:= $(IMAGEDIR)/emmc.hdimg
IMAGE_EMMC_FILES	:= $(IMAGEDIR)/root.tgz
IMAGE_EMMC_CONFIG	:= emmc.config

# ----------------------------------------------------------------------------
# Image
# ----------------------------------------------------------------------------
IMAGE_EMMC_ENV := \
	VFAT_CONFIG='$(shell cat $(PTXDIST_PLATFORMCONFIGDIR)/config/images/vfat.config)'


$(IMAGE_EMMC_IMAGE):
	@$(call targetinfo)
	@$(call image/genimage, IMAGE_EMMC)
	@$(call finish)

# vim: syntax=make
