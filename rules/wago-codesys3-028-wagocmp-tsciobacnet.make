## -*-makefile-*-
# $Id: wago-codesys3-024-tsciobacnet.make 7242 2014-06-24 15:44:38Z joerg.bolte@wago.com $
#
# Copyright (C) 2016 by WAGO GmbH & Co. KG
#
# See CREDITS for details about who has contributed to this project.
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
PACKAGES-$(PTXCONF_CDS3_TSCIOBACNET) += cds3-tsciobacnet
#
#--- paths and names --------------------------------------------------------- 
#
CDS3_TSCIOBACNET                 := TscIoBacnet
CDS3_TSCIOBACNET_NAME       		 := TscIoBacnet
CDS3_TSCIOBACNET_SO_VERSION      := 1.5.0

ifdef PTXCONF_CDS3_TSCIOBACNET_SOURCE_DEV
CDS3_TSCIOBACNET_GIT_URL         := ssh://svtfs01007:22/tfs/ProductDevelopment/BACnet_Stack/_git/TscIoBacnet
CDS3_TSCIOBACNET_FOLDER          := TscIoBacnet_git
endif

ifdef PTXCONF_CDS3_TSCIOBACNET_SOURCE_RELEASED
CDS3_TSCIOBACNET_BUILD_ID        := 16515712062
CDS3_TSCIOBACNET_FOLDER          := TscIoBacnet_git
endif


ifdef PTXCONF_CDS3_TSCIOBACNET_SOURCE_DEV
CDS3_TSCIOBACNET_BUILD_ID_SUFFIX := -$(call remove_quotes,$(PTXCONF_CDS3_TSCIOBACNET_SOURCE_DEV_BRANCH))
endif
ifdef PTXCONF_CDS3_TSCIOBACNET_SOURCE_RELEASED
CDS3_TSCIOBACNET_BUILD_ID_SUFFIX := -$(CDS3_TSCIOBACNET_BUILD_ID)
endif

CDS3_TSCIOBACNET_REL_PATH        := wago_intern/codesys3-Component/$(CDS3_TSCIOBACNET_FOLDER)
CDS3_TSCIOBACNET_SRC_DIR         := $(PTXDIST_WORKSPACE)/$(CDS3_TSCIOBACNET_REL_PATH)
CDS3_TSCIOBACNET_VERSION         := $(CDS3_TSCIOBACNET_SO_VERSION)$(CDS3_TSCIOBACNET_BUILD_ID_SUFFIX)
CDS3_TSCIOBACNET_ENV_VENDOR      := WAGO

ifdef PTXCONF_CDS3_TSCIOBACNET_SOURCE_RELEASED
CDS3_TSCIOBACNET_URL             := $(call jfrog_template_to_url, CDS3_TSCIOBACNET)
else
CDS3_TSCIOBACNET_URL             := file://$(CDS3_TSCIOBACNET_REL_PATH)
endif
CDS3_TSCIOBACNET_SUFFIX          := $(suffix $(CDS3_TSCIOBACNET_URL))
CDS3_TSCIOBACNET_MD5              = $(shell [ -f $(CDS3_TSCIOBACNET_MD5_FILE) ] && cat $(CDS3_TSCIOBACNET_MD5_FILE))
CDS3_TSCIOBACNET_MD5_FILE        := wago_intern/artifactory_sources/$(CDS3_TSCIOBACNET)$(CDS3_TSCIOBACNET_SUFFIX).md5
CDS3_TSCIOBACNET_ARTIFACT         = $(call jfrog_get_filename,$(CDS3_TSCIOBACNET_URL))
ifdef PTXCONF_CDS3_TSCIOBACNET_SOURCE_RELEASED
CDS3_TSCIOBACNET_ARCHIVE         := $(CDS3_TSCIOBACNET)-$(CDS3_TSCIOBACNET_VERSION)$(CDS3_TSCIOBACNET_SUFFIX)
endif

CDS3_TSCIOBACNET_BUILDROOT_DIR   := $(BUILDDIR)/$(CDS3_TSCIOBACNET)
CDS3_TSCIOBACNET_DIR             := $(CDS3_TSCIOBACNET_BUILDROOT_DIR)/$(CDS3_TSCIOBACNET)
CDS3_TSCIOBACNET_LICENSE         := unknown

