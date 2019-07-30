# -*-makefile-*-
#
# Copyright (C) 2014 by <AGa>
#
# See CREDITS for details about who has contributed to this project.
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
IMAGE_PACKAGES-$(PTXCONF_IMAGE_BOOT_TAR) += image-boot-tar

#
# Paths and names
#
IMAGE_BOOT_TAR			:= image-boot-tgz
IMAGE_BOOT_TAR_DIR		:= $(BUILDDIR)/$(IMAGE_BOOT_TAR)
IMAGE_BOOT_TAR_IMAGE		:= $(IMAGEDIR)/boot.tgz
IMAGE_BOOT_TAR_PLAT_SUFFIX	:= $(call remove_quotes, $(PTXCONF_IMAGE_BOOT_TAR_PLAT_SUFFIX))

ifeq ($(IMAGE_BOOT_TAR_PLAT_SUFFIX),)

IMAGE_BOOT_TAR_IMAGE		:= $(IMAGEDIR)/boot.tgz

else

IMAGE_BOOT_TAR_IMAGE		:= $(strip $(foreach suffix, $(IMAGE_BOOT_TAR_PLAT_SUFFIX), \
					$(addprefix $(IMAGEDIR)/boot-,$(suffix).tgz)))

endif

IMAGE_BOOT_TAR_ADDITIONAL_FILES	:= $(call remove_quotes, $(PTXCONF_IMAGE_BOOT_TAR_ADDITIONAL_FILES))
LOCAL_TEMPDIR	= $(PTXDIST_TEMPDIR)/$(notdir $@)
IMAGE_BOOT_TMP_DIR		= $(LOCAL_TEMPDIR)/boot

# ----------------------------------------------------------------------------
# Image
# ----------------------------------------------------------------------------

$(IMAGE_BOOT_TAR_IMAGE): $(IMAGEDIR)/linuximage $(STATEDIR)/dtc.targetinstall

	@$(call targetinfo)

	@mkdir -p $(IMAGE_BOOT_TMP_DIR)

#	first extract the target suffix
#	if no suffix is configured, we generate
#	only boot.tgz and leave the SUFFIX-variable
#	empty.
ifneq ($(IMAGE_BOOT_TAR_PLAT_SUFFIX),)
	@$(eval SUFFIX := $(shell echo $(notdir $@) | sed 's/boot-//g' | sed 's/.tgz//g'))
else
	@$(eval SUFFIX :=)
endif

#	1. copy and rename all oftree's
	@find $(IMAGEDIR)/*$(SUFFIX)*.dtb -type f | while read oftree; do \
		_renamed=$$(echo $$(basename $${oftree}) | sed 's/wago/oftree0/g'); \
		cp $${oftree} $(IMAGE_BOOT_TMP_DIR)/$${_renamed}; \
	done

#	2. copy kernel
	@cp $(IMAGEDIR)/linuximage $(IMAGE_BOOT_TMP_DIR)/linux0;

#	3. copy setupfw + oftree

#	4. copy all additionally configured files
	@for add_file in $(IMAGE_BOOT_TAR_ADDITIONAL_FILES); do \
		cp $${add_file} $(IMAGE_BOOT_TMP_DIR); \
	done

#	5. finally generate the archive
	@cd $(LOCAL_TEMPDIR) && \
		tar cfz $(notdir $@) boot/

	@cp $(LOCAL_TEMPDIR)/$(notdir $@) $(IMAGEDIR)

	@rm -rf $(IMAGE_BOOT_TMP_DIR)

	@$(call finish)

# vim: syntax=make
