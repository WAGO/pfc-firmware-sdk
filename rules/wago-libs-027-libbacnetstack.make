# -*-makefile-*-
#
# Copyright (C) 2012 by <WAGO Kontakttechnik GmbH & Co. KG>
#
# See CREDITS for details about who has contributed to this project.
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
#--- provided package --------------------------------------------------------
#
PACKAGES-$(PTXCONF_LIBBACNETSTACK) += libbacnetstack

#
#--- paths and names --------------------------------------------------------- 
#
LIBBACNETSTACK                   := libbacnetstack

ifdef PTXCONF_LIBBACNETSTACK_SOURCE_DEV
LIBBACNETSTACK_REVISION          := 22
LIBBACNETSTACK_SO_VERSION        := 1.0.0
LIBBACNETSTACK_GIT_URL           := ssh://svtfs01007:22/tfs/ProductDevelopment/_git/BACnet_Stack
LIBBACNETSTACK_FOLDER            := libbacnetstack_rev$(LIBBACNETSTACK_REVISION)
endif

ifdef PTXCONF_LIBBACNETSTACK_SOURCE_RELEASED
LIBBACNETSTACK_REVISION          := 22
LIBBACNETSTACK_SO_VERSION        := 1.0.0
LIBBACNETSTACK_BUILD_ID          := 15972403399
LIBBACNETSTACK_FOLDER            := libbacnetstack_rev$(LIBBACNETSTACK_REVISION)
endif

ifdef PTXCONF_LIBBACNETSTACK_SOURCE_LEGACY
LIBBACNETSTACK_REVISION          := 14
LIBBACNETSTACK_SO_VERSION        := 0.0.1
LIBBACNETSTACK_FOLDER            := libbacnetstack
endif



ifdef PTXCONF_LIBBACNETSTACK_SOURCE_DEV
LIBBACNETSTACK_BUILD_ID_SUFFIX      := -$(call remove_quotes,$(PTXCONF_LIBBACNETSTACK_SOURCE_DEV_BRANCH))
endif
ifdef PTXCONF_LIBBACNETSTACK_SOURCE_RELEASED
LIBBACNETSTACK_BUILD_ID_SUFFIX      := -$(LIBBACNETSTACK_BUILD_ID)
endif

LIBBACNETSTACK_REL_PATH          := wago_intern/device/bacnet/$(LIBBACNETSTACK_FOLDER)
LIBBACNETSTACK_SRC_DIR           := $(PTXDIST_WORKSPACE)/$(LIBBACNETSTACK_REL_PATH)
LIBBACNETSTACK_VERSION           := rev$(LIBBACNETSTACK_REVISION)_$(LIBBACNETSTACK_SO_VERSION)$(LIBBACNETSTACK_BUILD_ID_SUFFIX)

ifdef PTXCONF_LIBBACNETSTACK_SOURCE_RELEASED
LIBBACNETSTACK_URL               := $(call jfrog_template_to_url, LIBBACNETSTACK)
else
LIBBACNETSTACK_URL               := file://$(LIBBACNETSTACK_REL_PATH)
endif
LIBBACNETSTACK_SUFFIX            := $(suffix $(LIBBACNETSTACK_URL))
LIBBACNETSTACK_MD5                = $(shell [ -f $(LIBBACNETSTACK_MD5_FILE) ] && cat $(LIBBACNETSTACK_MD5_FILE))
LIBBACNETSTACK_MD5_FILE          := wago_intern/artifactory_sources/$(LIBBACNETSTACK)$(LIBBACNETSTACK_SUFFIX).md5
LIBBACNETSTACK_ARTIFACT           = $(call jfrog_get_filename,$(LIBBACNETSTACK_URL))
ifdef PTXCONF_LIBBACNETSTACK_SOURCE_RELEASED
LIBBACNETSTACK_ARCHIVE           := $(LIBBACNETSTACK)-$(LIBBACNETSTACK_VERSION)$(LIBBACNETSTACK_SUFFIX)
endif

