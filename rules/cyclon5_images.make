# -*-makefile-*-
#
# Copyright (C) 2019 by WAGO Kontakttechnik GmbH
#
# See CREDITS for details about who has contributed to this project.
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
PACKAGES-$(PTXCONF_CYCLON5_IMAGES) += cyclon5_images

#
# Paths and names
#

CYCLON5_IMAGES_SUFFIX		:= .tar.gz
CYCLON5_IMAGES_REPO		:= $(call remove_quotes, ${PTXCONF_CYCLON5_IMAGES_REPO})
ARTIFACTORY_BASE_URL		:= $(call remove_quotes, ${PTXCONF_ARTIFACTORY_BASE_URL})
CYCLON5_IMAGES_FPGA_IMAGE	:= $(call remove_quotes, ${PTXCONF_CYCLON5_IMAGES_FPGA_IMAGE})
CYCLON5_IMAGES_REPO_NAME	:= $(call remove_quotes, ${PTXCONF_CYCLON5_IMAGES_REPO_NAME})
CYCLON5_IMAGES_VERSION		:= $(call remove_quotes, ${PTXCONF_CYCLON5_IMAGES_VERSION})
CYCLON5_IMAGES_URL		:= ${ARTIFACTORY_BASE_URL}/${CYCLON5_IMAGES_REPO}/wago/pfc8230/${CYCLON5_IMAGES_REPO_NAME}/
ifdef PTXCONF_CYCLON5_IMAGES
	ifndef CYCLON5_IMAGES_VERSION
		#Always get the latest version
		CYCLON5_IMAGES_VERSION  := $(shell curl -s -H "X-JFrog-Art-API:${JFROG_APIKEY}" "${CYCLON5_IMAGES_URL}" | grep -Po '(?<=href="${CYCLON5_IMAGES_REPO_NAME}-)[^"]*(?=\${CYCLON5_IMAGES_SUFFIX}")' | tail -1)
	endif
endif
CYCLON5_IMAGES_URL      := $(CYCLON5_IMAGES_URL)$(CYCLON5_IMAGES_REPO_NAME)-$(CYCLON5_IMAGES_VERSION)$(CYCLON5_IMAGES_SUFFIX)
CYCLON5_IMAGES          := cyclon5_images-$(CYCLON5_IMAGES_VERSION)
CYCLON5_IMAGES_FPGA_SYMLINK := cyclon5.img
CYCLON5_IMAGES_CPU_ELF_FILE := c5sxpfc8230hw4pnio1.elf
CYCLON5_IMAGES_CPU_ELF_SYMLINK := rproc-cyc5_rproc-fw

#CYCLON5_IMAGES_VERSION_HEADER := "rlb_version.h"

CYCLON5_IMAGES_DIR            := $(BUILDDIR)/$(CYCLON5_IMAGES)
CYCLON5_IMAGES_MD5             = $(shell [ -f $(CYCLON5_IMAGES_MD5_FILE) ] && cat $(CYCLON5_IMAGES_MD5_FILE))
CYCLON5_IMAGES_SOURCE_BASEDIR := wago_intern/artifactory_sources
CYCLON5_IMAGES_MD5_FILE       := ${CYCLON5_IMAGES_SOURCE_BASEDIR}/$(CYCLON5_IMAGES)$(CYCLON5_IMAGES_SUFFIX).md5
CYCLON5_IMAGES_LICENSE        := proprietary

CYCLON5_IMAGES_PACKAGE_NAME := cyclon5_binaries_$(PTXDIST_IPKG_ARCH_STRING)
CYCLON5_IMAGES_PLATFORMCONFIGPACKAGEDIR := $(PTXDIST_PLATFORMCONFIGDIR)/packages


# ----------------------------------------------------------------------------
# Get
# ----------------------------------------------------------------------------

$(STATEDIR)/cyclon5_images.get:
	@$(call targetinfo)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_IMAGES
	${PTXDIST_WORKSPACE}/scripts/wago/artifactory.sh fetch \
	'$(CYCLON5_IMAGES_URL)' ${CYCLON5_IMAGES_SOURCE_BASEDIR}/$(CYCLON5_IMAGES)$(CYCLON5_IMAGES_SUFFIX) '$(CYCLON5_IMAGES_MD5_FILE)'
endif

	@$(call touch)

# ----------------------------------------------------------------------------
# Extract
# ----------------------------------------------------------------------------

