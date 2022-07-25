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
PACKAGES-$(PTXCONF_CDS3_TSCDRM) += cds3-tscdrm

CDS3_TSCDRM_VERSION      := 1.3.2
CDS3_TSCDRM              := TscDrm
CDS3_TSCDRM_DIR          := $(BUILDDIR)/$(CDS3_TSCDRM)
CDS3_TSCDRM_URL          := file://$(PTXDIST_WORKSPACE)/wago_intern/codesys3-Component/$(CDS3_TSCDRM)
CDS3_TSCDRM_SRC_DIR      := $(PTXDIST_WORKSPACE)/wago_intern/codesys3-Component/$(CDS3_TSCDRM)
CDS3_TSCDRM_SRC          := $(CDS3_TSCDRM_SRC_DIR)/$(CDS3_TSCDRM)
CDS3_TSCDRM_BIN          := lib$(CDS3_TSCDRM).so.$(CDS3_TSCDRM_VERSION)

CDS3_TSCDRM_PACKAGE_NAME := cds3-tscdrm_$(CDS3_TSCDRM_VERSION)_$(PTXDIST_IPKG_ARCH_STRING)
CDS3_TSCDRM_PLATFORMCONFIGPACKAGEDIR := $(PTXDIST_PLATFORMCONFIGDIR)/packages

#CDS3_TSCDRM_CMP_INCLUDE:=$(PLCLINUXRT_V3_DIR)/Components/
#CDS3_TSCDRM_PLATFORM_INCLUDE:=$(PLCLINUXRT_V3_DIR)/Platforms/Linux/

# ----------------------------------------------------------------------------
# Get
# ----------------------------------------------------------------------------

$(STATEDIR)/cds3-tscdrm.get:
	@$(call targetinfo)
	@$(call touch)

# ----------------------------------------------------------------------------
# Extract
# ----------------------------------------------------------------------------

$(STATEDIR)/cds3-tscdrm.extract:
	@$(call targetinfo)
	mkdir -p $(CDS3_TSCDRM_DIR)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
 	# WAGO_TOOLS_BUILD_VERSION_TRUNK | WAGO_TOOLS_BUILD_VERSION_RELEASE

#normally this is needed to use but the old schroot does not have unzip so we have to go a workaround via tar.gz
	rsync -a --exclude=.svn/ \
		--exclude=".*" \
		--exclude="*.d" \
		--exclude="*.o" \
		--exclude="*.pdf"  \
		--exclude="*tar.bz2" \
		$(CDS3_TSCDRM_SRC_DIR)/ $(CDS3_TSCDRM_DIR)/

#	$(call patchin, CDS3_IODRVDAL)
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------


$(STATEDIR)/cds3-tscdrm.prepare:
	@$(call targetinfo)
	@$(call touch)


# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------
CDS3_TSCDRM_PATH      := PATH=$(CROSS_PATH)
CDS3_TSCDRM_MAKE_ENV  := $(CROSS_ENV)
CDS3_TSCDRM_MAKE_OPT  := CC=$(CROSS_CC)

ifdef PTXCONF_CDS3_TSCDRM_TARGET_PFCXXX
	CDS3_TSCDRM_MAKE_ENV += TRG_PLATFORM=pfc
else
ifdef PTXCONF_CDS3_TSCDRM_TARGET_PAC
	CDS3_TSCDRM_MAKE_ENV += TRG_PLATFORM=pac
endif
endif

$(STATEDIR)/cds3-tscdrm.compile:
	@$(call targetinfo)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
#	@$(call xslt_compile, $(CDS3_TSCDRM_DIR)/xml/codesys3.xml)
	$(call world/compile, CDS3_TSCDRM)
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

$(STATEDIR)/cds3-tscdrm.install:
	@$(call targetinfo)
	@$(call touch)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/cds3-tscdrm.targetinstall:
	@$(call targetinfo)
	@$(call install_init, cds3-tscdrm)

	@$(call install_fixup,cds3-tscdrm,PRIORITY,optional)
	@$(call install_fixup,cds3-tscdrm,SECTION,base)
	@$(call install_fixup,cds3-tscdrm,AUTHOR,"WAGO")
	@$(call install_fixup,cds3-tscdrm,DESCRIPTION,missing)

ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES

	# Extract precompiled binaries from archive
	rm -rf $(CDS3_TSCDRM_PLATFORMCONFIGPACKAGEDIR)/tmp/*
	cd $(CDS3_TSCDRM_PLATFORMCONFIGPACKAGEDIR)/tmp && \
	ar -xov $(CDS3_TSCDRM_PLATFORMCONFIGPACKAGEDIR)/$(CDS3_TSCDRM_PACKAGE_NAME).ipk
	@$(call install_archive, cds3-tscdrm, 0, 0, $(CDS3_TSCDRM_PLATFORMCONFIGPACKAGEDIR)/tmp/data.tar.gz, )
else
	# WAGO_TOOLS_BUILD_VERSION_TRUNK | WAGO_TOOLS_BUILD_VERSION_RELEASE

	@$(call install_copy, cds3-tscdrm, 0, 0, 0750, $(CDS3_TSCDRM_DIR)/$(CDS3_TSCDRM_BIN), /usr/lib/$(CDS3_TSCDRM_BIN))
	@$(call install_link, cds3-tscdrm, ./$(CDS3_TSCDRM_BIN), /usr/lib/lib$(CDS3_TSCDRM).so);
	@$(call install_link, cds3-tscdrm, ../$(CDS3_TSCDRM_BIN), /usr/lib/cds3-custom-components/lib$(CDS3_TSCDRM).so);


endif
	@$(call install_finish,cds3-tscdrm)
ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_RELEASE
	# Backup binaries in configs/@platform@/packages/
	cp $(PKGDIR)/$(CDS3_TSCDRM_PACKAGE_NAME).ipk $(CDS3_TSCDRM_PLATFORMCONFIGPACKAGEDIR)/
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------

$(STATEDIR)/cds3-tscdrm.clean:
#	@$(call xslt_clean, $(CDS3_TSCDRM_DIR)/xml/codesys3.xml)
	rm -rf $(STATEDIR)/cds3-tscdrm.*
	rm -rf $(PKGDIR)/cds3-tscdrm_*
	rm -rf $(CDS3_TSCDRM_DIR)