LIBBACNETSTACK_BUILDROOT_DIR     := $(BUILDDIR)/$(LIBBACNETSTACK)
LIBBACNETSTACK_DIR               := $(LIBBACNETSTACK_BUILDROOT_DIR)/$(LIBBACNETSTACK)
LIBBACNETSTACK_LICENSE           := unknown

LIBBACNETSTACK_BUILDCONFIG       := Release
LIBBACNETSTACK_BUILD_DIR         := $(LIBBACNETSTACK_BUILDROOT_DIR)/bin/$(LIBBACNETSTACK_BUILDCONFIG)
LIBBACNETSTACK_LICENSE           := unknown
LIBBACNETSTACK_BIN               := $(LIBBACNETSTACK).a
LIBBACNETSTACK_CONF_TOOL         := NO
LIBBACNETSTACK_MAKE_ENV          := $(CROSS_ENV) \
              BUILDCONFIG=$(LIBBACNETSTACK_BUILDCONFIG) \
              BIN_DIR=$(LIBBACNETSTACK_BUILD_DIR) \
              TARGET_ARCH=$(PTXCONF_ARCH_STRING) \
              ARM_ARCH_VERSION=7 \
              SCRIPT_DIR=$(PTXDIST_SYSROOT_HOST)/lib/ct-build
#LIBBACNETSTACK_BUILD_ENV         := WAGO
#LIBBACNETSTACK_BUILD_ENV         := CS_LABS
LIBBACNETSTACK_PATH              := PATH=$(CROSS_PATH)
LIBBACNETSTACK_PACKAGE_NAME      := $(LIBBACNETSTACK)_$(LIBBACNETSTACK_VERSION)_$(PTXDIST_IPKG_ARCH_STRING)
LIBBACNETSTACK_PLATFORMCONFIGPACKAGEDIR := $(PTXDIST_PLATFORMCONFIGDIR)/packages


# ----------------------------------------------------------------------------
# Get
# ----------------------------------------------------------------------------

ifdef PTXCONF_LIBBACNETSTACK_SOURCE_DEV

$(LIBBACNETSTACK_SRC_DIR):
	{ cd $(PTXDIST_WORKSPACE)/wago_intern && git clone $(LIBBACNETSTACK_GIT_URL) $(LIBBACNETSTACK_SRC_DIR); } \
    || rm -fr $(LIBBACNETSTACK_SRC_DIR)
ifdef PTXCONF_LIBBACNETSTACK_SOURCE_DEV_BRANCH
	{ cd $(LIBBACNETSTACK_SRC_DIR) && git checkout $(PTXCONF_LIBBACNETSTACK_SOURCE_DEV_BRANCH); } \
    || rm -fr $(LIBBACNETSTACK_SRC_DIR)
endif

$(STATEDIR)/libbacnetstack.get: | $(LIBBACNETSTACK_SRC_DIR)

endif

ifdef PTXCONF_LIBBACNETSTACK_SOURCE_RELEASED
$(STATEDIR)/libbacnetstack.get:
	@$(call targetinfo)

ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	@${PTXDIST_WORKSPACE}/scripts/wago/artifactory.sh fetch \
    '$(LIBBACNETSTACK_URL)' wago_intern/artifactory_sources/$(LIBBACNETSTACK_ARCHIVE) '$(LIBBACNETSTACK_MD5_FILE)'
endif

	@$(call touch)
endif

# ----------------------------------------------------------------------------
# Extract
# ----------------------------------------------------------------------------


$(STATEDIR)/libbacnetstack.extract:
	@$(call targetinfo)
	@mkdir -p $(LIBBACNETSTACK_BUILDROOT_DIR)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
ifdef PTXCONF_LIBBACNETSTACK_SOURCE_RELEASED
	@mkdir -p $(LIBBACNETSTACK_DIR)
	@tar xvf wago_intern/artifactory_sources/$(LIBBACNETSTACK_ARCHIVE) -C $(LIBBACNETSTACK_DIR) --strip-components=1
	@$(call patchin, LIBBACNETSTACK)
