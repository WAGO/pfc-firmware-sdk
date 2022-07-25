## -*-makefile-*-
# $Id$
#
# Copyright (C) 2013 by WAGO GmbH & Co. KG
#
# See CREDITS for details about who has contributed to this project.
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
PACKAGES-$(PTXCONF_CDS3_TSCSYNCBOOTPROJECT) += cds3-tscsyncbootproject

CDS3_TSCSYNCBOOTPROJECT_VERSION	 := 1.0.0
CDS3_TSCSYNCBOOTPROJECT              := TscSyncBootproject
CDS3_TSCSYNCBOOTPROJECT_DIR          := $(BUILDDIR)/$(CDS3_TSCSYNCBOOTPROJECT)
CDS3_TSCSYNCBOOTPROJECT_URL          := file://$(PTXDIST_WORKSPACE)/wago_intern/codesys3-Component/$(CDS3_TSCSYNCBOOTPROJECT)
CDS3_TSCSYNCBOOTPROJECT_SRC_DIR      := $(PTXDIST_WORKSPACE)/wago_intern/codesys3-Component/$(CDS3_TSCSYNCBOOTPROJECT)
CDS3_TSCSYNCBOOTPROJECT_SRC          := $(CDS3_TSCSYNCBOOTPROJECT_SRC_DIR)/$(CDS3_TSCSYNCBOOTPROJECT)
CDS3_TSCSYNCBOOTPROJECT_BIN          := lib$(CDS3_TSCSYNCBOOTPROJECT).so.$(CDS3_TSCSYNCBOOTPROJECT_VERSION)

CDS3_TSCSYNCBOOTPROJECT_PACKAGE_NAME := cds3-tscsyncbootproject_$(CDS3_TSCSYNCBOOTPROJECT_VERSION)_$(PTXDIST_IPKG_ARCH_STRING)
CDS3_TSCSYNCBOOTPROJECT_PLATFORMCONFIGPACKAGEDIR := $(PTXDIST_PLATFORMCONFIGDIR)/packages

#CDS3_TSCSYNCBOOTPROJECT_CMP_INCLUDE:=$(PLCLINUXRT_V3_DIR)/Components/
#CDS3_TSCSYNCBOOTPROJECT_PLATFORM_INCLUDE:=$(PLCLINUXRT_V3_DIR)/Platforms/Linux/

# ----------------------------------------------------------------------------
# Get
# ----------------------------------------------------------------------------

$(STATEDIR)/cds3-tscsyncbootproject.get:
	@$(call targetinfo)
	@$(call touch)

# ----------------------------------------------------------------------------
# Extract
# ----------------------------------------------------------------------------

$(STATEDIR)/cds3-tscsyncbootproject.extract:
	@$(call targetinfo)
	mkdir -p $(CDS3_TSCSYNCBOOTPROJECT_DIR)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
 	# WAGO_TOOLS_BUILD_VERSION_TRUNK | WAGO_TOOLS_BUILD_VERSION_RELEASE

#normally this is needed to use but the old schroot does not have unzip so we have to go a workaround via tar.gz
	rsync -a --exclude=.svn/ \
		--exclude=".*" \
		--exclude="*.d" \
		--exclude="*.o" \
		--exclude="*.pdf"  \
		--exclude="*tar.bz2" \
		$(CDS3_TSCSYNCBOOTPROJECT_SRC_DIR)/ $(CDS3_TSCSYNCBOOTPROJECT_DIR)/

#	$(call patchin, CDS3_IODRVDAL)
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------


$(STATEDIR)/cds3-tscsyncbootproject.prepare:
	@$(call targetinfo)
	@$(call touch)


# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------
CDS3_TSCSYNCBOOTPROJECT_PATH      := PATH=$(CROSS_PATH)
CDS3_TSCSYNCBOOTPROJECT_MAKE_ENV  := $(CROSS_ENV)
CDS3_TSCSYNCBOOTPROJECT_MAKE_OPT  := CC=$(CROSS_CC)

