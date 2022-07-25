## -*-makefile-*-
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
PACKAGES-$(PTXCONF_CDS3_IODRVDALI) += cds3-iodrvdali

CDS3_IODRVDALI_VERSION    := 0.0.1
CDS3_IODRVDALI            := IoDrvDali
CDS3_IODRVDALI_DIR        := $(BUILDDIR)/$(CDS3_IODRVDALI)
CDS3_IODRVDALI_URL        := file://$(PTXDIST_WORKSPACE)/wago_intern/codesys3-Component/$(CDS3_IODRVDALI)
CDS3_IODRVDALI_SRC_DIR    := $(PTXDIST_WORKSPACE)/wago_intern/codesys3-Component/$(CDS3_IODRVDALI)
CDS3_IODRVDALI_SRC        := $(CDS3_IODRVDALI_SRC_DIR)/$(CDS3_IODRVDALI)
CDS3_IODRVDALI_BIN        := lib$(CDS3_IODRVDALI).so.$(CDS3_IODRVDALI_VERSION)

CDS3_IODRVDALI_PACKAGE_NAME := cds3-iodrvdali_$(CDS3_IODRVDALI_VERSION)_$(PTXDIST_IPKG_ARCH_STRING)
CDS3_IODRVDALI_PLATFORMCONFIGPACKAGEDIR := $(PTXDIST_PLATFORMCONFIGDIR)/packages

#CDS3_IODRVDALI_CMP_INCLUDE:=$(PLCLINUXRT_V3_DIR)/Components/
#CDS3_IODRVDALI_PLATFORM_INCLUDE:=$(PLCLINUXRT_V3_DIR)/Platforms/Linux/

# ----------------------------------------------------------------------------
# Get
# ----------------------------------------------------------------------------

$(STATEDIR)/cds3-iodrvdali.get:
	@$(call targetinfo)
	@$(call touch)

# ----------------------------------------------------------------------------
# Extract
# ----------------------------------------------------------------------------

$(STATEDIR)/cds3-iodrvdali.extract:
	@$(call targetinfo)
	mkdir -p $(CDS3_IODRVDALI_DIR)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
 	# WAGO_TOOLS_BUILD_VERSION_TRUNK | WAGO_TOOLS_BUILD_VERSION_RELEASE

#normally this is needed to use but the old schroot does not have unzip so we have to go a workaround via tar.gz
	rsync -a --exclude=.svn/ \
		--exclude=".*" \
		--exclude="*.d" \
		--exclude="*.o" \
		--exclude="*.pdf"  \
		--exclude="*tar.bz2" \
		$(CDS3_IODRVDALI_SRC_DIR)/ $(CDS3_IODRVDALI_DIR)/

#	$(call patchin, CDS3_IODRVDALI)
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------


$(STATEDIR)/cds3-iodrvdali.prepare:
	@$(call targetinfo)
	@$(call touch)


# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------
CDS3_IODRVDALI_PATH      := PATH=$(CROSS_PATH)
CDS3_IODRVDALI_MAKE_ENV  := $(CROSS_ENV)

CDS3_IODRVDALI_MAKE_OPT += "DBGMODE=-g3"
CDS3_IODRVDALI_MAKE_OPT += "OPTIMIZE=-O0"


$(STATEDIR)/cds3-iodrvdali.compile:
	@$(call targetinfo)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	$(call world/compile, CDS3_IODRVDALI)
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

$(STATEDIR)/cds3-iodrvdali.install:
	@$(call targetinfo)
	@$(call touch)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/cds3-iodrvdali.targetinstall:
	@$(call targetinfo)
	@$(call install_init, cds3-iodrvdali)

	@$(call install_fixup,cds3-iodrvdali,PRIORITY,optional)
	@$(call install_fixup,cds3-iodrvdali,SECTION,base)
	@$(call install_fixup,cds3-iodrvdali,AUTHOR,"WAGO")
	@$(call install_fixup,cds3-iodrvdali,DESCRIPTION,missing)

ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES

	# Extract precompiled binaries from archive
	rm -rf $(CDS3_IODRVDALI_PLATFORMCONFIGPACKAGEDIR)/tmp/*
	cd $(CDS3_IODRVDALI_PLATFORMCONFIGPACKAGEDIR)/tmp && \
	ar -xov $(CDS3_IODRVDALI_PLATFORMCONFIGPACKAGEDIR)/$(CDS3_IODRVDALI_PACKAGE_NAME).ipk
	@$(call install_archive, cds3-iodrvdali, 0, 0, $(CDS3_IODRVDALI_PLATFORMCONFIGPACKAGEDIR)/tmp/data.tar.gz, )
else
	# WAGO_TOOLS_BUILD_VERSION_TRUNK | WAGO_TOOLS_BUILD_VERSION_RELEASE

	@$(call install_copy, cds3-iodrvdali, 0, 0, 0755, $(CDS3_IODRVDALI_DIR)/$(CDS3_IODRVDALI_BIN), /usr/lib/$(CDS3_IODRVDALI_BIN))
	@$(call install_link, cds3-iodrvdali, ../$(CDS3_IODRVDALI_BIN), /usr/lib/IoDrvDal/lib$(CDS3_IODRVDALI).so);

endif
	@$(call install_finish,cds3-iodrvdali)
ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_RELEASE
	# Backup binaries in configs/@platform@/packages/
	cp $(PKGDIR)/$(CDS3_IODRVDALI_PACKAGE_NAME).ipk $(CDS3_IODRVDALI_PLATFORMCONFIGPACKAGEDIR)/
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------

$(STATEDIR)/cds3-iodrvdali.clean:
	rm -rf $(STATEDIR)/cds3-iodrvdali.*
	rm -rf $(PKGDIR)/cds3-iodrvdali_*
	rm -rf $(CDS3_IODRVDALI_DIR)
