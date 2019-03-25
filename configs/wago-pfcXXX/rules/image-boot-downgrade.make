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
IMAGE_PACKAGES-$(PTXCONF_IMAGE_BOOT_DOWNGRADE) += image-boot-downgrade

#
# Paths and names
#
IMAGE_BOOT_DOWNGRADE		:= image-boot-downgrade
IMAGE_BOOT_DOWNGRADE_DIR	:= $(BUILDDIR)/$(IMAGE_BOOT_DOWNGRADE)
IMAGE_BOOT_DOWNGRADE_IMAGE	:= $(IMAGEDIR)/boot-downgrade.vfat
IMAGE_BOOT_DOWNGRADE_CONFIG	:= boot-downgrade-vfat.config
IMAGE_BOOT_DOWNGRADE_ARCHIVES	:= $(call remove_quotes,$(PTXCONF_IMAGE_BOOT_DOWNGRADE_ARCHIVES))
IMAGE_BOOT_DOWNGRADE_SCRIPT	:= $(PTXDIST_PLATFORMCONFIGDIR)/firmware/downgrade_fw.sh
IMAGE_BOOT_DOWNGRADE_SUFFIX	:= $(call remove_quotes,$(PTXCONF_IMAGE_BOOT_DOWNGRADE_PLATFORM_SUFFIX))


make_vfat_image =								\
	_TMP="$(PTXDIST_TEMPDIR)/tmp_recovery";					\
	cd $(IMAGEDIR);								\
	mkdir -p $${_TMP};							\
	tar xf "$(PTXDIST_WORKSPACE)/src/$(strip $(1))" -C $${_TMP};		\
	ln -s $${_TMP}/mlo--nand;						\
	ln -s $${_TMP}/barebox.img;						\
	ln -s $${_TMP}/linuximage recovery;					\
	$(call image/genimage, IMAGE_BOOT_DOWNGRADE);				\
	_VFAT_NAME=$$(echo $(1) | sed 's/.tar.xz//g');				\
	mv $$(basename $(IMAGE_BOOT_DOWNGRADE_IMAGE)) $${_VFAT_NAME}.vfat;	\
	rm -f mlo--nand;							\
	rm -f barebox.img;							\
	rm -f recovery;								\
	rm -rf $${_TMP}

# ----------------------------------------------------------------------------
# Image
# ----------------------------------------------------------------------------

$(IMAGEDIR)/downgrade.bin:
	@$(call targetinfo)

	@if [ ! -e "$(IMAGE_BOOT_DOWNGRADE_SCRIPT)" ]; then \
		echo "ERROR: $(IMAGE_BOOT_DOWNGRADE_SCRIPT) not found"; \
		exit 1; \
	fi

	@mkdir $(PTXDIST_TEMPDIR)/tmp_lzop
	@cp $(IMAGE_BOOT_DOWNGRADE_SCRIPT) $(PTXDIST_TEMPDIR)/tmp_lzop/downgrade
	@lzop -9 -q -f -o $@.$(IMAGE_BOOT_DOWNGRADE_SUFFIX) $(PTXDIST_TEMPDIR)/tmp_lzop/downgrade

	@rm -rf $(PTXDIST_TEMPDIR)/tmp_lzop

	@$(call finish)

$(IMAGE_BOOT_DOWNGRADE_IMAGE): $(IMAGEDIR)/downgrade.bin
	@$(call targetinfo)

	@if [ -z "$(IMAGE_BOOT_DOWNGRADE_ARCHIVES)" ]; then \
		echo "ERROR: please specify archive-names that are located under '$(PTXDIST_WORKSPACE)/local_src'"; \
		exit 1; \
	fi

	@$(foreach archive, $(IMAGE_BOOT_DOWNGRADE_ARCHIVES), \
		$(call make_vfat_image, $(archive));)

	@rm -f $(IMAGEDIR)/downgrade.bin.$(IMAGE_BOOT_DOWNGRADE_SUFFIX)

	@$(call finish)

# vim: syntax=make