ifdef PTXCONF_CDS3_TSCSYNCBOOTPROJECT_TARGET_PFCXXX
	CDS3_TSCSYNCBOOTPROJECT_MAKE_ENV += TRG_PLATFORM=pfc
else
ifdef PTXCONF_CDS3_TSCSYNCBOOTPROJECT_TARGET_PAC
	CDS3_TSCSYNCBOOTPROJECT_MAKE_ENV += TRG_PLATFORM=pac
endif
endif

$(STATEDIR)/cds3-tscsyncbootproject.compile:
	@$(call targetinfo)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
#	@$(call xslt_compile, $(CDS3_TSCSYNCBOOTPROJECT_DIR)/xml/codesys3.xml)
	$(call world/compile, CDS3_TSCSYNCBOOTPROJECT)
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

$(STATEDIR)/cds3-tscsyncbootproject.install:
	@$(call targetinfo)
	@$(call touch)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/cds3-tscsyncbootproject.targetinstall:
	@$(call targetinfo)
	@$(call install_init, cds3-tscsyncbootproject)

	@$(call install_fixup,cds3-tscsyncbootproject,PRIORITY,optional)
	@$(call install_fixup,cds3-tscsyncbootproject,SECTION,base)
	@$(call install_fixup,cds3-tscsyncbootproject,AUTHOR,"WAGO")
	@$(call install_fixup,cds3-tscsyncbootproject,DESCRIPTION,missing)

ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES

	# Extract precompiled binaries from archive
	rm -rf $(CDS3_TSCSYNCBOOTPROJECT_PLATFORMCONFIGPACKAGEDIR)/tmp/*
	cd $(CDS3_TSCSYNCBOOTPROJECT_PLATFORMCONFIGPACKAGEDIR)/tmp && \
	ar -xov $(CDS3_TSCSYNCBOOTPROJECT_PLATFORMCONFIGPACKAGEDIR)/$(CDS3_TSCSYNCBOOTPROJECT_PACKAGE_NAME).ipk
	@$(call install_archive, cds3-tscsyncbootproject, 0, 0, $(CDS3_TSCSYNCBOOTPROJECT_PLATFORMCONFIGPACKAGEDIR)/tmp/data.tar.gz, )
else
	# WAGO_TOOLS_BUILD_VERSION_TRUNK | WAGO_TOOLS_BUILD_VERSION_RELEASE

	@$(call install_copy, cds3-tscsyncbootproject, 0, 0, 0750, $(CDS3_TSCSYNCBOOTPROJECT_DIR)/$(CDS3_TSCSYNCBOOTPROJECT_BIN), /usr/lib/$(CDS3_TSCSYNCBOOTPROJECT_BIN))
	@$(call install_link, cds3-tscsyncbootproject, ./$(CDS3_TSCSYNCBOOTPROJECT_BIN), /usr/lib/lib$(CDS3_TSCSYNCBOOTPROJECT).so);
	@$(call install_link, cds3-tscsyncbootproject, ../$(CDS3_TSCSYNCBOOTPROJECT_BIN), /usr/lib/cds3-custom-components/lib$(CDS3_TSCSYNCBOOTPROJECT).so);


endif
	@$(call install_finish,cds3-tscsyncbootproject)
ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_RELEASE
	# Backup binaries in configs/@platform@/packages/
	cp $(PKGDIR)/$(CDS3_TSCSYNCBOOTPROJECT_PACKAGE_NAME).ipk $(CDS3_TSCSYNCBOOTPROJECT_PLATFORMCONFIGPACKAGEDIR)/
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------

$(STATEDIR)/cds3-tscsyncbootproject.clean:
#	@$(call xslt_clean, $(CDS3_TSCSYNCBOOTPROJECT_DIR)/xml/codesys3.xml)
	rm -rf $(STATEDIR)/cds3-tscsyncbootproject.*
	rm -rf $(PKGDIR)/cds3-tscsyncbootproject_*
	rm -rf $(CDS3_TSCSYNCBOOTPROJECT_DIR)