CDS3_TSCIOBACNET_BUILDCONFIG     := Release
CDS3_TSCIOBACNET_BUILD_DIR       := $(CDS3_TSCIOBACNET_BUILDROOT_DIR)/bin/$(CDS3_TSCIOBACNET_BUILDCONFIG)
CDS3_TSCIOBACNET_BIN             := lib$(CDS3_TSCIOBACNET).so.$(CDS3_TSCIOBACNET_SO_VERSION)
CDS3_TSCIOBACNET_SO_NAME         := $(CDS3_TSCIOBACNET).so
CDS3_TSCIOBACNET_CONF_TOOL       := NO
CDS3_TSCIOBACNET_MAKE_ENV        := $(CROSS_ENV) \
BUILDCONFIG=$(LIBBACNETSTACK_BUILDCONFIG_BUILDCONFIG) \
              BIN_DIR=$(LIBBACNETSTACK_BUILDCONFIG_BUILD_DIR) \
              TARGET_ARCH=$(PTXCONF_ARCH_STRING) \
              ARM_ARCH_VERSION=7 \
              SCRIPT_DIR=$(PTXDIST_SYSROOT_HOST)/lib/ct-build

CDS3_TSCIOBACNET_PATH            := PATH=$(CROSS_PATH)
CDS3_TSCIOBACNET_PACKAGE_NAME    := cds3-tsciobacnet_$(CDS3_TSCIOBACNET_VERSION)_$(PTXDIST_IPKG_ARCH_STRING)
CDS3_TSCIOBACNET_PACKAGE_NAME_CP    := cds3-tsciobacnet_$(CDS3_TSCIOBACNET_SO_VERSION)+$(CDS3_TSCIOBACNET_BUILD_ID)_$(PTXDIST_IPKG_ARCH_STRING)
CDS3_TSCIOBACNET_PLATFORMCONFIGPACKAGEDIR := $(PTXDIST_PLATFORMCONFIGDIR)/packages


# ----------------------------------------------------------------------------
# Get
# ----------------------------------------------------------------------------

ifdef PTXCONF_CDS3_TSCIOBACNET_SOURCE_DEV

$(CDS3_TSCIOBACNET_SRC_DIR):
	{ cd $(PTXDIST_WORKSPACE)/wago_intern && git clone $(CDS3_TSCIOBACNET_GIT_URL) $(CDS3_TSCIOBACNET_SRC_DIR); } \
    || rm -fr $(CDS3_TSCIOBACNET_SRC_DIR)
ifdef PTXCONF_CDS3_TSCIOBACNET_SOURCE_DEV_BRANCH
	{ cd $(CDS3_TSCIOBACNET_SRC_DIR) && git checkout $(PTXCONF_CDS3_TSCIOBACNET_SOURCE_DEV_BRANCH); } \
    || rm -fr $(CDS3_TSCIOBACNET_SRC_DIR)
endif

$(STATEDIR)/cds3-tsciobacnet.get: | $(CDS3_TSCIOBACNET_SRC_DIR)

endif

ifdef PTXCONF_CDS3_TSCIOBACNET_SOURCE_RELEASED
$(STATEDIR)/cds3-tsciobacnet.get:
	@$(call targetinfo)

ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	@${PTXDIST_WORKSPACE}/scripts/wago/artifactory.sh fetch \
    '$(CDS3_TSCIOBACNET_URL)' wago_intern/artifactory_sources/$(CDS3_TSCIOBACNET_ARCHIVE) '$(CDS3_TSCIOBACNET_MD5_FILE)'
endif

	@$(call touch)
endif

# ----------------------------------------------------------------------------
# Extract
# ----------------------------------------------------------------------------

$(STATEDIR)/cds3-tsciobacnet.extract:
	@$(call targetinfo)
	@mkdir -p $(CDS3_TSCIOBACNET_BUILDROOT_DIR)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
ifdef PTXCONF_CDS3_TSCIOBACNET_SOURCE_RELEASED
	@mkdir -p $(CDS3_TSCIOBACNET_DIR)
	@tar xvf wago_intern/artifactory_sources/$(CDS3_TSCIOBACNET_ARCHIVE) -C $(CDS3_TSCIOBACNET_DIR) --strip-components=1
	@$(call patchin, CDS3_TSCIOBACNET)
