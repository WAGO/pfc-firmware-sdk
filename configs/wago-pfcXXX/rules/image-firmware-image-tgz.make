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
IMAGE_PACKAGES-$(PTXCONF_IMAGE_FIRMWARE_IMAGE_TAR) += image-firmware-image-tar

#
# Paths and names
#
IMAGE_FIRMWARE_IMAGE_TAR		:= image-firmware-image-tar
IMAGE_FIRMWARE_IMAGE_TAR_DIR		:= $(BUILDDIR)/$(IMAGE_FIRMWARE_IMAGE_TAR)
IMAGE_FIRMWARE_IMAGE_TAR_PATH_IMAGE	:= $(PTXDIST_TEMPDIR)/image-firmware-image-tar-tmp
IMAGE_FIRMWARE_IMAGE_TAR_IMAGE		:= $(IMAGE_FIRMWARE_IMAGE_TAR_PATH_IMAGE)/firmware.tar
IMAGE_FIRMWARE_IMAGE_TAR_CONFIG		:= firmware.config

# ----------------------------------------------------------------------------
# Image
# ----------------------------------------------------------------------------

$(IMAGE_FIRMWARE_IMAGE_TAR_IMAGE): $(IMAGEDIR)/root.tgz $(STATEDIR)/barebox_mlo.targetinstall \
		$(STATEDIR)/barebox.targetinstall $(IMAGEDIR)/linuximage $(STATEDIR)/dtc.targetinstall \
		$(STATEDIR)/host-wago-cm-production.install $(IMAGEDIR)/boot.vfat
	@$(call targetinfo)

#	create directory with firmware related renamed files
	@$(call image/genimage, IMAGE_FIRMWARE_IMAGE_TAR)

	@cp $(HOST_WAGO_CM_PRODUCTION_DIR)/postinst $(IMAGE_FIRMWARE_IMAGE_TAR_PATH_IMAGE)
	@cp $(HOST_WAGO_CM_PRODUCTION_DIR)/util-linux-ng_*_arm.ipk $(IMAGE_FIRMWARE_IMAGE_TAR_PATH_IMAGE)
	@cp $(HOST_WAGO_CM_PRODUCTION_DIR)/e2fsprogs_*_arm.ipk $(IMAGE_FIRMWARE_IMAGE_TAR_PATH_IMAGE)
	@cp $(HOST_WAGO_CM_PRODUCTION_DIR)/format_emmc.sh $(IMAGE_FIRMWARE_IMAGE_TAR_PATH_IMAGE)
	@cp $(HOST_WAGO_CM_PRODUCTION_DIR)/mv88e6321_prepare.sh $(IMAGE_FIRMWARE_IMAGE_TAR_PATH_IMAGE)
	@cp $(HOST_WAGO_CM_PRODUCTION_DIR)/coreutils_*_arm.ipk $(IMAGE_FIRMWARE_IMAGE_TAR_PATH_IMAGE)
	@cp $(HOST_WAGO_CM_PRODUCTION_DIR)/common-functions.sh $(IMAGE_FIRMWARE_IMAGE_TAR_PATH_IMAGE)

#	now create the final firmare archiv. This archiv is renamed by wago production to *.hex
	@echo -n "Creating '$(notdir $(@))'..."

	@(  echo -n "tar -cf $@ "; \
	    echo -n "--exclude=$(notdir $@) "; \
	    echo -n "-C $(IMAGE_FIRMWARE_IMAGE_TAR_PATH_IMAGE)/ ." \
	) | $(FAKEROOT) --

	@cp $@ $(IMAGEDIR)/
	@rm -rf $(IMAGE_FIRMWARE_IMAGE_TAR_PATH_IMAGE)

	@echo "done."

	@$(call finish)

# vim: syntax=make
