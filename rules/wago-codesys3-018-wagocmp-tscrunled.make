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
PACKAGES-$(PTXCONF_CDS3_TSCRUNLED) += cds3-tscrunled

CDS3_TSCRUNLED_VERSION	 := 0.0.2
CDS3_TSCRUNLED              := TscRunLed
CDS3_TSCRUNLED_DIR          := $(BUILDDIR)/$(CDS3_TSCRUNLED)
CDS3_TSCRUNLED_URL          := file://$(PTXDIST_WORKSPACE)/wago_intern/codesys3-Component/$(CDS3_TSCRUNLED)
CDS3_TSCRUNLED_SRC_DIR      := $(PTXDIST_WORKSPACE)/wago_intern/codesys3-Component/$(CDS3_TSCRUNLED)
CDS3_TSCRUNLED_SRC          := $(CDS3_TSCRUNLED_SRC_DIR)/$(CDS3_TSCRUNLED)
CDS3_TSCRUNLED_BIN          := lib$(CDS3_TSCRUNLED).so.$(CDS3_TSCRUNLED_VERSION)

CDS3_TSCRUNLED_PACKAGE_NAME := cds3-tscrunled_$(CDS3_TSCRUNLED_VERSION)_$(PTXDIST_IPKG_ARCH_STRING)
CDS3_TSCRUNLED_PLATFORMCONFIGPACKAGEDIR := $(PTXDIST_PLATFORMCONFIGDIR)/packages

#CDS3_IODRVDAL_CMP_INCLUDE:=$(PLCLINUXRT_V3_DIR)/Components/
#CDS3_IODRVDAL_PLATFORM_INCLUDE:=$(PLCLINUXRT_V3_DIR)/Platforms/Linux/

# ----------------------------------------------------------------------------
# Get
# ----------------------------------------------------------------------------

$(STATEDIR)/cds3-tscrunled.get:
	@$(call targetinfo)
	@$(call touch)

# ----------------------------------------------------------------------------
# Extract
# ----------------------------------------------------------------------------

$(STATEDIR)/cds3-tscrunled.extract:
	@$(call targetinfo)
	mkdir -p $(CDS3_TSCRUNLED_DIR)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
 	# WAGO_TOOLS_BUILD_VERSION_TRUNK | WAGO_TOOLS_BUILD_VERSION_RELEASE

#normally this is needed to use but the old schroot does not have unzip so we have to go a workaround via tar.gz
	rsync -a --exclude=.svn/ \
		--exclude=".*" \
		--exclude="*.d" \
		--exclude="*.o" \
		--exclude="*.pdf"  \
		--exclude="*tar.bz2" \
		$(CDS3_TSCRUNLED_SRC_DIR)/ $(CDS3_TSCRUNLED_DIR)/

#	$(call patchin, CDS3_IODRVDAL)
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------


$(STATEDIR)/cds3-tscrunled.prepare:
	@$(call targetinfo)
	@$(call touch)


# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------
CDS3_TSCRUNLED_PATH      := PATH=$(CROSS_PATH)
CDS3_TSCRUNLED_MAKE_ENV  := $(CROSS_ENV)
CDS3_TSCRUNLED_MAKE_OPT  := CC=$(CROSS_CC)

ifdef PTXCONF_CDS3_TSCRUNLED_TARGET_PFCXXX
	CDS3_TSCRUNLED_MAKE_ENV += TRG_PLATFORM=pfc
else
ifdef PTXCONF_CDS3_TSCRUNLED_TARGET_PAC
	CDS3_TSCRUNLED_MAKE_ENV += TRG_PLATFORM=pac
endif
endif

$(STATEDIR)/cds3-tscrunled.compile:
	@$(call targetinfo)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
#	@$(call xslt_compile, $(CDS3_TSCRUNLED_DIR)/xml/codesys3.xml)
	$(call world/compile, CDS3_TSCRUNLED)
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

$(STATEDIR)/cds3-tscrunled.install:
	@$(call targetinfo)
	@$(call touch)


# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/cds3-tscrunled.targetinstall:
	@$(call targetinfo)
	@$(call install_init, cds3-tscrunled)

	@$(call install_fixup,cds3-tscrunled,PRIORITY,optional)
	@$(call install_fixup,cds3-tscrunled,SECTION,base)
	@$(call install_fixup,cds3-tscrunled,AUTHOR,"WAGO")
	@$(call install_fixup,cds3-tscrunled,DESCRIPTION,missing)

ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES

	# Extract precompiled binaries from archive
	rm -rf $(CDS3_TSCRUNLED_PLATFORMCONFIGPACKAGEDIR)/tmp/*
	cd $(CDS3_TSCRUNLED_PLATFORMCONFIGPACKAGEDIR)/tmp && \
	ar -xov $(CDS3_TSCRUNLED_PLATFORMCONFIGPACKAGEDIR)/$(CDS3_TSCRUNLED_PACKAGE_NAME).ipk
	@$(call install_archive, cds3-tscrunled, 0, 0, $(CDS3_TSCRUNLED_PLATFORMCONFIGPACKAGEDIR)/tmp/data.tar.gz, )
else
	# WAGO_TOOLS_BUILD_VERSION_TRUNK | WAGO_TOOLS_BUILD_VERSION_RELEASE

	@$(call install_copy, cds3-tscrunled, 0, 0, 0750, $(CDS3_TSCRUNLED_DIR)/$(CDS3_TSCRUNLED_BIN), /usr/lib/$(CDS3_TSCRUNLED_BIN))
	@$(call install_link, cds3-tscrunled, ./$(CDS3_TSCRUNLED_BIN), /usr/lib/lib$(CDS3_TSCRUNLED).so);
	@$(call install_link, cds3-tscrunled, ../$(CDS3_TSCRUNLED_BIN), /usr/lib/cds3-custom-components/lib$(CDS3_TSCRUNLED).so);
	

endif
	@$(call install_finish,cds3-tscrunled)
ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_RELEASE
	# Backup binaries in configs/@platform@/packages/
	cp $(PKGDIR)/$(CDS3_TSCRUNLED_PACKAGE_NAME).ipk $(CDS3_TSCRUNLED_PLATFORMCONFIGPACKAGEDIR)/
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------

$(STATEDIR)/cds3-tscrunled.clean:
#	@$(call xslt_clean, $(CDS3_TSCRUNLED_DIR)/xml/codesys3.xml)
	rm -rf $(STATEDIR)/cds3-tscrunled.*
	rm -rf $(PKGDIR)/cds3-tscrunled_*
	rm -rf $(CDS3_TSCRUNLED_DIR)
