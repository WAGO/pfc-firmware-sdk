#######################################################################################################################
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.
#
# This file is part of WAGO PFC BSP.
#
# Copyright (c) 2018-2019 WAGO Kontakttechnik GmbH & Co. KG
#
# Contributors:
#   PEn: WAGO Kontakttechnik GmbH & Co. KG
#   AGa: WAGO Kontakttechnik GmbH & Co. KG
#######################################################################################################################

-include selected_platformconfig
-include selected_ptxconfig

# ESC sequences
ESC_DOLLAR := $$
$(ESC_DOLLAR) := $$

# Environment
BUILDTYPE ?= development
IMAGE_DIR ?= $(PLATFORMDIR)/images
OUT_DIR ?= $(IMAGE_DIR)
SD_ACTIVATED = $(shell echo "$(PTXCONF_IMAGE_SD)$(PTXCONF_IMAGE_SRC_SD)" | grep --only-matching y)
PRODUCTION_ACTIVATED = $(shell echo "$(PTXCONF_HOST_WAGO_CM_PRODUCTION)")
DOWNGRADE_ACTIVATED = $(shell echo "$(PTXCONF_IMAGE_SD_DOWNGRADE)")
RAUC_ACTIVATED = $(shell echo "$(PTXCONF_IMAGE_RAUC)")
PLATFORM ?= $(shell echo $(PTXCONF_PLATFORM))
KERNEL_VERSION ?= $(shell echo $(PTXCONF_KERNEL_VERSION))
PLATFORMDIR ?= platform-$(PLATFORM)
TARGETROOT ?= $(shell ptxdist print ROOTDIR)
PLATFORM_PROJECTROOT ?= projectroot.$(PLATFORM)
BUILDSUPPORTDIR ?= shared_public/build
XMLSTARLET ?= xmlstarlet
WUP_CONTROLFILE_GENERATOR ?= shared_public/build/create_wup_controlfile.sh

