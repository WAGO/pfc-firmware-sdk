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
IMAGE_PACKAGES-$(PTXCONF_IMAGE_SRC_SD) += image-src-sd

#
# Paths and names
#
IMAGE_SRC_SD		:= image-src-sd
IMAGE_SRC_SD_DIR	:= $(BUILDDIR)/$(IMAGE_SRC_SD)
IMAGE_SRC_SD_IMAGE	:= $(IMAGEDIR)/sd_src.hdimg
IMAGE_SRC_SD_FILES	:= $(IMAGEDIR)/root.tgz
IMAGE_SRC_SD_CONFIG	:= sd-src.config

# ----------------------------------------------------------------------------
# Image
# ----------------------------------------------------------------------------

$(IMAGE_SRC_SD_IMAGE):
	@$(call targetinfo)
	@$(call image/genimage, IMAGE_SRC_SD)
	@$(call finish)

# vim: syntax=make
