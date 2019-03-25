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
IMAGE_PACKAGES-$(PTXCONF_IMAGE_SD_DOWNGRADE) += image-sd-downgrade

#
# Paths and names
#
IMAGE_SD_DOWNGRADE		:= image-sd-downgrade
IMAGE_SD_DOWNGRADE_DIR		:= $(BUILDDIR)/$(IMAGE_SD_DOWNGRADE)
IMAGE_SD_DOWNGRADE_IMAGE	:= $(IMAGEDIR)/sd-downgrade.hdimg
IMAGE_SD_DOWNGRADE_CONFIG	:= sd-downgrade.config
IMAGE_SD_DOWNGRADE_ARCHIVES	:= $(call remove_quotes,$(PTXCONF_IMAGE_SD_DOWNGRADE_ARCHIVES))
IMAGE_SD_DOWNGRADE_VFAT_LIST	:= $(call remove_quotes,$(PTXCONF_IMAGE_SD_DOWNGRADE_VFAT))

# ----------------------------------------------------------------------------
# Image
# ----------------------------------------------------------------------------

$(IMAGE_SD_DOWNGRADE_IMAGE):
	@$(call targetinfo)

	@for vfat_image in $(IMAGE_SD_DOWNGRADE_VFAT_LIST); do \
		if [ ! -e "$(IMAGEDIR)/$${vfat_image}" ]; then \
			echo "ERROR: $(IMAGEDIR)/$${vfat_image} not available"; \
			exit 1; \
		else \
			ln -s -f "$(IMAGEDIR)/$${vfat_image}" $(IMAGEDIR)/boot-downgrade.vfat; \
			$(call image/genimage, IMAGE_SD_DOWNGRADE); \
			_IMG_NAME=$$(echo $${vfat_image} | sed 's/.vfat//g'); \
			mv $(IMAGE_SD_DOWNGRADE_IMAGE) $(IMAGEDIR)/sd-downgrade-$${_IMG_NAME}.hdimg; \
		fi \
	done

	@rm -f $(IMAGEDIR)/boot-downgrade.vfat

	@$(call finish)

# vim: syntax=make