endif
ifndef PTXCONF_CDS3_TSCIOBACNET_SOURCE_RELEASED
	@if [ ! -L $(CDS3_TSCIOBACNET_DIR) ]; then \
	  	ln -s $(CDS3_TSCIOBACNET_SRC_DIR) $(CDS3_TSCIOBACNET_DIR); \
	fi
endif
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Extract.post
# ----------------------------------------------------------------------------

ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
$(STATEDIR)/cds3-tsciobacnet.extract.post:
	@$(call targetinfo)
	@$(call touch)
endif

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

$(STATEDIR)/cds3-tsciobacnet.prepare:
	@$(call targetinfo)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	@$(call world/prepare, CDS3_TSCIOBACNET)
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------
CDS3_TSCIOBACNET_MAKE_OPT  := CC=$(CROSS_CC)

CDS3_TSCIOBACNET_MAKE_OPT += "DBGMODE=-g3"
CDS3_TSCIOBACNET_MAKE_OPT += "OPTIMIZE=-O2"

ifdef PTXCONF_LIBBACNETSTACK_SOURCE_LEGACY
LIBBACNETSTACK_MAKE_OPT += "LIBBACNETSTACK_REVISION=14"
else
LIBBACNETSTACK_MAKE_OPT += "LIBBACNETSTACK_REVISION=22"
endif

$(STATEDIR)/cds3-tsciobacnet.compile:
	@$(call targetinfo)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	$(call world/compile, CDS3_TSCIOBACNET)
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

$(STATEDIR)/cds3-tsciobacnet.install:
	@$(call targetinfo)
	@$(call touch)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/cds3-tsciobacnet.targetinstall:
	@$(call targetinfo)
	@$(call install_init, cds3-tsciobacnet)

	@$(call install_fixup,cds3-tsciobacnet,PRIORITY,optional)
	@$(call install_fixup,cds3-tsciobacnet,SECTION,base)
	@$(call install_fixup,cds3-tsciobacnet,AUTHOR,"WAGO")
	@$(call install_fixup,cds3-tsciobacnet,DESCRIPTION,missing)

ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES

	# Extract precompiled binaries from archive
	rm -rf $(CDS3_TSCIOBACNET_PLATFORMCONFIGPACKAGEDIR)/tmp/*
	cd $(CDS3_TSCIOBACNET_PLATFORMCONFIGPACKAGEDIR)/tmp && \
	ar -xov $(CDS3_TSCIOBACNET_PLATFORMCONFIGPACKAGEDIR)/$(CDS3_TSCIOBACNET_PACKAGE_NAME_CP).ipk
	@$(call install_archive, cds3-tsciobacnet, 0, 0, $(CDS3_TSCIOBACNET_PLATFORMCONFIGPACKAGEDIR)/tmp/data.tar.gz, )
else
	# WAGO_TOOLS_BUILD_VERSION_TRUNK | WAGO_TOOLS_BUILD_VERSION_RELEASE

	@$(call install_copy, cds3-tsciobacnet, 0, 0, 0755, $(CDS3_TSCIOBACNET_DIR)/$(CDS3_TSCIOBACNET_BIN), /usr/lib/$(CDS3_TSCIOBACNET_BIN))
	@$(call install_link, cds3-tsciobacnet, $(CDS3_TSCIOBACNET_BIN), /usr/lib/lib$(CDS3_TSCIOBACNET_NAME).so);
	@$(call install_link, cds3-tsciobacnet, ../$(CDS3_TSCIOBACNET_BIN), /usr/lib/cds3-custom-components/lib$(CDS3_TSCIOBACNET_NAME).so);

endif
	@$(call install_finish,cds3-tsciobacnet)
ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_RELEASE
	# Backup binaries in configs/@platform@/packages/
	cp $(PKGDIR)/$(CDS3_TSCIOBACNET_PACKAGE_NAME_CP).ipk $(CDS3_TSCIOBACNET_PLATFORMCONFIGPACKAGEDIR)/
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------

$(STATEDIR)/cds3-tsciobacnet.clean:
	rm -rf $(STATEDIR)/cds3-tsciobacnet.*
	rm -rf $(PKGDIR)/cds3-tsciobacnet_*
	rm -rf $(CDS3_TSCIOBACNET_DIR)