#Firmware information
FIRMWARE_REVISION_STRING := $(shell cat $(TARGETROOT)/etc/REVISIONS)
FIRMWARE_RELEASE_INDEX_REGEX = s/.*\(//g
FIRMWARE_RELEASE_INDEX := $(shell echo "$(FIRMWARE_REVISION_STRING)" | sed -r '$(FIRMWARE_RELEASE_INDEX_REGEX)' | head -c 2)
FIRMWARE_REVISION_MAJOR := $(shell echo "$(FIRMWARE_REVISION_STRING)" | sed -r 's/.*=//g' | head -c 2)
FIRMWARE_REVISION_MINOR := $(shell echo "$(FIRMWARE_REVISION_STRING)" | sed -r 's/.*=.{3}//g' | head -c 2)
FIRMWARE_REVISION_BUGFIX := $(shell echo "$(FIRMWARE_REVISION_STRING)" | sed -r 's/.*=.{6}//g' | head -c 2)
FIRMWARE_REVISION := $(FIRMWARE_REVISION_MAJOR)$(FIRMWARE_REVISION_MINOR)$(FIRMWARE_REVISION_BUGFIX)
FIRMWARE_SVNREVISION := $(shell cat $(TARGETROOT)/etc/SVNREVISION | head -n1 | cut -dn -f2 | cut -d@ -f1)

# Map platform names to more user friendly target names
ifeq ($(PLATFORM),wago-pfcXXX)
FIRMWARE_PLATFORM ?= PFC-Linux
else ifeq ($(PLATFORM),wago-pfcXXX-hardened)
FIRMWARE_PLATFORM ?= PFC-Linux
else ifeq ($(PLATFORM),vtp-ctp)
FIRMWARE_PLATFORM ?= TP-Linux
else
FIRMWARE_PLATFORM ?= $(PLATFORM)
endif

# Commonly used image ID build from firmware information
ifeq ($(BUILDTYPE),release)
IMAGE_ID=V$(FIRMWARE_REVISION)_$(FIRMWARE_RELEASE_INDEX)_r$(FIRMWARE_SVNREVISION)
else
IMAGE_ID=V$(FIRMWARE_REVISION)_$(FIRMWARE_RELEASE_INDEX)_r$(FIRMWARE_SVNREVISION)_development
endif

# SD card image
SD_IMAGE ?= $(OUT_DIR)/$(FIRMWARE_PLATFORM)_sd_$(IMAGE_ID).img
SD_IMAGE_ORIGINAL ?= $(PLATFORMDIR)/images/sd.hdimg

# WAGO Update Package
WUP ?= $(OUT_DIR)/$(FIRMWARE_PLATFORM)_update_$(IMAGE_ID).wup
WUP_CONTROLFILE ?= $(OUT_DIR)/package-info.xml
WUP_CONTROLFILE_SCHEMA ?= $(BUILDSUPPORTDIR)/FWUPFC-Linux_1_0.xsd
export WUP_PASSWORD ?= 
WUP_ATTACHMENTS +=
WUP_FILES = $(WUP_CONTROLFILE)
WUP_FILES += $(RAUC_UPDATEFILE)
WUP_FILES += $(WUP_ATTACHMENTS)
WUP_ZIP_CMD ?= $(if $(WUP_PASSWORD),zip -j --password $($$){WUP_PASSWORD},zip -j)

# Target update file (RAUC)
RAUC_UPDATEFILE ?= $(OUT_DIR)/$(FIRMWARE_PLATFORM)_update_$(IMAGE_ID).raucb
RAUC_UPDATEFILE_LEGACY ?= $(OUT_DIR)/update_$(IMAGE_ID).raucb
RAUC_UPDATEFILE_ORIGINAL ?= $(PLATFORMDIR)/images/update.raucb

# RAUC specific files
RAUC_HOST_FOLDER = $(shell ptxdist print RAUC)
ifeq ($(BUILDTYPE),release)
RAUC_KEY ?= $(shell echo $(PTXCONF_RAUC_RELEASE_KEY))
RAUC_CERTIFICATE ?= $(shell echo $(PTXCONF_RAUC_RELEASE_CERT))
else
RAUC_KEY ?= $(shell echo $(PTXCONF_RAUC_DEVELOPMENT_KEY))
RAUC_CERTIFICATE ?= $(shell echo $(PTXCONF_RAUC_DEVELOPMENT_CERT))
endif
RAUC_KEYRING ?= $(shell echo $(PTXCONF_RAUC_DEVELOPMENT_KEYRING))
RAUC_CMD ?= $(PLATFORMDIR)/build-host/$(RAUC_HOST_FOLDER)/rauc

# Downgrade images
DOWNGRADE_IMAGES += $(OUT_DIR)/sd-downgrade-firmware-02-pfc200_$(IMAGE_ID).img
DOWNGRADE_IMAGES += $(OUT_DIR)/sd-downgrade-firmware-03-pfc200_$(IMAGE_ID).img
DOWNGRADE_IMAGES += $(OUT_DIR)/sd-downgrade-firmware-04-pfc200_$(IMAGE_ID).img

# Production images
PRODUCTION_IMAGES += $(OUT_DIR)/nand-wago-production-pfc100_$(IMAGE_ID).ubi
PRODUCTION_IMAGES += $(OUT_DIR)/nand-wago-production-pfc200_$(IMAGE_ID).img
PRODUCTION_IMAGES += $(OUT_DIR)/nand-wago-production-pfc200v2_$(IMAGE_ID).ubi
PRODUCTION_IMAGES += $(OUT_DIR)/emmc-wago-production-pfc200v3_$(IMAGE_ID).img
PRODUCTION_IMAGES += $(OUT_DIR)/emmc-commission-pfc200v3_$(IMAGE_ID).img
PRODUCTION_IMAGES += $(OUT_DIR)/firmware_$(IMAGE_ID).hex

ifeq ($(BUILDTYPE),release)
PRODUCTION_IMAGES += $(OUT_DIR)/vmlinux_$(IMAGE_ID)
endif

# Firmware description files
FW_DESC_FILES += $(OUT_DIR)/SVNREVISION
FW_DESC_FILES += $(OUT_DIR)/REVISIONS

# Archives
IMAGES_ARCHIVE += $(OUT_DIR)/images_$(IMAGE_ID).tar.gz
ROOT_DEBUG_ARCHIVE += $(OUT_DIR)/root-debug_$(IMAGE_ID).tar.gz

# Obsolete image files
ifneq ($(BUILDTYPE),release)
OBSOLETE_FILES += $(OUT_DIR)/vmlinux_$(IMAGE_ID)_obsolet
OBSOLETE_FILES += $(OUT_DIR)/firmware_$(IMAGE_ID)_obsolet.hex
OBSOLETE_FILES += $(OUT_DIR)/sd_$(IMAGE_ID)_obsolete.img
endif

# Select default dist targets
DIST_TARGETS ?=
DIST_TARGETS += $(FW_DESC_FILES)
ifeq ($(SD_ACTIVATED),y)
DIST_TARGETS += $(SD_IMAGE)
endif
ifeq ($(PRODUCTION_ACTIVATED),y)
DIST_TARGETS += $(PRODUCTION_IMAGES)
endif
ifeq ($(DOWNGRADE_ACTIVATED),y)
DIST_TARGETS += $(DOWNGRADE_IMAGES)
endif
ifeq ($(RAUC_ACTIVATED),y)
DIST_TARGETS += $(RAUC_UPDATEFILE)
DIST_TARGETS += $(RAUC_UPDATEFILE_LEGACY)
DIST_TARGETS += $(WUP_CONTROLFILE)
DIST_TARGETS += $(WUP)
endif
ifneq ($(IMAGE_DIR),$(OUT_DIR))
# Don't create archives when executed locally
DIST_TARGETS += $(IMAGES_ARCHIVE)
DIST_TARGETS += $(ROOT_DEBUG_ARCHIVE)
endif

#DIST_TARGETS += $(OBSOLETE_FILES)


# Main targets
#######################################################################################################################
.PHONY: dist distclean sdcard wup updatefile updatepack production_images downgrade_images fw_desc_files


dist: $(DIST_TARGETS)

sdcard: $(SD_IMAGE)

wup: $(WUP)

updatefile: $(RAUC_UPDATEFILE)

production_images: $(PRODUCTION_IMAGES)

downgrade_images: $(DOWNGRADE_IMAGES)

fw_desc_files: $(FW_DESC_FILES)

distclean:
	   rm -f $(DIST_TARGETS) \
	&& rm -f $(OUT_DIR)/ptxdist.images.stage


# Target rules
#######################################################################################################################

#
# !!! ATTENTION !!!
#
# ptxdist.images.stage is used by Jenkins to determine if the artifacts are renamed by this
# Makefile. If it is not found, Jenkins falls back to his deprecated internal implementation
# as it assumes a legacy fw version to be present.
# Changes to this target have to be sync-ed with Jenkins Shared Library.
# Please contact DevTools group for details.
$(OUT_DIR)/ptxdist.images.stage:
	touch $@

$(OUT_DIR)/%: $(TARGETROOT)/etc/% | $(OUT_DIR)
	@echo "Extract from target root directory: $<"
	cp $< $@

$(OUT_DIR)/%_$(IMAGE_ID).img: $(IMAGE_DIR)/%.img | $(OUT_DIR)
	@echo "Create versioned image by copy: $<"
	cp $< $@

$(OUT_DIR)/%_$(IMAGE_ID).img: $(IMAGE_DIR)/%.hdimg | $(OUT_DIR)
	@echo "Create versioned image by copy: $<"
	cp $< $@

$(OUT_DIR)/%_$(IMAGE_ID).ubi: $(IMAGE_DIR)/%.ubi | $(OUT_DIR)
	@echo "Create versioned image by copy: $<"
	cp $< $@

#$(OUT_DIR)/%_$(IMAGE_ID)_obsolete.img: $(IMAGE_DIR)/%.hdimg | $(OUT_DIR)
#	@echo "Create versioned image by copy: $<"
#	cp $< $@

#$(OUT_DIR)/firmware_$(IMAGE_ID)_obsolet.hex: $(IMAGE_DIR)/firmware.tar | $(OUT_DIR)
#	@echo "Create versioned image by copy: $<"
#	cp $< $@
	
#$(OUT_DIR)/vmlinux_$(IMAGE_ID)_obsolet: $(PLATFORMDIR)/build-target/linux-$(KERNEL_VERSION)/vmlinux | $(OUT_DIR)
#	@echo "Create versioned kernel image by copy: $<"
#	cp $< $@

$(OUT_DIR)/firmware_$(IMAGE_ID).hex: $(IMAGE_DIR)/firmware.tar | $(OUT_DIR)
	@echo "Create versioned image by copy: $<"
	cp $< $@
	
$(OUT_DIR)/vmlinux_$(IMAGE_ID): $(PLATFORMDIR)/build-target/linux-$(KERNEL_VERSION)/vmlinux | $(OUT_DIR)
	@echo "Create versioned kernel image by copy: $<"
	cp $< $@

$(IMAGES_ARCHIVE): $(OUT_DIR)/ptxdist.images.stage | $(OUT_DIR)
	@echo "Create image archive"
	tar -cvzf $@ -C $(IMAGE_DIR) .

$(ROOT_DEBUG_ARCHIVE): $(OUT_DIR)/ptxdist.images.stage | $(OUT_DIR)
	@echo "Create debug root archive"
	tar -cvzf $@ -C $(TARGETROOT) .

$(SD_IMAGE): $(SD_IMAGE_ORIGINAL) Makefile | $(OUT_DIR)
	@echo "Create versioned SD card image by copy"
	cp $< $@

$(WUP): $(WUP_FILES) Makefile | $(OUT_DIR)
	@echo "Create WAGO update package \"$@\""
	   rm -f $@ \
	&& $(WUP_ZIP_CMD) $@ $(WUP_FILES)

ifeq ($(BUILDTYPE),release)
$(RAUC_UPDATEFILE): $(RAUC_UPDATEFILE_ORIGINAL) $(RAUC_CERTIFICATE) $(RAUC_KEY) $(RAUC_KEYRING) Makefile | $(OUT_DIR)
	@echo "Create RAUC update file \"$@\" for build type $(BUILDTYPE) by resign with key \"$(RAUC_KEY)\""
	   rm -f $@ \
	&& $(RAUC_CMD) resign --cert=$(RAUC_CERTIFICATE) --key=$(RAUC_KEY) --keyring=$(RAUC_KEYRING) $< $@
$(RAUC_UPDATEFILE_LEGACY): $(RAUC_UPDATEFILE_ORIGINAL) $(RAUC_CERTIFICATE) $(RAUC_KEY) $(RAUC_KEYRING) Makefile | $(OUT_DIR)
	@echo "Create RAUC update file \"$@\" for build type $(BUILDTYPE) by resign with key \"$(RAUC_KEY)\""
	   rm -f $@ \
	&& $(RAUC_CMD) resign --cert=$(RAUC_CERTIFICATE) --key=$(RAUC_KEY) --keyring=$(RAUC_KEYRING) $< $@
else
$(RAUC_UPDATEFILE): $(RAUC_UPDATEFILE_ORIGINAL) Makefile | $(OUT_DIR)
	@echo "Create RAUC update file \"$@\" for build type $(BUILDTYPE) by copy"
	cp $< $@
$(RAUC_UPDATEFILE_LEGACY): $(RAUC_UPDATEFILE_ORIGINAL) Makefile | $(OUT_DIR)
	@echo "Create RAUC update file \"$@\" for build type $(BUILDTYPE) by copy"
	cp $< $@
endif

$(WUP_CONTROLFILE): $(WUP_CONTROLFILE_SCHEMA) $(WUP_CONTROLFILE_GENERATOR) $(RAUC_UPDATEFILE) Makefile | $(OUT_DIR)
	@echo "Create WAGO update package control file \"$@\" and validate against schema \"$(WUP_CONTROLFILE_SCHEMA)\""
	$(WUP_CONTROLFILE_GENERATOR) $@ $(FIRMWARE_REVISION_MAJOR)\
	                                $(FIRMWARE_REVISION_MINOR)\
	                                $(FIRMWARE_REVISION_BUGFIX)\
	                                $(FIRMWARE_RELEASE_INDEX)\
	                                $(RAUC_UPDATEFILE)\
	                                $(PLATFORM)\
	&& $(XMLSTARLET) validate --xsd $< $@

$(OUT_DIR):
	mkdir -p $@


# Error messages explaining preconditions
#######################################################################################################################
selected_platformconfig:
	$(error PTXdist platform not configured)

$(SD_IMAGE_ORIGINAL):
	$(error SD card image file was not build (under "$(SD_IMAGE_ORIGINAL)") or provided via path in variable SD_IMAGE_ORIGINAL)

$(RAUC_UPDATEFILE_ORIGINAL):
	$(error RAUC update file was not build (under "$(RAUC_UPDATEFILE_ORIGINAL)") or provided via path in variable RAUC_UPDATEFILE_ORIGINAL)

$(WUP_CONTROLFILE_SCHEMA):
	$(error WUP control file schema not provided (under "$(WUP_CONTROLFILE_SCHEMA)" or via path in variable WUP_CONTROLFILE_SCHEMA))

$(RAUC_CERTIFICATE):
	$(error RAUC certificate file not provided (under "$(RAUC_CERTIFICATE)" or via path in variable RAUC_CERTIFICATE))

$(RAUC_KEY):
	$(error RAUC key file not provided (under "$(RAUC_KEY)" or via path in variable RAUC_KEY))

ifneq ($(RAUC_CERTIFICATE),$(RAUC_KEYRING))
$(RAUC_KEYRING):
	$(error RAUC keyring file not provided (under "$(RAUC_KEYRING)" or via path in variable RAUC_KEYRING))
endif
