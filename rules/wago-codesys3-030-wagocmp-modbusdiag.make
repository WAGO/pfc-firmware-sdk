## -*-makefile-*-
# $Id: wago-codesys3-030-cmpmodbusdiag.make $
#
# Copyright (C) 2017 by Wago Kontakttechnik GmbH & Co. KG
#
# See CREDITS for details about who has contributed to this project.
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
PACKAGES-$(PTXCONF_CDS3_CMPMODBUSDIAG) += cds3-cmpmodbusdiag

CDS3_CMPMODBUSDIAG_VERSION    := 0.0.2
CDS3_CMPMODBUSDIAG            := CmpModbusDiag
CDS3_CMPMODBUSDIAG_NAME       := CmpModbusDiag
CDS3_CMPMODBUSDIAG_DIR        := $(BUILDDIR)/$(CDS3_CMPMODBUSDIAG)
CDS3_CMPMODBUSDIAG_URL        := file://$(PTXDIST_WORKSPACE)/wago_intern/codesys3-Component/$(CDS3_CMPMODBUSDIAG)
CDS3_CMPMODBUSDIAG_SRC_DIR    := $(PTXDIST_WORKSPACE)/wago_intern/codesys3-Component/$(CDS3_CMPMODBUSDIAG)
CDS3_CMPMODBUSDIAG_SRC        := $(CDS3_CMPMODBUSDIAG_SRC_DIR)/$(CDS3_CMPMODBUSDIAG_NAME)
CDS3_CMPMODBUSDIAG_BIN        := lib$(CDS3_CMPMODBUSDIAG_NAME).so.$(CDS3_CMPMODBUSDIAG_VERSION)

CDS3_CMPMODBUSDIAG_PACKAGE_NAME := cds3-cmpmodbusdiag_$(CDS3_CMPMODBUSDIAG_VERSION)_$(PTXDIST_IPKG_ARCH_STRING)
CDS3_CMPMODBUSDIAG_PLATFORMCONFIGPACKAGEDIR := $(PTXDIST_PLATFORMCONFIGDIR)/packages

# ----------------------------------------------------------------------------
# Get
# ----------------------------------------------------------------------------

$(STATEDIR)/cds3-cmpmodbusdiag.get:
	@$(call targetinfo)
	@$(call touch)

# ----------------------------------------------------------------------------
# Extract
# ----------------------------------------------------------------------------

$(STATEDIR)/cds3-cmpmodbusdiag.extract:
	@$(call targetinfo)
	mkdir -p $(CDS3_CMPMODBUSDIAG_DIR)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
 	# WAGO_TOOLS_BUILD_VERSION_TRUNK | WAGO_TOOLS_BUILD_VERSION_RELEASE

	#normally this is needed to use but the old schroot does not have unzip so we have to go a workaround via tar.gz
	rsync -a --exclude=.svn/ \
		--exclude=".*" \
		--exclude="*.d" \
		--exclude="*.o" \
		--exclude="*.pdf"  \
		--exclude="*tar.bz2" \
		$(CDS3_CMPMODBUSDIAG_SRC_DIR)/ $(CDS3_CMPMODBUSDIAG_DIR)/
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------


$(STATEDIR)/cds3-cmpmodbusdiag.prepare:
	@$(call targetinfo)
	@$(call touch)


# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------
CDS3_CMPMODBUSDIAG_PATH      := PATH=$(CROSS_PATH)
CDS3_CMPMODBUSDIAG_MAKE_ENV  := $(CROSS_ENV)
CDS3_CMPMODBUSDIAG_MAKE_OPT  := CC=$(CROSS_CC)

#CDS3_CMPMODBUSDIAG_MAKE_OPT += "DBGMODE=-g3"
#CDS3_CMPMODBUSDIAG_MAKE_OPT += "OPTIMIZE=-O2"


$(STATEDIR)/cds3-cmpmodbusdiag.compile:
	@$(call targetinfo)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	$(call world/compile, CDS3_CMPMODBUSDIAG)
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

$(STATEDIR)/cds3-cmpmodbusdiag.install:
	@$(call targetinfo)
	@$(call touch)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/cds3-cmpmodbusdiag.targetinstall:
	@$(call targetinfo)
	@$(call install_init, cds3-cmpmodbusdiag)

	@$(call install_fixup,cds3-cmpmodbusdiag,PRIORITY,optional)
	@$(call install_fixup,cds3-cmpmodbusdiag,SECTION,base)
	@$(call install_fixup,cds3-cmpmodbusdiag,AUTHOR,"WAGO")
	@$(call install_fixup,cds3-cmpmodbusdiag,DESCRIPTION,missing)

ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES

	# Extract precompiled binaries from archive
	rm -rf $(CDS3_CMPMODBUSDIAG_PLATFORMCONFIGPACKAGEDIR)/tmp/*
	cd $(CDS3_CMPMODBUSDIAG_PLATFORMCONFIGPACKAGEDIR)/tmp && \
	ar -xov $(CDS3_CMPMODBUSDIAG_PLATFORMCONFIGPACKAGEDIR)/$(CDS3_CMPMODBUSDIAG_PACKAGE_NAME).ipk
	@$(call install_archive, cds3-cmpmodbusdiag, 0, 0, $(CDS3_CMPMODBUSDIAG_PLATFORMCONFIGPACKAGEDIR)/tmp/data.tar.gz, )
else
	# WAGO_TOOLS_BUILD_VERSION_TRUNK | WAGO_TOOLS_BUILD_VERSION_RELEASE

	@$(call install_copy, cds3-cmpmodbusdiag, 0, 0, 0750, $(CDS3_CMPMODBUSDIAG_DIR)/$(CDS3_CMPMODBUSDIAG_BIN), /usr/lib/$(CDS3_CMPMODBUSDIAG_BIN))
	@$(call install_link, cds3-cmpmodbusdiag, $(CDS3_CMPMODBUSDIAG_BIN), /usr/lib/lib$(CDS3_CMPMODBUSDIAG_NAME).so);
	@$(call install_link, cds3-cmpmodbusdiag, ../$(CDS3_CMPMODBUSDIAG_BIN), /usr/lib/cds3-custom-components/lib$(CDS3_CMPMODBUSDIAG_NAME).so);

endif
	@$(call install_finish,cds3-cmpmodbusdiag)
ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_RELEASE
	# Backup binaries in configs/@platform@/packages/
	cp $(PKGDIR)/$(CDS3_CMPMODBUSDIAG_PACKAGE_NAME).ipk $(CDS3_CMPMODBUSDIAG_PLATFORMCONFIGPACKAGEDIR)/
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------

$(STATEDIR)/cds3-cmpmodbusdiag.clean:
	rm -rf $(STATEDIR)/cds3-cmpmodbusdiag.*
	rm -rf $(PKGDIR)/cds3-cmpmodbusdiag_*
	rm -rf $(CDS3_CMPMODBUSDIAG_DIR)