endif
ifndef PTXCONF_LIBBACNETSTACK_SOURCE_RELEASED
	@if [ ! -L $(LIBBACNETSTACK_DIR) ]; then \
		ln -s $(LIBBACNETSTACK_SRC_DIR) $(LIBBACNETSTACK_DIR); \
	fi
endif
endif
	@$(call touch)


# ----------------------------------------------------------------------------
# Extract.post
# ----------------------------------------------------------------------------

ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
$(STATEDIR)/libbacnetstack.extract.post:
	@$(call targetinfo)
	@$(call touch)
endif

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

$(STATEDIR)/libbacnetstack.prepare:
	@$(call targetinfo)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	@$(call world/prepare, LIBBACNETSTACK)
	@$(call xslt_compile, $(LIBBACNETSTACK_DIR)/config/bacnetdiag.xml)
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------

$(STATEDIR)/libbacnetstack.compile:
	@$(call targetinfo)

ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	@$(call world/compile, LIBBACNETSTACK)
endif
	@$(call touch)


# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

$(STATEDIR)/libbacnetstack.install:
	@$(call targetinfo)

ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
#   BSP mode: install by extracting tgz file
	@mkdir -p $(LIBBACNETSTACK_PKGDIR) && \
  	tar xvzf $(LIBBACNETSTACK_PLATFORMCONFIGPACKAGEDIR)/$(LIBBACNETSTACK_PACKAGE_NAME).tgz -C $(LIBBACNETSTACK_PKGDIR)
else	
# 	normal mode, call "make install"	
	
	@$(call world/install, LIBBACNETSTACK)
	
ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_RELEASE
#   # save install directory to tgz for BSP mode
	@mkdir -p $(LIBBACNETSTACK_PLATFORMCONFIGPACKAGEDIR)
	@cd $(LIBBACNETSTACK_PKGDIR) && tar cvzf $(LIBBACNETSTACK_PLATFORMCONFIGPACKAGEDIR)/$(LIBBACNETSTACK_PACKAGE_NAME).tgz *
endif
endif

	@$(call touch)


# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/libbacnetstack.targetinstall:
	@$(call targetinfo)
	@$(call install_init, libbacnetstack)	

	@$(call install_fixup, libbacnetstack,PRIORITY,optional)
	@$(call install_fixup, libbacnetstack,SECTION,base)
	@$(call install_fixup, libbacnetstack,AUTHOR,"Andreas Hiebert <andreas.hiebert@wago.com>")
	@$(call install_fixup, libbacnetstack,DESCRIPTION,"BACnet stack for the PFC controller" )

	@$(call install_copy, libbacnetstack, ${PTXCONF_ROOTFS_PASSWD_USER_UID}, ${PTXCONF_ROOTFS_PASSWD_USER_GID}, 0755, /home/user)
	@$(call install_copy, libbacnetstack, 0, 0, 0755, /home/user/bacnet)

	@$(call install_finish, libbacnetstack)

	@$(call touch)

# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------

PTX_ROOT=$(PTXCONF_SYSROOT_TARGET)/../root
PTX_DAL=$(PTXCONF_SYSROOT_TARGET)/usr/lib/dal

$(STATEDIR)/libbacnetstack.clean:
	@$(call targetinfo)
	@if [ -d $(LIBBACNETSTACK_DIR) ]; then \
	  $(LIBBACNETSTACK_MAKE_ENV) $(LIBBACNETSTACK_PATH) $(MAKE) $(MFLAGS) -C $(LIBBACNETSTACK_DIR) clean; \
	fi
	@$(call clean_pkg, LIBBACNETSTACK)
	@rm -rf $(LIBBACNETSTACK_BUILDROOT_DIR)
	@echo Deleting default bacnet folder:	
	@echo $(PTXDIST_PLATFORMDIR)/root/home/user/bacnet
	@rm -rf $(PTXDIST_PLATFORMDIR)/root/home/user/bacnet
	@echo $(PTXCONF_SYSROOT_TARGET)/home/user/bacnet
	@rm -rf $(PTXCONF_SYSROOT_TARGET)/home/user/bacnet
	
# vim: syntax=make

