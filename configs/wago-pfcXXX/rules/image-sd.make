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
IMAGE_PACKAGES-$(PTXCONF_IMAGE_SD) += image-sd

#
# Paths and names
#
IMAGE_SD	:= image-sd
IMAGE_SD_DIR	:= $(BUILDDIR)/$(IMAGE_SD)
IMAGE_SD_IMAGE	:= $(IMAGEDIR)/sd.hdimg
IMAGE_SD_FILES	:= $(IMAGEDIR)/root.tgz
IMAGE_SD_CONFIG	:= sd.config

# ----------------------------------------------------------------------------
# Image
# ----------------------------------------------------------------------------

$(IMAGE_SD_IMAGE):
	@$(call targetinfo)
	@$(call image/genimage, IMAGE_SD)
	@$(call finish)

# vim: syntax=make
