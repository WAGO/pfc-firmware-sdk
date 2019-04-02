#######################################################################################################################
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.
#
# This file is part of WAGO PFC BSP.
#
# Copyright (c) 2018
#
# Contributors:
#   PEn: WAGO Kontakttechnik GmbH & Co. KG
#######################################################################################################################

-include selected_platformconfig
-include selected_ptxconfig

# ESC sequences
ESC_DOLLAR := $$
$(ESC_DOLLAR) := $$

# Environment
BUILDTYPE ?= development
OUT_DIR ?= $(PLATFORMDIR)/images
SD_ACTIVATED=$(shell echo "$(PTXCONF_IMAGE_SD)$(PTXCONF_IMAGE_SRC_SD)" | grep --only-matching y)
RAUC_ACTIVATED=$(shell echo "$(PTXCONF_IMAGE_RAUC)")
PLATFORM ?= $(shell echo $(PTXCONF_PLATFORM))
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
#FIRMWARE_RELEASE_INDEX := 00
FIRMWARE_REVISION_MAJOR := $(shell echo "$(FIRMWARE_REVISION_STRING)" | sed -r 's/.*=//g' | head -c 2)
FIRMWARE_REVISION_MINOR := $(shell echo "$(FIRMWARE_REVISION_STRING)" | sed -r 's/.*=.{3}//g' | head -c 2)
FIRMWARE_REVISION_BUGFIX := $(shell echo "$(FIRMWARE_REVISION_STRING)" | sed -r 's/.*=.{6}//g' | head -c 2)
FIRMWARE_REVISION := $(FIRMWARE_REVISION_MAJOR)$(FIRMWARE_REVISION_MINOR)$(FIRMWARE_REVISION_BUGFIX)
FIRMWARE_SVNREVISION := $(shell cat $(TARGETROOT)/etc/SVNREVISION | head -n1 | cut -dn -f2 | cut -d@ -f1)

# SD card image
SD_IMAGE ?= $(OUT_DIR)/sd_V$(FIRMWARE_REVISION)_$(FIRMWARE_RELEASE_INDEX)_r$(FIRMWARE_SVNREVISION).img
SD_IMAGE_ORIGINAL ?= $(PLATFORMDIR)/images/sd.hdimg

# WAGO Update Package
ifeq ($(PLATFORM),wago-pfcXXX)
WUP_PLATFORM ?= PFC-Linux
else ifeq ($(PLATFORM),vtp-ctp)
WUP_PLATFORM ?= TP-Linux
else
WUP_PLATFORM ?= Unknown
endif
WUP ?= $(OUT_DIR)/$(WUP_PLATFORM)_update_V$(FIRMWARE_REVISION)_$(FIRMWARE_RELEASE_INDEX)_r$(FIRMWARE_SVNREVISION).wup
WUP_CONTROLFILE ?= $(OUT_DIR)/package-info.xml
WUP_CONTROLFILE_SCHEMA ?= $(BUILDSUPPORTDIR)/FWUPFC-Linux_1_0.xsd
export WUP_PASSWORD ?= 
WUP_ATTACHMENTS +=
WUP_FILES = $(WUP_CONTROLFILE)
WUP_FILES += $(RAUC_UPDATEFILE)
WUP_FILES += $(WUP_ATTACHMENTS)
WUP_ZIP_CMD ?= $(if $(WUP_PASSWORD),zip -j --password $($$){WUP_PASSWORD},zip -j)

# Target update files (RAUC)
RAUC_UPDATEFILE ?= $(OUT_DIR)/update_V$(FIRMWARE_REVISION)_$(FIRMWARE_RELEASE_INDEX)_r$(FIRMWARE_SVNREVISION).raucb
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

# Select targets
DIST_TARGETS ?=
ifeq ($(SD_ACTIVATED)-$(SD_IMAGE_VERSIONED),y-y)
DIST_TARGETS += sdcard
endif
ifeq ($(RAUC_ACTIVATED),y)
DIST_TARGETS += updatefile
DIST_TARGETS += wup
endif

.PHONY: dist distclean wup updatefile


dist: $(DIST_TARGETS)

sdcard: $(SD_IMAGE)

wup: $(WUP)

updatefile: $(RAUC_UPDATEFILE)

distclean: 
	   rm -f $(SD_IMAGE) \
	&& rm -f $(WUP) \
	&& rm -f $(RAUC_UPDATEFILE) \
	&& rm -f $(WUP_CONTROLFILE)

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
else
$(RAUC_UPDATEFILE): $(RAUC_UPDATEFILE_ORIGINAL) Makefile | $(OUT_DIR)
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
