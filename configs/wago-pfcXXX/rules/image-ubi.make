# -*-makefile-*-
#
# Copyright (C) 2013 by Jan Luebbe <jlu@pengutronix.de>
#
# See CREDITS for details about who has contributed to this project.
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
IMAGE_PACKAGES-$(PTXCONF_IMAGE_DATA_UBI) += image-data-ubi

#
# Paths and names
#
IMAGE_DATA_UBI			:= image-data-ubi
# use separate folder per image to support parallel build
IMAGE_DATA_UBI_DIR		= $(BUILDDIR)/$(IMAGE_DATA_UBI)/$(notdir $@)
IMAGE_DATA_UBI_ROOT		:= $(IMAGEDIR)/root.tgz
ifdef PTXCONF_PFC_200_G2
	IMAGE_DATA_UBI_CONFIG		:= data-ubi_g2.config
else
	IMAGE_DATA_UBI_CONFIG		:= data-ubi.config
endif

# create a list with all available boot*.tgz file in IMAGEDIR
IMAGE_DATA_UBI_BOOT_FILES       := $(strip $(call remove_quotes, $(PTXCONF_IMAGE_DATA_UBI_BOOT_FILES)))

# create a list of *.ubi targets
IMAGE_DATA_UBI_BOOT_FILES_SUBST	:= $(strip $(foreach boot_file, $(IMAGE_DATA_UBI_BOOT_FILES), \
                                        $(subst .tgz,.ubi,$(subst boot,data, $(boot_file)))))

IMAGE_DATA_UBI_IMAGE_FILES := $(strip $(foreach file,$(IMAGE_DATA_UBI_BOOT_FILES), \
                         $(addprefix $(IMAGEDIR)/,$(file))))

IMAGE_DATA_UBI_IMAGE := $(strip $(foreach file,$(IMAGE_DATA_UBI_BOOT_FILES_SUBST), \
                         $(addprefix $(IMAGEDIR)/,$(file))))

# ----------------------------------------------------------------------------
# Image
# ----------------------------------------------------------------------------

$(IMAGE_DATA_UBI_IMAGE):
	@$(call targetinfo)

#	for multiplatform targets like pfcXXX, we need to trick ptxdist because
#	the ptxdist image generation mechanism is build for only one platform
#	with one target.
#	for multiplatform targets we set the below variables per target. In this
#	case the genimage-tool gets only platform-related parameters. feel free
#	to improve this implementation.

#	be careful with the eval command. The produced variables are global. Thats
#	why we are prefixing our variables with target-string
	$(eval $@_IMAGE_DATA_UBI_TMP_FILES 	:= $(IMAGE_DATA_UBI_ROOT))
	$(eval $@_IMAGE_DATA_UBI_TMP_FILES 	+= $(IMAGE_DATA_UBI_IMAGE_FILES))
	$(eval $@_IMAGE_DATA_UBI_TMP_DIR  	:= $(IMAGE_DATA_UBI_DIR))
	$(eval $@_IMAGE_DATA_UBI_TMP_CONFIG	:= $(IMAGE_DATA_UBI_CONFIG))
	$(eval $@_IMAGE_DATA_UBI_TMP_IMAGE 	:= $@)

	@$(call image/genimage, $@_IMAGE_DATA_UBI_TMP)
	@$(call finish)

# vim: syntax=make