$(STATEDIR)/cyclon5_images.extract:
	@$(call targetinfo)

ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_IMAGES
	@mkdir -p $(CYCLON5_IMAGES_DIR) && \
	tar xvzf ${CYCLON5_IMAGES_SOURCE_BASEDIR}/$(CYCLON5_IMAGES)$(CYCLON5_IMAGES_SUFFIX) -C $(CYCLON5_IMAGES_DIR) --strip-components=1 > /dev/null 2>&1
	@$(call patchin, CYCLON5_IMAGES)
endif
	@$(call touch)

ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_IMAGES
$(STATEDIR)/cyclon5_images.extract.post:
	@$(call targetinfo)
	@$(call touch)
endif

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

$(STATEDIR)/cyclon5_images.prepare:
	@$(call targetinfo)
	@$(call touch)

# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------

$(STATEDIR)/cyclon5_images.compile:
	@$(call targetinfo)
	@$(call touch)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

$(STATEDIR)/cyclon5_images.install:
	@$(call targetinfo)
	@$(call touch)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/cyclon5_images.targetinstall:
	@$(call targetinfo)

	@$(call install_init, cyclon5_images)
	@$(call install_fixup, cyclon5_images, PRIORITY, optional)
	@$(call install_fixup, cyclon5_images, SECTION, base)
	@$(call install_fixup, cyclon5_images, AUTHOR, "WAGO Kontakttechnik GmbH")
	@$(call install_fixup, cyclon5_images, DESCRIPTION, missing)
	@mkdir -p $(IMAGEDIR)/img_pnio/;
	@for i in $(shell cd  $(CYCLON5_IMAGES_DIR)/$(CYCLON5_IMAGES_REPO_NAME)_debug && find $(PTXCONF_CYCLON5_INIT_IMAGES)* -type f); do \
		$(call install_copy, cyclon5_images, 0, 0, 0755, $(CYCLON5_IMAGES_DIR)/$(CYCLON5_IMAGES_REPO_NAME)_debug/$$i, /boot/img_pnio/$$i); \
	done
	@for i in $(shell cd  $(CYCLON5_IMAGES_DIR)/$(CYCLON5_IMAGES_REPO_NAME)_debug && find $(PTXCONF_CYCLON5_INIT_IMAGES)* -type f); do \
		cp $(CYCLON5_IMAGES_DIR)/$(CYCLON5_IMAGES_REPO_NAME)_debug/$$i $(IMAGEDIR)/img_pnio/$$i; \
	done
	@$(call install_copy, cyclon5_images, 0, 0, 0644, $(CYCLON5_IMAGES_DIR)/$(CYCLON5_IMAGES_REPO_NAME)_debug/$(PTXCONF_CYCLON5_IMAGES_FPGA_IMAGE), /boot/$(PTXCONF_CYCLON5_IMAGES_FPGA_IMAGE))
	@$(call install_copy, cyclon5_images, 0, 0, 0644, $(CYCLON5_IMAGES_DIR)/$(CYCLON5_IMAGES_REPO_NAME)_debug/$(CYCLON5_IMAGES_CPU_ELF_FILE), /usr/lib/firmware/$(CYCLON5_IMAGES_CPU_ELF_FILE))
	@$(call install_link, cyclon5_images, /usr/lib/firmware/$(CYCLON5_IMAGES_CPU_ELF_FILE), /usr/lib/firmware/$(CYCLON5_IMAGES_CPU_ELF_SYMLINK))
	@cp $(CYCLON5_IMAGES_DIR)/$(CYCLON5_IMAGES_REPO_NAME)_debug/$(PTXCONF_CYCLON5_IMAGES_FPGA_IMAGE) $(IMAGEDIR)/$(CYCLON5_IMAGES_FPGA_SYMLINK)
	@$(call install_link, cyclon5_images, $(PTXCONF_CYCLON5_IMAGES_FPGA_IMAGE), /boot/$(CYCLON5_IMAGES_FPGA_SYMLINK))

	#Workaround
	@$(call install_finish, cyclon5_images)

	@$(call touch)

# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------

$(STATEDIR)/cyclon5_images.clean:
	@$(call targetinfo)
	@$(call clean_pkg, CYCLON5_IMAGES)
	@rm -f $(ROOTDIR)/usr/lib/firmware/$(CYCLON5_IMAGES_CPU_ELF_SYMLINK)
	@rm -f $(ROOTDIR)/usr/lib/firmware/$(CYCLON5_IMAGES_CPU_ELF_FILE)
# vim: syntax=make
